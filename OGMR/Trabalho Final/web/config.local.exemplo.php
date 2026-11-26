<?php
/*
 * Copie para config.local.php e ajuste. Esse arquivo NÃO vai para o git.
 * Só defina o que for diferente do padrão em config.php.
 */

define('DB_SENHA', 'udesc');

// false = envia SNMP de verdade para o switch (laboratório / apresentação)
define('GERENTE_SIMULAR', true);

// ---- Desenvolvimento no Windows (XAMPP) ----------------------------------
// define('DIR_DADOS', 'C:/port-warden/dados');
// define('JAVA_BIN', 'C:/Program Files/Java/jdk-21/bin/java.exe');
// define('GERENTE_JAR', 'C:/port-warden/gerente.jar');
// define('PHP_BIN', 'C:/xampp/php/php.exe');
// define('CRON_SIMULADO', true);   // o Windows não tem crontab

// ---- Apresentação ---------------------------------------------------------
// Ninguém deve acessar como "professor" a partir do próprio servidor:
// define('MACS_FIXOS', []);
