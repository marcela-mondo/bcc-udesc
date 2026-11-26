<?php
/*
 * GET api/status.php
 * Estado das portas da sala + agendamentos. A tela chama a cada poucos segundos.
 */
declare(strict_types=1);
require __DIR__ . '/../inc/iniciar.php';

$ctx = exigir_login(true);
session_write_close(); // libera a sessão: não trava outras requisições do mesmo navegador

try {
    reconciliar();                       // executa o que o cron perdeu (servidor desligado etc.)
    $status = status_sala($ctx['sala_id']);
    $ag = agendamentos_da_sala($ctx['sala_id']);
    responder_json([
        'ok'            => true,
        'sala'          => $ctx['sala_nome'],
        'agora'         => fmt(agora()),
        'portas'        => $status['portas'],
        'erros_switch'  => $status['erros_switch'],
        'agendamentos'  => $ag['abertos'],
        'historico'     => $ag['historico'],
        'simulado'      => GERENTE_SIMULAR,
    ]);
} catch (Throwable $e) {
    error_log('port-warden status: ' . $e->getMessage());
    responder_json(['ok' => false, 'erro' => 'Erro ao consultar o status: ' . $e->getMessage()], 500);
}
