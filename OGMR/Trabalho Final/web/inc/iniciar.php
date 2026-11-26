<?php
/*
 * Carregado no topo de toda página, API e script de linha de comando.
 */
declare(strict_types=1);

require_once __DIR__ . '/../config.php';
require_once __DIR__ . '/util.php';
require_once __DIR__ . '/db.php';
require_once __DIR__ . '/gerente.php';
require_once __DIR__ . '/cron.php';
require_once __DIR__ . '/agendamentos.php';
require_once __DIR__ . '/autorizacao.php';
require_once __DIR__ . '/layout.php';
