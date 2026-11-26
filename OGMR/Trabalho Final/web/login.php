<?php
declare(strict_types=1);
require __DIR__ . '/inc/iniciar.php';

iniciar_sessao();
$maquina = exigir_maquina_autorizada();   // máquina não autorizada nem vê esta página
$erro = null;

if (($_SERVER['REQUEST_METHOD'] ?? '') === 'POST') {
    if (!csrf_valido($_POST['csrf'] ?? null)) {
        $erro = 'Sessão expirada. Tente de novo.';
    } else {
        $usuario = autenticar(trim((string) ($_POST['login'] ?? '')), (string) ($_POST['senha'] ?? ''));
        if ($usuario) {
            session_regenerate_id(true);
            $_SESSION['usuario_id'] = (int) $usuario['id'];
            $_SESSION['usuario_nome'] = $usuario['nome'];
            $_SESSION['mac'] = $maquina['mac'];
            $_SESSION['sala_id'] = (int) $maquina['sala_id'];
            unset($_SESSION['csrf']);
            registrar_log('login', true, 'login em ' . $maquina['sala_nome'], null, null, 'web', (int) $usuario['id']);
            header('Location: index.php');
            exit;
        }
        registrar_log('login', false, 'falha de login: ' . mb_substr((string) ($_POST['login'] ?? ''), 0, 50));
        $erro = 'Usuário ou senha inválidos.';
    }
}

layout_topo('Entrar');
?>
<div class="row justify-content-center">
  <div class="col-sm-9 col-md-6 col-lg-4">
    <div class="card shadow-sm">
      <div class="card-body p-4">
        <h1 class="h5 mb-1">Controle de acesso à Internet</h1>
        <p class="text-secondary small mb-4">Sala <strong><?= h($maquina['sala_nome']) ?></strong></p>

        <?php if ($erro): ?>
          <div class="alert alert-danger py-2 small" role="alert"><?= h($erro) ?></div>
        <?php endif; ?>

        <form method="post" autocomplete="off">
          <input type="hidden" name="csrf" value="<?= h(csrf_token()) ?>">
          <div class="mb-3">
            <label class="form-label" for="login">Usuário</label>
            <input class="form-control" id="login" name="login" required autofocus
                   value="<?= h($_POST['login'] ?? '') ?>">
          </div>
          <div class="mb-4">
            <label class="form-label" for="senha">Senha</label>
            <input class="form-control" id="senha" name="senha" type="password" required>
          </div>
          <button class="btn btn-primary w-100" type="submit">Entrar</button>
        </form>
      </div>
    </div>
  </div>
</div>
<?php
layout_rodape();
