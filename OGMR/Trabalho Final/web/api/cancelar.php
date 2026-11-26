<?php
/*
 * POST api/cancelar.php   (JSON, cabeçalho X-CSRF-Token)
 * { "id": 15 }          cancela um agendamento
 * { "lote": "a1b2c3" }  cancela todos do mesmo clique (ex.: "bloquear sala")
 * Se o bloqueio já estava valendo, a porta é liberada.
 */
declare(strict_types=1);
require __DIR__ . '/../inc/iniciar.php';

$ctx = exigir_login_api_post();
$entrada = ler_json_entrada();

if (!empty($entrada['lote']) && is_string($entrada['lote'])) {
    $ids = ids_do_lote($entrada['lote'], $ctx['sala_id']);
} elseif (!empty($entrada['id'])) {
    $ids = [(int) $entrada['id']];
} else {
    responder_json(['ok' => false, 'erro' => 'Informe id ou lote.'], 400);
}
if (!$ids) {
    responder_json(['ok' => false, 'erro' => 'Nada para cancelar.'], 404);
}

try {
    $resultados = cancelar_agendamentos($ids, $ctx['usuario_id'], $ctx['sala_id']);
} catch (Throwable $e) {
    error_log('port-warden cancelar: ' . $e->getMessage());
    responder_json(['ok' => false, 'erro' => 'Erro ao cancelar: ' . $e->getMessage()], 500);
}

$falhas = array_values(array_filter($resultados, fn ($x) => empty($x['ok'])));
responder_json(['ok' => count($falhas) === 0, 'total' => count($resultados), 'falhas' => count($falhas), 'resultados' => $resultados]);
