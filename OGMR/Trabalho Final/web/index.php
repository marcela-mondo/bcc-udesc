<?php
/*
 * Tela principal: portas da sala, com status atualizado automaticamente.
 * O conteúdo é desenhado pelo assets/app.js a partir de api/status.php.
 */
declare(strict_types=1);
require __DIR__ . '/inc/iniciar.php';

$ctx = exigir_login();
$inicioPadrao = arredondar_minuto_cima(agora()->modify('+5 minutes'))->format('Y-m-d\TH:i');

layout_topo('Sala ' . $ctx['sala_nome'], $ctx);
?>
<div id="app" data-intervalo="<?= (int) STATUS_INTERVALO_MS ?>">

  <div id="alertas" class="position-fixed top-0 end-0 p-3" style="z-index:1080; max-width:420px"></div>

  <div class="d-flex flex-wrap align-items-center justify-content-between gap-2 mb-3">
    <div>
      <h1 class="h4 mb-0">Sala <?= h($ctx['sala_nome']) ?></h1>
      <div class="small text-secondary">
        <span id="resumo">Carregando…</span> ·
        <span id="atualizado" title="Atualiza sozinho">—</span>
        <span id="indicador" class="indicador"></span>
      </div>
    </div>
    <input id="busca" type="search" class="form-control" style="max-width:280px"
           placeholder="Buscar por nome, IP, MAC ou porta">
  </div>

  <div id="erro-switch" class="alert alert-warning d-none"></div>

  <!-- Painel de ações -->
  <div class="card shadow-sm mb-4">
    <div class="card-body">
      <div class="row g-3 align-items-end">
        <div class="col-12 col-lg-3">
          <label class="form-label small fw-semibold mb-1">Quando bloquear</label>
          <div class="btn-group w-100" role="group">
            <input type="radio" class="btn-check" name="modo" id="modo-agora" value="agora" checked>
            <label class="btn btn-outline-secondary" for="modo-agora">Agora</label>
            <input type="radio" class="btn-check" name="modo" id="modo-agendar" value="agendar">
            <label class="btn btn-outline-secondary" for="modo-agendar">Agendar</label>
          </div>
        </div>
        <div class="col-12 col-sm-6 col-lg-3" id="campo-inicio" hidden>
          <label for="inicio" class="form-label small fw-semibold mb-1">Início</label>
          <input type="datetime-local" id="inicio" class="form-control" value="<?= h($inicioPadrao) ?>">
        </div>
        <div class="col-12 col-sm-6 col-lg-3">
          <label for="duracao" class="form-label small fw-semibold mb-1">Por quanto tempo</label>
          <div class="input-group">
            <input type="number" id="duracao" class="form-control" min="1" value="50">
            <select id="unidade" class="form-select" style="max-width:120px">
              <option value="min" selected>minutos</option>
              <option value="h">horas</option>
              <option value="d">dias</option>
            </select>
          </div>
        </div>
        <div class="col-12 col-lg">
          <div class="d-flex flex-wrap gap-2 justify-content-lg-end">
            <button id="btn-sala" class="btn btn-danger">Bloquear sala inteira</button>
            <button id="btn-bloquear" class="btn btn-outline-danger" disabled>Bloquear selecionadas</button>
            <button id="btn-desbloquear" class="btn btn-outline-success" disabled>Desbloquear selecionadas</button>
            <button id="btn-liberar-sala" class="btn btn-success">Liberar sala inteira</button>
          </div>
        </div>
      </div>
      <div class="small text-secondary mt-2">
        <span id="contagem">Nenhuma porta selecionada</span> ·
        <a href="#" id="sel-todas">selecionar todas</a> ·
        <a href="#" id="sel-nenhuma">limpar</a>
      </div>
    </div>
  </div>

  <!-- Grade de portas -->
  <div id="grade" class="row g-3 mb-4"></div>

  <!-- Agendamentos -->
  <div class="card shadow-sm">
    <div class="card-header bg-white fw-semibold">Bloqueios ativos e agendados</div>
    <div class="table-responsive">
      <table class="table table-sm align-middle mb-0">
        <thead class="table-light">
          <tr><th>Porta</th><th>Máquina</th><th>Início</th><th>Fim</th><th>Situação</th><th></th></tr>
        </thead>
        <tbody id="tabela-agendamentos">
          <tr><td colspan="6" class="text-secondary small">Carregando…</td></tr>
        </tbody>
      </table>
    </div>
  </div>
</div>
<?php
layout_rodape(['assets/app.js']);
