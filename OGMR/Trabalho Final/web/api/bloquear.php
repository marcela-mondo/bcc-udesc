<?php
/*
 * POST api/bloquear.php   (JSON, cabeçalho X-CSRF-Token)
 * {
 *   "portas":  [3, 5] | "sala",       ids de porta, ou "sala" = todas as bloqueáveis
 *   "modo":    "agora" | "agendar",
 *   "inicio":  "2026-11-24T14:00",     só no modo agendar
 *   "duracao": 50,
 *   "unidade": "min" | "h" | "d"
 * }
 */
declare(strict_types=1);
require __DIR__ . '/../inc/iniciar.php';

$ctx = exigir_login_api_post();
$entrada = ler_json_entrada();

try {
    [$inicio, $fim, $imediato] = calcular_periodo(
        (string) ($entrada['modo'] ?? ''),
        isset($entrada['inicio']) ? (string) $entrada['inicio'] : null,
        $entrada['duracao'] ?? null,
        isset($entrada['unidade']) ? (string) $entrada['unidade'] : null
    );
} catch (InvalidArgumentException $e) {
    responder_json(['ok' => false, 'erro' => $e->getMessage()], 400);
}

$portas = $entrada['portas'] ?? [];
if ($portas === 'sala') {
    $portas = array_column(
        array_filter(portas_da_sala($ctx['sala_id']), fn ($p) => (int) $p['bloqueavel'] === 1),
        'porta_id'
    );
}
if (!is_array($portas) || !$portas) {
    responder_json(['ok' => false, 'erro' => 'Selecione pelo menos uma porta.'], 400);
}

try {
    $r = criar_bloqueio($portas, $inicio, $fim, $imediato, $ctx['usuario_id'], $ctx['sala_id']);
} catch (Throwable $e) {
    error_log('port-warden bloquear: ' . $e->getMessage());
    responder_json(['ok' => false, 'erro' => 'Erro ao bloquear: ' . $e->getMessage()], 500);
}

$falhas = array_values(array_filter($r['resultados'], fn ($x) => empty($x['ok'])));
responder_json([
    'ok'         => count($falhas) === 0,
    'imediato'   => $imediato,
    'inicio'     => $r['inicio'],
    'fim'        => $r['fim'],
    'lote'       => $r['lote'],
    'total'      => count($r['resultados']),
    'falhas'     => count($falhas),
    'resultados' => $r['resultados'],
]);
