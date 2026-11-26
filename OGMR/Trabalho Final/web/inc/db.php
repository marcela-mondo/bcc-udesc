<?php
/*
 * Conexão com o banco (PDO). Use sempre consultas preparadas (com ?).
 */
declare(strict_types=1);

function db(): PDO
{
    static $pdo = null;
    if ($pdo === null) {
        $pdo = new PDO(DB_DSN, DB_USUARIO, DB_SENHA, [
            PDO::ATTR_ERRMODE            => PDO::ERRMODE_EXCEPTION,
            PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
            PDO::ATTR_EMULATE_PREPARES   => false,
        ]);
    }
    return $pdo;
}

/** Executa uma consulta preparada e devolve o statement. */
function sql(string $consulta, array $parametros = []): PDOStatement
{
    $st = db()->prepare($consulta);
    $st->execute($parametros);
    return $st;
}

/** Monta "?,?,?" para usar com IN (...). */
function marcadores(array $valores): string
{
    return implode(',', array_fill(0, count($valores), '?'));
}
