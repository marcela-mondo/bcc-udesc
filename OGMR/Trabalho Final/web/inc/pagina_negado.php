<?php
/*
 * Mostrada para quem acessa de uma máquina NÃO autorizada.
 * De propósito não mostra login nem detalhes do sistema.
 */
declare(strict_types=1);
require_once __DIR__ . '/layout.php';

layout_topo('Acesso negado');
?>
<div class="row justify-content-center">
  <div class="col-md-6">
    <div class="card shadow-sm border-danger">
      <div class="card-body text-center py-5">
        <h1 class="h4 text-danger mb-3">Acesso não autorizado</h1>
        <p class="mb-1">Este computador não tem permissão para usar o controle de acesso à Internet.</p>
        <p class="text-secondary small mb-0">Seu endereço: <?= h(ip_cliente()) ?></p>
      </div>
    </div>
  </div>
</div>
<?php
layout_rodape();
