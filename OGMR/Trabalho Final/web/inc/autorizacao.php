<?php
/*
 * Quem pode usar o sistema:
 *   1) a requisição precisa vir de uma MÁQUINA AUTORIZADA (pelo MAC);
 *      caso contrário, nem a tela de login aparece (exigência do enunciado);
 *   2) o usuário precisa fazer LOGIN;
 *   3) ele só controla a SALA da máquina autorizada de onde está acessando.
 *
 * Como descobrimos o MAC: o PHP só enxerga o IP do cliente. Como o servidor
 * e a máquina do professor estão na mesma rede local, o IP aparece na tabela
 * ARP do servidor (/proc/net/arp), que diz o MAC.
 * Se o cliente estiver em OUTRA sub-rede (passando por roteador), a ARP
 * mostra o MAC do roteador e a verificação não funciona.
 */
declare(strict_types=1);

function ip_cliente(): string
{
    return $_SERVER['REMOTE_ADDR'] ?? '';
}

/** Descobre o MAC de um IP pela tabela ARP do servidor. */
function mac_do_ip(string $ip): ?string
{
    if ($ip === '' || filter_var($ip, FILTER_VALIDATE_IP) === false) {
        return null;
    }
    if (isset(MACS_FIXOS[$ip])) {
        return normalizar_mac(MACS_FIXOS[$ip]);
    }

    // Linux: /proc/net/arp  ->  IP  HWtype  Flags  MAC  Mask  Device
    if (is_readable('/proc/net/arp')) {
        foreach (file('/proc/net/arp', FILE_IGNORE_NEW_LINES) ?: [] as $i => $linha) {
            if ($i === 0) {
                continue;
            }
            $c = preg_split('/\s+/', trim($linha));
            if (($c[0] ?? '') === $ip && ($c[2] ?? '') !== '0x0') {
                $mac = normalizar_mac($c[3] ?? null);
                if ($mac !== null && $mac !== '00:00:00:00:00:00') {
                    return $mac;
                }
            }
        }
    }

    // Linux (IPv6 ou se a ARP acima não tinha): ip neigh
    if (PHP_OS_FAMILY === 'Linux') {
        $saida = (string) shell_exec('ip neigh show ' . escapeshellarg($ip) . ' 2>/dev/null');
        if (preg_match('/lladdr\s+([0-9a-f:]{17})/i', $saida, $m)) {
            return normalizar_mac($m[1]);
        }
    }

    // Windows (desenvolvimento): arp -a 10.90.90.93
    if (PHP_OS_FAMILY === 'Windows') {
        $saida = (string) shell_exec('arp -a ' . escapeshellarg($ip));
        if (preg_match('/' . preg_quote($ip, '/') . '\s+([0-9a-f]{2}(?:-[0-9a-f]{2}){5})/i', $saida, $m)) {
            return normalizar_mac($m[1]);
        }
    }
    return null;
}

/** A máquina que está acessando é autorizada? Devolve a sala que ela controla. */
function maquina_autorizada(): ?array
{
    static $cache = false;
    if ($cache !== false) {
        return $cache;
    }
    $mac = mac_do_ip(ip_cliente());
    $cache = null;
    if ($mac !== null) {
        $linha = sql('SELECT ma.mac, ma.sala_id, s.nome AS sala_nome
                      FROM maquinas_autorizadas ma JOIN salas s ON s.id = ma.sala_id
                      WHERE ma.mac = ? AND ma.ativa = 1', [$mac])->fetch();
        $cache = $linha ?: null;
    }
    return $cache;
}

function iniciar_sessao(): void
{
    if (session_status() === PHP_SESSION_ACTIVE) {
        return;
    }
    session_name('portwarden');
    session_set_cookie_params([
        'httponly' => true,
        'samesite' => 'Strict',
        'secure'   => !empty($_SERVER['HTTPS']),
    ]);
    session_start();
}

/** Barra quem não está numa máquina autorizada. */
function exigir_maquina_autorizada(bool $api = false): array
{
    $maquina = maquina_autorizada();
    if ($maquina !== null) {
        return $maquina;
    }
    $mac = mac_do_ip(ip_cliente());
    registrar_log('acesso_negado', false, 'IP ' . ip_cliente() . ' MAC ' . ($mac ?? 'desconhecido'));
    if ($api) {
        responder_json(['ok' => false, 'erro' => 'Máquina não autorizada.'], 403);
    }
    http_response_code(403);
    require __DIR__ . '/pagina_negado.php';
    exit;
}

/**
 * Exige máquina autorizada + login. Devolve o contexto da requisição:
 * ['usuario_id', 'usuario_nome', 'sala_id', 'sala_nome', 'mac']
 */
function exigir_login(bool $api = false): array
{
    iniciar_sessao();
    $maquina = exigir_maquina_autorizada($api);

    $logado = !empty($_SESSION['usuario_id'])
        // trocou de máquina (ou a máquina mudou de sala) depois do login? exige login de novo
        && ($_SESSION['mac'] ?? null) === $maquina['mac']
        && (int) ($_SESSION['sala_id'] ?? 0) === (int) $maquina['sala_id'];

    if ($logado) {
        $ativo = sql('SELECT ativo FROM usuarios WHERE id = ?', [$_SESSION['usuario_id']])->fetchColumn();
        $logado = (int) $ativo === 1;
    }
    if (!$logado) {
        if ($api) {
            responder_json(['ok' => false, 'erro' => 'Sessão expirada. Faça login de novo.'], 401);
        }
        header('Location: login.php');
        exit;
    }
    return [
        'usuario_id'   => (int) $_SESSION['usuario_id'],
        'usuario_nome' => $_SESSION['usuario_nome'] ?? '',
        'sala_id'      => (int) $maquina['sala_id'],
        'sala_nome'    => $maquina['sala_nome'],
        'mac'          => $maquina['mac'],
    ];
}

/** Para as APIs que alteram algo (POST): exige login e token CSRF. */
function exigir_login_api_post(): array
{
    exigir_metodo('POST');
    $ctx = exigir_login(true);
    if (!csrf_valido($_SERVER['HTTP_X_CSRF_TOKEN'] ?? null)) {
        responder_json(['ok' => false, 'erro' => 'Token de segurança inválido. Recarregue a página.'], 403);
    }
    return $ctx;
}

/** Confere login e senha. Devolve o usuário ou null. */
function autenticar(string $login, string $senha): ?array
{
    $u = sql('SELECT id, login, nome, senha_hash FROM usuarios WHERE login = ? AND ativo = 1', [$login])->fetch();
    if ($u && password_verify($senha, $u['senha_hash'])) {
        return $u;
    }
    // Gasta o mesmo tempo mesmo se o usuário não existir (não revela quem existe)
    password_verify($senha, '$2y$10$7G5N5qjcaWAJjN1YkiYXluCt4KSkZGVDtRuIFVLO5v70VsR4/YEzy');
    return null;
}
