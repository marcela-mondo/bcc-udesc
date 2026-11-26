<?php
/*
 * Chamado pelo CRONTAB na hora marcada. Não é acessível pelo navegador.
 *
 *   php cli/executar.php <id_agendamento> bloquear|desbloquear
 *   php cli/executar.php reconciliar        executa o que ficou atrasado
 *   php cli/executar.php listar             mostra as linhas do port-warden no crontab
 *
 * Código de saída: 0 = ok, 1 = uso incorreto, 2 = falha.
 */
declare(strict_types=1);

if (PHP_SAPI !== 'cli') {
    http_response_code(404);
    exit;
}

require __DIR__ . '/../inc/iniciar.php';

function saida(string $msg): void
{
    echo '[' . date('Y-m-d H:i:s') . '] executar: ' . $msg . PHP_EOL;
}

$args = array_slice($argv, 1);

try {
    if (($args[0] ?? '') === 'reconciliar') {
        $feitos = reconciliar();
        saida(count($feitos) . ' agendamento(s) atrasado(s) processado(s)');
        foreach ($feitos as $f) {
            saida('  ' . $f['mensagem']);
        }
        exit(0);
    }
    if (($args[0] ?? '') === 'listar') {
        foreach (cron_listar() as $linha) {
            echo $linha, PHP_EOL;
        }
        exit(0);
    }
    if (count($args) !== 2 || !ctype_digit($args[0]) || !in_array($args[1], ['bloquear', 'desbloquear'], true)) {
        fwrite(STDERR, "uso: php executar.php <id> bloquear|desbloquear | reconciliar | listar\n");
        exit(1);
    }
    $r = executar_agendamento((int) $args[0], $args[1]);
    saida(($r['ok'] ? 'OK ' : 'FALHA ') . $r['mensagem']);
    exit($r['ok'] ? 0 : 2);
} catch (Throwable $e) {
    saida('ERRO ' . $e->getMessage());
    exit(2);
}
