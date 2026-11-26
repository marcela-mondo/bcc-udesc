<?php
declare(strict_types=1);
require __DIR__ . '/inc/iniciar.php';

iniciar_sessao();
$_SESSION = [];
session_destroy();
header('Location: login.php');
