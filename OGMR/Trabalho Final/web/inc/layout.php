<?php
/*
 * Cabeçalho e rodapé HTML comuns.
 *
 * Bootstrap: o switch da bancada NÃO tem internet, então o CSS/JS precisa
 * estar em assets/vendor/ (rode web/assets/vendor/baixar_bootstrap.sh uma vez).
 * Se o arquivo local não existir, o navegador tenta a CDN.
 */
declare(strict_types=1);

const BOOTSTRAP_VERSAO = '5.3.3';

function layout_topo(string $titulo, array $ctx = []): void
{
    $cdnCss = 'https://cdn.jsdelivr.net/npm/bootstrap@' . BOOTSTRAP_VERSAO . '/dist/css/bootstrap.min.css';
    ?>
<!doctype html>
<html lang="pt-BR">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title><?= h($titulo) ?> · Port Warden</title>
  <link rel="stylesheet" href="assets/vendor/bootstrap.min.css"
        onerror="this.onerror=null;this.href='<?= h($cdnCss) ?>'">
  <link rel="stylesheet" href="assets/app.css">
  <?php if (session_status() === PHP_SESSION_ACTIVE): ?>
  <meta name="csrf-token" content="<?= h(csrf_token()) ?>">
  <?php endif; ?>
</head>
<body class="bg-body-tertiary">
<nav class="navbar navbar-dark bg-dark mb-4">
  <div class="container">
    <span class="navbar-brand fw-semibold">
      <svg width="22" height="22" viewBox="0 0 16 16" fill="currentColor" class="me-1 align-text-bottom" aria-hidden="true">
        <path d="M8 1a2 2 0 0 1 2 2v4H6V3a2 2 0 0 1 2-2m3 6V3a3 3 0 0 0-6 0v4a2 2 0 0 0-2 2v5a2 2 0 0 0 2 2h6a2 2 0 0 0 2-2V9a2 2 0 0 0-2-2"/>
      </svg>
      Port Warden
    </span>
    <?php if (!empty($ctx['usuario_nome'])): ?>
    <span class="navbar-text text-light small">
      Sala <strong><?= h($ctx['sala_nome']) ?></strong> ·
      <?= h($ctx['usuario_nome']) ?> ·
      <a href="logout.php" class="link-light">Sair</a>
    </span>
    <?php endif; ?>
  </div>
</nav>
<main class="container pb-5">
    <?php
}

function layout_rodape(array $scripts = []): void
{
    ?>
</main>
<?php foreach ($scripts as $s): ?>
<script src="<?= h($s) ?>"></script>
<?php endforeach; ?>
</body>
</html>
    <?php
}
