<?php
/*
 * Funções pequenas usadas em todo o sistema.
 */
declare(strict_types=1);

const FORMATO_DATA = 'Y-m-d H:i:s';

/** Escapa texto para HTML (evita XSS). */
function h(?string $s): string
{
    return htmlspecialchars((string) $s, ENT_QUOTES, 'UTF-8');
}

function agora(): DateTimeImmutable
{
    return new DateTimeImmutable('now');
}

function fmt(DateTimeInterface $d): string
{
    return $d->format(FORMATO_DATA);
}

/** Zera os segundos (o cron só trabalha com minutos). */
function arredondar_minuto_baixo(DateTimeImmutable $d): DateTimeImmutable
{
    return $d->setTime((int) $d->format('H'), (int) $d->format('i'), 0);
}

/** Sobe para o próximo minuto cheio, se tiver segundos. */
function arredondar_minuto_cima(DateTimeImmutable $d): DateTimeImmutable
{
    $baixo = arredondar_minuto_baixo($d);
    return $baixo == $d ? $d : $baixo->modify('+1 minute');
}

/** 'AA-BB-CC-DD-EE-FF' / 'aabb.ccdd.eeff' -> 'aa:bb:cc:dd:ee:ff' (ou null se inválido). */
function normalizar_mac(?string $mac): ?string
{
    if ($mac === null) {
        return null;
    }
    $hex = strtolower(preg_replace('/[^0-9a-fA-F]/', '', $mac));
    if (strlen($hex) !== 12) {
        return null;
    }
    return implode(':', str_split($hex, 2));
}

/** Pasta de dados (cria se não existir). */
function dir_dados(): string
{
    if (!is_dir(DIR_DADOS)) {
        @mkdir(DIR_DADOS, 0775, true);
    }
    return DIR_DADOS;
}

function caminho_dados(string $arquivo): string
{
    return rtrim(dir_dados(), '/\\') . DIRECTORY_SEPARATOR . $arquivo;
}

// ---------------------------------------------------------------------------
// JSON (APIs)
// ---------------------------------------------------------------------------
function responder_json(array $dados, int $codigoHttp = 200): never
{
    http_response_code($codigoHttp);
    header('Content-Type: application/json; charset=utf-8');
    header('Cache-Control: no-store');
    echo json_encode($dados, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES);
    exit;
}

function ler_json_entrada(): array
{
    $dados = json_decode((string) file_get_contents('php://input'), true);
    return is_array($dados) ? $dados : [];
}

function exigir_metodo(string $metodo): void
{
    if (($_SERVER['REQUEST_METHOD'] ?? '') !== $metodo) {
        responder_json(['ok' => false, 'erro' => "Use $metodo."], 405);
    }
}

// ---------------------------------------------------------------------------
// CSRF: impede que outro site faça o navegador do professor bloquear portas
// ---------------------------------------------------------------------------
function csrf_token(): string
{
    if (empty($_SESSION['csrf'])) {
        $_SESSION['csrf'] = bin2hex(random_bytes(32));
    }
    return $_SESSION['csrf'];
}

function csrf_valido(?string $token): bool
{
    return is_string($token) && !empty($_SESSION['csrf']) && hash_equals($_SESSION['csrf'], $token);
}

// ---------------------------------------------------------------------------
// Log no banco (tabela log_acoes)
// ---------------------------------------------------------------------------
function registrar_log(string $acao, bool $sucesso, ?string $mensagem = null, ?int $portaId = null,
                       ?int $agendamentoId = null, string $origem = 'web', ?int $usuarioId = null): void
{
    try {
        sql('INSERT INTO log_acoes (data_hora, usuario_id, porta_id, agendamento_id, acao, origem, sucesso, mensagem, ip_cliente)
             VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)', [
            fmt(agora()),
            $usuarioId ?? ($_SESSION['usuario_id'] ?? null),
            $portaId,
            $agendamentoId,
            $acao,
            $origem,
            $sucesso ? 1 : 0,
            $mensagem !== null ? mb_substr($mensagem, 0, 255) : null,
            $_SERVER['REMOTE_ADDR'] ?? null,
        ]);
    } catch (Throwable $e) {
        error_log('port-warden: falha ao registrar log: ' . $e->getMessage());
    }
}
