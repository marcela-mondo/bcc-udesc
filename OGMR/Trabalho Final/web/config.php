<?php
/*
 * port-warden | web/config.php
 *
 * Configurações do sistema. NÃO edite senhas aqui: crie um arquivo
 * config.local.php (ignorado pelo git) e defina só o que quiser mudar, ex.:
 *
 *   <?php
 *   define('DB_SENHA', 'minhaSenhaDeVerdade');
 *   define('GERENTE_SIMULAR', false);
 *
 * Tudo que não for definido no config.local.php usa o padrão abaixo.
 */

if (is_file(__DIR__ . '/config.local.php')) {
    require __DIR__ . '/config.local.php';
}

date_default_timezone_set('America/Sao_Paulo');

// ---- Banco de dados (MySQL) --------------------------------------------
defined('DB_DSN')     || define('DB_DSN', 'mysql:host=localhost;dbname=portwarden;charset=utf8mb4');
defined('DB_USUARIO') || define('DB_USUARIO', 'portwarden');
defined('DB_SENHA')   || define('DB_SENHA', 'udesc');

// ---- Pastas ----------------------------------------------------------------
// Pasta FORA do site, gravável pelo www-data: logs, cache, estado do simulador.
// (Não use /tmp: o Apache do Ubuntu/Mint tem /tmp privado e o cron não o enxerga.)
defined('DIR_DADOS')   || define('DIR_DADOS', '/opt/port-warden/dados');

// ---- Gerente SNMP (Java) ---------------------------------------------------
defined('JAVA_BIN')        || define('JAVA_BIN', '/usr/bin/java');
defined('GERENTE_JAR')     || define('GERENTE_JAR', '/opt/port-warden/lib/gerente.jar');
// true = não envia SNMP de verdade (desenvolvimento sem switch)
defined('GERENTE_SIMULAR') || define('GERENTE_SIMULAR', true);

// ---- Cron ------------------------------------------------------------------
defined('PHP_BIN')       || define('PHP_BIN', '/usr/bin/php');
// true = grava as linhas num arquivo em vez do crontab (útil no Windows)
defined('CRON_SIMULADO') || define('CRON_SIMULADO', false);

// ---- Interface -------------------------------------------------------------
// De quanto em quanto tempo a tela pergunta o status das portas
defined('STATUS_INTERVALO_MS')   || define('STATUS_INTERVALO_MS', 5000);
// Por quantos segundos reaproveitar a última leitura SNMP (evita abrir o Java a cada clique)
defined('STATUS_CACHE_SEGUNDOS') || define('STATUS_CACHE_SEGUNDOS', 4);
// Duração máxima de um bloqueio
defined('DURACAO_MAXIMA_DIAS')   || define('DURACAO_MAXIMA_DIAS', 30);

// ---- Somente desenvolvimento -----------------------------------------------
// Acessos vindos destes IPs são tratados como se viessem deste MAC.
// Serve para testar abrindo o navegador DENTRO da própria VM (localhost não
// aparece na tabela ARP). DEIXE VAZIO NA APRESENTAÇÃO.
defined('MACS_FIXOS') || define('MACS_FIXOS', [
    '127.0.0.1' => '08:00:27:eb:3f:25',
    '10.0.2.15'       => '08:00:27:eb:3f:25',
]);
