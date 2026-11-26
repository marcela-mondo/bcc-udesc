<?php
/*
 * POST api/desbloquear.php   (JSON, cabeçalho X-CSRF-Token)
 * { "portas": [3, 5] | "sala" }
 * Libera as portas agora e cancela os agendamentos pendentes delas.
 */
declare(strict_types=1);
require __DIR__ . '/../inc/iniciar.php';

$ctx = exigir_login_api_post();
$entrada = ler_json_entrada();

$portas = $entrada['portas'] ?? [];
if ($portas === 'sala') {
    // Na sala inteira, só as que podem ter sido bloqueadas (não mexe em uplink/servidor)
    $portas = array_column(
        array_filter(portas_da_sala($ctx['sala_id']), fn ($p) => (int) $p['bloqueavel'] === 1),
        'porta_id'
    );
}
if (!is_array($portas) || !$portas) {
    responder_json(['ok' => false, 'erro' => 'Selecione pelo menos uma porta.'], 400);
}

try {
    $resultados = desbloquear_portas($portas, $ctx['usuario_id'], $ctx['sala_id']);
} catch (Throwable $e) {
    error_log('port-warden desbloquear: ' . $e->getMessage());
    responder_json(['ok' => false, 'erro' => 'Erro ao desbloquear: ' . $e->getMessage()], 500);
}

$falhas = array_values(array_filter($resultados, fn ($x) => empty($x['ok'])));
responder_json([
    'ok'         => count($falhas) === 0,
    'total'      => count($resultados),
    'falhas'     => count($falhas),
    'resultados' => $resultados,
]);
