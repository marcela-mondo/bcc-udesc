<?php
/*
 * Regras de negócio: bloquear, desbloquear, agendar, cancelar e executar.
 *
 * Estados de um agendamento:
 *   agendado -> ativo -> concluido
 *        \         \--> cancelado (desbloqueio manual)
 *         \--> cancelado
 *   (qualquer falha de SNMP -> erro)
 */
declare(strict_types=1);

const UNIDADES_DURACAO = ['min' => 60, 'h' => 3600, 'd' => 86400];

// ===========================================================================
// Consultas
// ===========================================================================

function portas_da_sala(int $salaId): array
{
    return sql('SELECT * FROM vw_portas WHERE sala_id = ? ORDER BY switch_nome, porta_numero', [$salaId])->fetchAll();
}

function porta_da_sala(int $portaId, int $salaId): ?array
{
    $p = sql('SELECT * FROM vw_portas WHERE porta_id = ? AND sala_id = ?', [$portaId, $salaId])->fetch();
    return $p ?: null;
}

function agendamento_completo(int $id): ?array
{
    $a = sql('SELECT a.*, v.porta_numero, v.if_index, v.switch_id, v.switch_ip, v.community_escrita,
                     v.sala_id, v.host_nome
              FROM agendamentos a JOIN vw_portas v ON v.porta_id = a.porta_id
              WHERE a.id = ?', [$id])->fetch();
    return $a ?: null;
}

/** Agendamentos pendentes/ativos (e os últimos encerrados) de uma sala. */
function agendamentos_da_sala(int $salaId, int $historico = 10): array
{
    $abertos = sql("SELECT a.id, a.lote, a.porta_id, a.inicio, a.fim, a.status, v.porta_numero, v.host_nome, u.nome AS usuario
                    FROM agendamentos a
                    JOIN vw_portas v ON v.porta_id = a.porta_id
                    LEFT JOIN usuarios u ON u.id = a.usuario_id
                    WHERE v.sala_id = ? AND a.status IN ('agendado','ativo')
                    ORDER BY a.inicio, v.porta_numero", [$salaId])->fetchAll();
    $fechados = sql("SELECT a.id, a.lote, a.porta_id, a.inicio, a.fim, a.status, v.porta_numero, v.host_nome, u.nome AS usuario
                     FROM agendamentos a
                     JOIN vw_portas v ON v.porta_id = a.porta_id
                     LEFT JOIN usuarios u ON u.id = a.usuario_id
                     WHERE v.sala_id = ? AND a.status IN ('concluido','cancelado','erro')
                     ORDER BY a.atualizado_em DESC, a.id DESC
                     LIMIT " . (int) $historico, [$salaId])->fetchAll();
    return ['abertos' => $abertos, 'historico' => $fechados];
}

/** Existe OUTRO bloqueio valendo agora para a mesma porta? */
function outro_bloqueio_ativo(int $portaId, int $excetoId): bool
{
    $agora = fmt(agora());
    return (bool) sql("SELECT 1 FROM agendamentos
                       WHERE porta_id = ? AND id <> ?
                         AND (status = 'ativo' OR (status = 'agendado' AND inicio <= ?))
                         AND fim > ?
                       LIMIT 1", [$portaId, $excetoId, $agora, $agora])->fetchColumn();
}

function mudar_status(int $id, string $status): void
{
    sql('UPDATE agendamentos SET status = ?, atualizado_em = ? WHERE id = ?', [$status, fmt(agora()), $id]);
}

// ===========================================================================
// Bloquear (agora ou agendado)
// ===========================================================================

/**
 * Calcula início e fim a partir do que veio da tela.
 * @return array{0: DateTimeImmutable, 1: DateTimeImmutable, 2: bool} [inicio, fim, imediato]
 */
function calcular_periodo(string $modo, ?string $inicioTexto, $duracao, ?string $unidade): array
{
    $agora = agora();
    if (!isset(UNIDADES_DURACAO[$unidade ?? ''])) {
        throw new InvalidArgumentException('Unidade de duração inválida (use min, h ou d).');
    }
    $duracao = filter_var($duracao, FILTER_VALIDATE_INT);
    if ($duracao === false || $duracao < 1) {
        throw new InvalidArgumentException('Informe uma duração de pelo menos 1.');
    }
    $segundos = $duracao * UNIDADES_DURACAO[$unidade];
    if ($segundos > DURACAO_MAXIMA_DIAS * 86400) {
        throw new InvalidArgumentException('Duração máxima: ' . DURACAO_MAXIMA_DIAS . ' dias.');
    }

    if ($modo === 'agora') {
        $inicio = $agora;
        $imediato = true;
    } elseif ($modo === 'agendar') {
        $inicio = $inicioTexto ? DateTimeImmutable::createFromFormat('Y-m-d\TH:i', $inicioTexto) : false;
        if (!$inicio) {
            throw new InvalidArgumentException('Data/hora de início inválida.');
        }
        $inicio = arredondar_minuto_baixo($inicio);
        $imediato = $inicio <= $agora;   // escolheu um horário que já passou: bloqueia já
        if ($imediato) {
            $inicio = $agora;
        }
    } else {
        throw new InvalidArgumentException('Modo inválido (use agora ou agendar).');
    }

    // O cron só dispara em minutos cheios: o fim sobe para o próximo minuto
    $fim = arredondar_minuto_cima($inicio->modify("+$segundos seconds"));
    // Início e fim no mesmo minuto: o cron não garante a ordem -> pelo menos 1 minuto
    if (arredondar_minuto_baixo($fim) <= arredondar_minuto_baixo($inicio)) {
        $fim = arredondar_minuto_baixo($inicio)->modify('+1 minute');
    }
    return [$inicio, $fim, $imediato];
}

/**
 * Bloqueia (ou agenda o bloqueio de) várias portas da sala.
 * Devolve um resultado por porta.
 */
function criar_bloqueio(array $portaIds, DateTimeImmutable $inicio, DateTimeImmutable $fim, bool $imediato,
                        int $usuarioId, int $salaId): array
{
    $lote = bin2hex(random_bytes(6));
    $resultados = [];

    foreach (array_unique(array_map('intval', $portaIds)) as $portaId) {
        $p = porta_da_sala($portaId, $salaId);
        if ($p === null) {
            $resultados[] = ['porta_id' => $portaId, 'ok' => false, 'erro' => 'Porta não pertence à sua sala.'];
            registrar_log('bloquear', false, 'porta de outra sala', null, null, 'web', $usuarioId);
            continue;
        }
        if (!(int) $p['bloqueavel']) {
            $motivo = (int) $p['protegida'] ? 'porta protegida' : ((int) $p['com_defeito'] ? 'porta com defeito' : 'porta não bloqueável');
            $resultados[] = ['porta_id' => $portaId, 'porta' => (int) $p['porta_numero'], 'ok' => false, 'erro' => ucfirst($motivo) . '.'];
            registrar_log('bloquear', false, $motivo, $portaId, null, 'web', $usuarioId);
            continue;
        }

        sql("INSERT INTO agendamentos (porta_id, usuario_id, lote, inicio, fim, status, criado_em, atualizado_em)
             VALUES (?, ?, ?, ?, ?, 'agendado', ?, ?)",
            [$portaId, $usuarioId, $lote, fmt($inicio), fmt($fim), fmt(agora()), fmt(agora())]);
        $agId = (int) db()->lastInsertId();
        $r = ['porta_id' => $portaId, 'porta' => (int) $p['porta_numero'], 'agendamento_id' => $agId];

        try {
            if ($imediato) {
                $snmp = gerente('bloquear', $p['switch_ip'], (int) $p['if_index'], $p['community_escrita']);
                limpar_cache_status((int) $p['switch_id']);
                if (!empty($snmp['ok'])) {
                    mudar_status($agId, 'ativo');
                    cron_adicionar($fim, $agId, 'desbloquear');
                    $r += ['ok' => true, 'status' => 'ativo'];
                } else {
                    mudar_status($agId, 'erro');
                    $r += ['ok' => false, 'erro' => $snmp['erro'] ?? 'falha no SNMP'];
                }
                registrar_log('bloquear', !empty($snmp['ok']), $snmp['erro'] ?? 'imediato até ' . fmt($fim), $portaId, $agId, 'web', $usuarioId);
            } else {
                cron_adicionar($inicio, $agId, 'bloquear');
                cron_adicionar($fim, $agId, 'desbloquear');
                $r += ['ok' => true, 'status' => 'agendado'];
                registrar_log('bloquear', true, 'agendado ' . fmt($inicio) . ' até ' . fmt($fim), $portaId, $agId, 'web', $usuarioId);
            }
        } catch (Throwable $e) {
            // Sem cron, a porta poderia ficar bloqueada para sempre: desfaz
            if ($imediato) {
                gerente('desbloquear', $p['switch_ip'], (int) $p['if_index'], $p['community_escrita']);
            }
            cron_remover($agId);
            mudar_status($agId, 'erro');
            $r += ['ok' => false, 'erro' => 'Falha ao agendar no cron: ' . $e->getMessage()];
            registrar_log('bloquear', false, 'cron: ' . $e->getMessage(), $portaId, $agId, 'web', $usuarioId);
        }
        $resultados[] = $r;
    }
    return ['lote' => $lote, 'inicio' => fmt($inicio), 'fim' => fmt($fim), 'resultados' => $resultados];
}

// ===========================================================================
// Desbloquear / cancelar (ação do professor)
// ===========================================================================

/** Libera portas agora e cancela tudo que estava pendente para elas. */
function desbloquear_portas(array $portaIds, int $usuarioId, int $salaId): array
{
    $resultados = [];
    foreach (array_unique(array_map('intval', $portaIds)) as $portaId) {
        $p = porta_da_sala($portaId, $salaId);
        if ($p === null) {
            $resultados[] = ['porta_id' => $portaId, 'ok' => false, 'erro' => 'Porta não pertence à sua sala.'];
            continue;
        }
        // 1) cancela agendamentos pendentes/ativos e tira do cron
        $pendentes = sql("SELECT id FROM agendamentos WHERE porta_id = ? AND status IN ('agendado','ativo')", [$portaId])
            ->fetchAll(PDO::FETCH_COLUMN);
        foreach ($pendentes as $agId) {
            cron_remover((int) $agId);
            mudar_status((int) $agId, 'cancelado');
        }
        // 2) liga a porta
        $snmp = gerente('desbloquear', $p['switch_ip'], (int) $p['if_index'], $p['community_escrita']);
        limpar_cache_status((int) $p['switch_id']);
        registrar_log('desbloquear', !empty($snmp['ok']), $snmp['erro'] ?? 'manual', $portaId, null, 'web', $usuarioId);
        $resultados[] = ['porta_id' => $portaId, 'porta' => (int) $p['porta_numero'], 'ok' => !empty($snmp['ok']),
                         'erro' => $snmp['erro'] ?? null, 'cancelados' => count($pendentes)];
    }
    return $resultados;
}

/** Cancela um agendamento (ou um lote inteiro). Se estiver ativo, libera a porta. */
function cancelar_agendamentos(array $ids, int $usuarioId, int $salaId): array
{
    $resultados = [];
    foreach (array_unique(array_map('intval', $ids)) as $id) {
        $a = agendamento_completo($id);
        if ($a === null || (int) $a['sala_id'] !== $salaId) {
            $resultados[] = ['id' => $id, 'ok' => false, 'erro' => 'Agendamento não encontrado na sua sala.'];
            continue;
        }
        if (!in_array($a['status'], ['agendado', 'ativo'], true)) {
            $resultados[] = ['id' => $id, 'ok' => false, 'erro' => 'Agendamento já encerrado.'];
            continue;
        }
        cron_remover($id);
        mudar_status($id, 'cancelado');
        $r = ['id' => $id, 'ok' => true];
        if ($a['status'] === 'ativo' && !outro_bloqueio_ativo((int) $a['porta_id'], $id)) {
            $snmp = gerente('desbloquear', $a['switch_ip'], (int) $a['if_index'], $a['community_escrita']);
            limpar_cache_status((int) $a['switch_id']);
            $r['ok'] = !empty($snmp['ok']);
            $r['erro'] = $snmp['erro'] ?? null;
        }
        registrar_log('desbloquear', $r['ok'], 'cancelado agendamento ' . $id, (int) $a['porta_id'], $id, 'web', $usuarioId);
        $resultados[] = $r;
    }
    return $resultados;
}

function ids_do_lote(string $lote, int $salaId): array
{
    return sql("SELECT a.id FROM agendamentos a JOIN vw_portas v ON v.porta_id = a.porta_id
                WHERE a.lote = ? AND v.sala_id = ? AND a.status IN ('agendado','ativo')", [$lote, $salaId])
        ->fetchAll(PDO::FETCH_COLUMN);
}

// ===========================================================================
// Execução pelo cron (cli/executar.php)
// ===========================================================================

/**
 * Executa a ação de um agendamento na hora marcada.
 * @return array ['ok' => bool, 'mensagem' => string]
 */
function executar_agendamento(int $id, string $acao): array
{
    $a = agendamento_completo($id);
    // A linha do cron é sempre removida: o cron não tem ano e rodaria de novo
    cron_remover($id, $acao);

    if ($a === null) {
        return ['ok' => false, 'mensagem' => "agendamento $id não existe (linha órfã removida)"];
    }
    $agora = agora();
    $fim = new DateTimeImmutable($a['fim']);

    if ($acao === 'bloquear') {
        if ($a['status'] !== 'agendado') {
            return ['ok' => true, 'mensagem' => "agendamento $id está '{$a['status']}': nada a fazer"];
        }
        if ($agora >= $fim) {
            mudar_status($id, 'concluido');
            cron_remover($id);
            return ['ok' => true, 'mensagem' => "agendamento $id já passou do fim: não bloqueado"];
        }
        $snmp = gerente('bloquear', $a['switch_ip'], (int) $a['if_index'], $a['community_escrita']);
        limpar_cache_status((int) $a['switch_id']);
        mudar_status($id, !empty($snmp['ok']) ? 'ativo' : 'erro');
        registrar_log('bloquear', !empty($snmp['ok']), $snmp['erro'] ?? 'agendado', (int) $a['porta_id'], $id, 'cron', (int) $a['usuario_id'] ?: null);
        return ['ok' => !empty($snmp['ok']), 'mensagem' => "bloquear porta {$a['porta_numero']}: " . ($snmp['erro'] ?? 'ok')];
    }

    if ($acao === 'desbloquear') {
        if (!in_array($a['status'], ['agendado', 'ativo', 'erro'], true)) {
            return ['ok' => true, 'mensagem' => "agendamento $id está '{$a['status']}': nada a fazer"];
        }
        // Outro bloqueio ainda vale para esta porta? Então não libera.
        if (outro_bloqueio_ativo((int) $a['porta_id'], $id)) {
            mudar_status($id, 'concluido');
            return ['ok' => true, 'mensagem' => "porta {$a['porta_numero']} continua bloqueada por outro agendamento"];
        }
        $snmp = gerente('desbloquear', $a['switch_ip'], (int) $a['if_index'], $a['community_escrita']);
        limpar_cache_status((int) $a['switch_id']);
        mudar_status($id, !empty($snmp['ok']) ? 'concluido' : 'erro');
        registrar_log('desbloquear', !empty($snmp['ok']), $snmp['erro'] ?? 'fim do prazo', (int) $a['porta_id'], $id, 'cron', (int) $a['usuario_id'] ?: null);
        return ['ok' => !empty($snmp['ok']), 'mensagem' => "desbloquear porta {$a['porta_numero']}: " . ($snmp['erro'] ?? 'ok')];
    }

    return ['ok' => false, 'mensagem' => "ação inválida: $acao"];
}

/**
 * Rede de segurança: se o servidor estava desligado na hora do cron,
 * executa o que ficou para trás (com 2 min de folga para o cron agir antes).
 */
function reconciliar(): array
{
    $limite = fmt(agora()->modify('-2 minutes'));
    $feitos = [];
    $atrasadosFim = sql("SELECT id FROM agendamentos WHERE status IN ('ativo','agendado') AND fim < ?", [$limite])
        ->fetchAll(PDO::FETCH_COLUMN);
    foreach ($atrasadosFim as $id) {
        $feitos[] = executar_agendamento((int) $id, 'desbloquear');
    }
    $atrasadosInicio = sql("SELECT id FROM agendamentos WHERE status = 'agendado' AND inicio < ? AND fim > ?",
        [$limite, fmt(agora())])->fetchAll(PDO::FETCH_COLUMN);
    foreach ($atrasadosInicio as $id) {
        $feitos[] = executar_agendamento((int) $id, 'bloquear');
    }
    return $feitos;
}

// ===========================================================================
// Status da sala para a tela (banco + leitura SNMP ao vivo)
// ===========================================================================

function status_sala(int $salaId): array
{
    $portas = portas_da_sala($salaId);

    // Uma leitura SNMP por switch da sala (uma sala pode ter vários)
    $leituras = [];
    $erros = [];
    foreach ($portas as $p) {
        $sid = (int) $p['switch_id'];
        if (isset($leituras[$sid])) {
            continue;
        }
        $st = status_switch(['id' => $sid, 'ip' => $p['switch_ip'], 'community_leitura' => $p['community_leitura']]);
        $porIf = [];
        foreach ($st['portas'] ?? [] as $linha) {
            $porIf[(int) $linha['if_index']] = $linha;
        }
        $leituras[$sid] = $porIf;
        if (empty($st['ok'])) {
            $erros[] = $p['switch_nome'] . ' (' . $p['switch_ip'] . '): ' . ($st['erro'] ?? 'sem resposta');
        }
    }

    // Agendamentos abertos por porta
    $abertos = [];
    foreach (agendamentos_da_sala($salaId, 0)['abertos'] as $a) {
        $abertos[(int) $a['porta_id']][] = $a;
    }

    $saida = [];
    foreach ($portas as $p) {
        $leitura = $leituras[(int) $p['switch_id']][(int) $p['if_index']] ?? null;
        $ativo = null;
        $proximo = null;
        foreach ($abertos[(int) $p['porta_id']] ?? [] as $a) {
            if ($a['status'] === 'ativo' && ($ativo === null || $a['fim'] > $ativo['fim'])) {
                $ativo = $a;
            }
            if ($a['status'] === 'agendado' && ($proximo === null || $a['inicio'] < $proximo['inicio'])) {
                $proximo = $a;
            }
        }
        $saida[] = [
            'porta_id'     => (int) $p['porta_id'],
            'porta'        => (int) $p['porta_numero'],
            'if_index'     => (int) $p['if_index'],
            'switch'       => $p['switch_nome'],
            'tipo'         => $p['tipo'],
            'protegida'    => (bool) (int) $p['protegida'],
            'com_defeito'  => (bool) (int) $p['com_defeito'],
            'bloqueavel'   => (bool) (int) $p['bloqueavel'],
            'observacao'   => $p['observacao'],
            'host'         => $p['host_nome'],
            'ip'           => $p['host_ip'],
            'mac'          => $p['host_mac'],
            'admin'        => $leitura['admin'] ?? 'desconhecido',  // estado real lido do switch
            'link'         => $leitura['oper'] ?? null,
            'bloqueado_ate' => $ativo['fim'] ?? null,
            'agendado_para' => $proximo['inicio'] ?? null,
        ];
    }
    return ['portas' => $saida, 'erros_switch' => $erros];
}
