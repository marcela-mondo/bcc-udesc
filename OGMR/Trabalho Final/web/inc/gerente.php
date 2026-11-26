<?php
/*
 * Ponte entre o PHP e o gerente SNMP em Java (gerente.jar).
 * O gerente sempre devolve um JSON; aqui ele vira um array PHP.
 */
declare(strict_types=1);

/**
 * @param string      $comando   bloquear | desbloquear | consultar | status | macs
 * @return array      ['ok' => bool, ... , 'erro' => '...']
 */
function gerente(string $comando, string $ipSwitch, ?int $ifIndex = null, ?string $community = null): array
{
    $args = [JAVA_BIN, '-jar', GERENTE_JAR];
    if (GERENTE_SIMULAR) {
        $args[] = '--simular';
        // Arquivo de estado compartilhado entre o Apache e o cron
        putenv('PORTWARDEN_SIM_ARQ=' . caminho_dados('switch_simulado.properties'));
    }
    if ($community !== null && $community !== '') {
        $args[] = '-c';
        $args[] = $community;
    }
    $args[] = $comando;
    $args[] = $ipSwitch;
    if ($ifIndex !== null) {
        $args[] = (string) $ifIndex;
    }

    // escapeshellarg em TUDO: nada vindo do navegador entra cru no comando
    $cmd = implode(' ', array_map('escapeshellarg', $args))
         . ' 2>> ' . escapeshellarg(caminho_log_gerente());

    $saida = [];
    $codigo = -1;
    exec($cmd, $saida, $codigo);

    $resultado = json_decode(implode("\n", $saida), true);
    if (!is_array($resultado)) {
        $resultado = [
            'ok'   => false,
            'erro' => 'O gerente não respondeu. Confira JAVA_BIN e GERENTE_JAR no config.php '
                    . '(código de saída ' . $codigo . ').',
        ];
    }
    if ($codigo !== 0) {
        $resultado['ok'] = false;
    }
    $resultado['codigo_saida'] = $codigo;
    return $resultado;
}

/** Log do gerente. Se a pasta não for gravável, usa a pasta temporária (senão o shell nem roda o comando). */
function caminho_log_gerente(): string
{
    $arquivo = caminho_dados('gerente.log');
    $pasta = dirname($arquivo);
    if ((is_file($arquivo) && is_writable($arquivo)) || (!is_file($arquivo) && is_writable($pasta))) {
        return $arquivo;
    }
    return sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'port-warden-gerente.log';
}

/**
 * Status de todas as portas de um switch, com cache curto em arquivo
 * (abrir o Java leva ~0,5 s; vários navegadores atualizando a cada 5 s somariam).
 */
function status_switch(array $switch): array
{
    $cache = caminho_dados('cache_status_switch_' . (int) $switch['id'] . '.json');
    if (is_file($cache) && (time() - filemtime($cache)) < STATUS_CACHE_SEGUNDOS) {
        $dados = json_decode((string) file_get_contents($cache), true);
        if (is_array($dados)) {
            return $dados;
        }
    }
    $dados = gerente('status', $switch['ip'], null, $switch['community_leitura']);
    if (!empty($dados['ok'])) {
        @file_put_contents($cache, json_encode($dados), LOCK_EX);
    }
    return $dados;
}

/** Depois de um SET, a leitura em cache fica velha: apaga. */
function limpar_cache_status(int $switchId): void
{
    @unlink(caminho_dados('cache_status_switch_' . $switchId . '.json'));
}
