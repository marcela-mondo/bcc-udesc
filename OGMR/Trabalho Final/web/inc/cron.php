<?php
/*
 * Agendamento pelo crontab (exigência do enunciado).
 *
 * Cada agendamento gera até duas linhas no crontab do usuário do Apache
 * (www-data), marcadas no fim com  # port-warden:<id>:<acao>
 *
 *   30 14 24 11 * /usr/bin/php /var/www/html/port-warden/cli/executar.php 15 bloquear >> ... 2>&1 # port-warden:15:bloquear
 *
 * O script executar.php apaga a própria linha depois de rodar, porque o
 * cron não tem campo de ano (a linha rodaria de novo no ano seguinte).
 *
 * Com CRON_SIMULADO = true (Windows), as linhas vão para um arquivo texto
 * e nada é executado automaticamente: rode o executar.php à mão.
 */
declare(strict_types=1);

const CRON_MARCADOR = '# port-warden:';

function cron_arquivo_simulado(): string
{
    return caminho_dados('crontab_simulado.txt');
}

/** Lê todas as linhas do crontab do usuário atual. */
function cron_ler(): array
{
    if (CRON_SIMULADO) {
        $arq = cron_arquivo_simulado();
        return is_file($arq) ? file($arq, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES) : [];
    }
    $saida = [];
    $codigo = 0;
    exec('crontab -l 2>&1', $saida, $codigo);
    if ($codigo !== 0) {
        // "no crontab for www-data" = ainda vazio, não é erro
        if (stripos(implode(' ', $saida), 'no crontab') !== false) {
            return [];
        }
        throw new RuntimeException('crontab -l falhou: ' . implode(' ', $saida));
    }
    return $saida;
}

/** Substitui o crontab inteiro pelas linhas dadas. */
function cron_gravar(array $linhas): void
{
    $conteudo = $linhas ? implode("\n", $linhas) . "\n" : '';
    if (CRON_SIMULADO) {
        file_put_contents(cron_arquivo_simulado(), $conteudo, LOCK_EX);
        return;
    }
    $proc = proc_open('crontab -', [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
    if (!is_resource($proc)) {
        throw new RuntimeException('não foi possível executar o comando crontab');
    }
    fwrite($pipes[0], $conteudo);
    fclose($pipes[0]);
    $erro = stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    if (proc_close($proc) !== 0) {
        throw new RuntimeException('crontab - falhou: ' . trim((string) $erro));
    }
}

/**
 * Lê, altera e grava o crontab com uma trava de arquivo, para que o site e
 * o próprio cron não se atropelem mexendo ao mesmo tempo.
 */
function cron_alterar(callable $alteracao): void
{
    $trava = fopen(caminho_dados('crontab.lock'), 'c');
    if ($trava === false) {
        throw new RuntimeException('não foi possível criar a trava do crontab em ' . DIR_DADOS);
    }
    flock($trava, LOCK_EX);
    try {
        $linhas = cron_ler();
        $novas = $alteracao($linhas);
        if ($novas !== $linhas) {
            cron_gravar(array_values($novas));
        }
    } finally {
        flock($trava, LOCK_UN);
        fclose($trava);
    }
}

/** Monta a linha do crontab para um agendamento. */
function cron_linha(DateTimeInterface $quando, int $agendamentoId, string $acao): string
{
    $script = realpath(__DIR__ . '/../cli/executar.php') ?: (__DIR__ . '/../cli/executar.php');
    $log = caminho_dados('cron.log');
    foreach ([PHP_BIN, $script, $log] as $caminho) {
        if (str_contains($caminho, '%')) {
            // '%' vira quebra de linha no crontab
            throw new RuntimeException("caminho com '%' não pode ir para o crontab: $caminho");
        }
    }
    return sprintf('%d %d %d %d * %s %s %d %s >> %s 2>&1 %s%d:%s',
        (int) $quando->format('i'),
        (int) $quando->format('G'),
        (int) $quando->format('j'),
        (int) $quando->format('n'),
        escapeshellarg(PHP_BIN),
        escapeshellarg($script),
        $agendamentoId,
        $acao,
        escapeshellarg($log),
        CRON_MARCADOR,
        $agendamentoId,
        $acao
    );
}

function cron_adicionar(DateTimeInterface $quando, int $agendamentoId, string $acao): void
{
    $nova = cron_linha($quando, $agendamentoId, $acao);
    cron_alterar(function (array $linhas) use ($nova, $agendamentoId, $acao) {
        $linhas = cron_filtrar($linhas, $agendamentoId, $acao); // evita duplicar
        $linhas[] = $nova;
        return $linhas;
    });
}

/** Remove as linhas de um agendamento (todas, ou só de uma ação). */
function cron_remover(int $agendamentoId, ?string $acao = null): void
{
    cron_alterar(fn (array $linhas) => cron_filtrar($linhas, $agendamentoId, $acao));
}

function cron_filtrar(array $linhas, int $agendamentoId, ?string $acao): array
{
    return array_values(array_filter($linhas, function (string $linha) use ($agendamentoId, $acao) {
        if (!preg_match('/# port-warden:(\d+):(\w+)\s*$/', $linha, $m)) {
            return true; // linha de outra coisa: mantém
        }
        if ((int) $m[1] !== $agendamentoId) {
            return true;
        }
        return $acao !== null && $m[2] !== $acao;
    }));
}

/** Só as linhas do port-warden (para depuração). */
function cron_listar(): array
{
    return array_values(array_filter(cron_ler(), fn ($l) => str_contains($l, CRON_MARCADOR)));
}
