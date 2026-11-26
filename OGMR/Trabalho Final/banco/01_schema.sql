-- =====================================================================
-- port-warden | 01_schema.sql
-- Estrutura (tabelas e view) do banco de configuração.
-- Executar:  mysql -u portwarden -p portwarden < 01_schema.sql
--
-- Ideia central:
--   switch --(tem)--> portas --(ficam em)--> sala
--   host (computador) --(está ligado em)--> porta
--   máquina autorizada (MAC) --(controla)--> sala
--
-- Uma sala pode ser atendida por vários switches e um switch pode atender
-- várias salas: isso sai naturalmente de "cada PORTA pertence a uma sala".
-- =====================================================================

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

DROP VIEW  IF EXISTS vw_portas;
DROP TABLE IF EXISTS log_acoes;
DROP TABLE IF EXISTS agendamentos;
DROP TABLE IF EXISTS usuarios;
DROP TABLE IF EXISTS maquinas_autorizadas;
DROP TABLE IF EXISTS hosts;
DROP TABLE IF EXISTS portas;
DROP TABLE IF EXISTS switches;
DROP TABLE IF EXISTS salas;

SET FOREIGN_KEY_CHECKS = 1;

-- ---------------------------------------------------------------------
-- salas: ambientes físicos (ex.: F203). É a unidade de "bloquear tudo".
-- ---------------------------------------------------------------------
CREATE TABLE salas (
  id         INT AUTO_INCREMENT PRIMARY KEY,
  nome       VARCHAR(50)  NOT NULL UNIQUE,      -- ex.: 'F203'
  descricao  VARCHAR(255) NULL
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------
-- switches: equipamentos gerenciados via SNMP.
-- ---------------------------------------------------------------------
CREATE TABLE switches (
  id                 INT AUTO_INCREMENT PRIMARY KEY,
  nome               VARCHAR(50)  NOT NULL UNIQUE,   -- apelido, ex.: 'SW-BANCADA'
  modelo             VARCHAR(50)  NULL,              -- ex.: 'D-Link DGS-3100-24'
  patrimonio         VARCHAR(30)  NULL,              -- ex.: 'UDESC 104818'
  ip                 VARCHAR(45)  NOT NULL UNIQUE,   -- ex.: '10.90.90.90'
  porta_snmp         INT          NOT NULL DEFAULT 161,
  community_leitura  VARCHAR(64)  NOT NULL DEFAULT 'public',
  community_escrita  VARCHAR(64)  NOT NULL DEFAULT 'private',
  qtd_portas         INT          NOT NULL DEFAULT 24,
  localizacao        VARCHAR(100) NULL
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------
-- portas: cada porta física de cada switch.
--   numero   = número impresso na frente do switch (o que o humano vê)
--   if_index = índice da interface no SNMP (o que vai no OID
--              1.3.6.1.2.1.2.2.1.7.<if_index>). CONFIRMAR COM WALK!
--   tipo     = para que a porta é usada
--   protegida = 1  -> o sistema NUNCA bloqueia (uplink, servidor, professor)
--   com_defeito = 1 -> não aparece como opção de bloqueio
-- ---------------------------------------------------------------------
CREATE TABLE portas (
  id           INT AUTO_INCREMENT PRIMARY KEY,
  switch_id    INT         NOT NULL,
  numero       INT         NOT NULL,
  if_index     INT         NOT NULL,
  sala_id      INT         NULL,
  tipo         ENUM('host','autorizada','servidor','uplink','livre')
               NOT NULL DEFAULT 'livre',
  protegida    TINYINT(1)  NOT NULL DEFAULT 0,
  com_defeito  TINYINT(1)  NOT NULL DEFAULT 0,
  observacao   VARCHAR(255) NULL,

  CONSTRAINT fk_portas_switch FOREIGN KEY (switch_id) REFERENCES switches(id)
    ON DELETE CASCADE,
  CONSTRAINT fk_portas_sala   FOREIGN KEY (sala_id)   REFERENCES salas(id)
    ON DELETE SET NULL,
  CONSTRAINT uq_porta_numero  UNIQUE (switch_id, numero),
  CONSTRAINT uq_porta_ifindex UNIQUE (switch_id, if_index)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------
-- hosts: computadores conhecidos. É por aqui que o professor bloqueia
-- "pelo nome, IP ou MAC" (exigência do enunciado).
--   porta_id NULL = computador cadastrado mas desconectado no momento.
-- MAC sempre em minúsculas com ':'  (ex.: 'aa:bb:cc:dd:ee:ff').
-- ---------------------------------------------------------------------
CREATE TABLE hosts (
  id        INT AUTO_INCREMENT PRIMARY KEY,
  nome      VARCHAR(50)  NOT NULL,
  ip        VARCHAR(45)  NULL,
  mac       CHAR(17)     NOT NULL UNIQUE,
  porta_id  INT          NULL UNIQUE,     -- uma máquina por porta
  CONSTRAINT fk_hosts_porta FOREIGN KEY (porta_id) REFERENCES portas(id)
    ON DELETE SET NULL
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------
-- maquinas_autorizadas: MACs que podem abrir a interface web.
-- Cada uma controla UMA sala (a sala onde ela está).
-- Para trocar a máquina do professor: atualizar o MAC aqui.
-- ---------------------------------------------------------------------
CREATE TABLE maquinas_autorizadas (
  id         INT AUTO_INCREMENT PRIMARY KEY,
  sala_id    INT          NOT NULL,
  mac        CHAR(17)     NOT NULL UNIQUE,
  descricao  VARCHAR(100) NULL,
  ativa      TINYINT(1)   NOT NULL DEFAULT 1,
  CONSTRAINT fk_aut_sala FOREIGN KEY (sala_id) REFERENCES salas(id)
    ON DELETE CASCADE
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------
-- usuarios: "principais" que fazem login.
-- senha_hash gerado com password_hash() do PHP (nunca senha pura!).
-- ---------------------------------------------------------------------
CREATE TABLE usuarios (
  id          INT AUTO_INCREMENT PRIMARY KEY,
  login       VARCHAR(50)  NOT NULL UNIQUE,
  nome        VARCHAR(100) NOT NULL,
  senha_hash  VARCHAR(255) NOT NULL,
  ativo       TINYINT(1)   NOT NULL DEFAULT 1,
  criado_em   DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------
-- agendamentos: cada bloqueio (imediato ou futuro) de UMA porta.
--   lote   = agrupa as portas de um mesmo clique (ex.: "bloquear sala")
--            para poder cancelar tudo junto.
--   status:
--     agendado  -> ainda não começou
--     ativo     -> porta bloqueada agora
--     concluido -> já foi desbloqueada no fim do prazo
--     cancelado -> desbloqueio manual antes do fim
--     erro      -> o SNMP SET falhou
-- As linhas do crontab levam o comentário  # port-warden:<id>
-- para que o PHP consiga removê-las ao cancelar.
-- ---------------------------------------------------------------------
CREATE TABLE agendamentos (
  id            INT AUTO_INCREMENT PRIMARY KEY,
  porta_id      INT          NOT NULL,
  usuario_id    INT          NULL,
  lote          VARCHAR(40)  NULL,
  inicio        DATETIME     NOT NULL,
  fim           DATETIME     NOT NULL,
  status        ENUM('agendado','ativo','concluido','cancelado','erro')
                NOT NULL DEFAULT 'agendado',
  criado_em     DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
  atualizado_em DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP
                ON UPDATE CURRENT_TIMESTAMP,
  CONSTRAINT fk_ag_porta   FOREIGN KEY (porta_id)   REFERENCES portas(id)
    ON DELETE CASCADE,
  CONSTRAINT fk_ag_usuario FOREIGN KEY (usuario_id) REFERENCES usuarios(id)
    ON DELETE SET NULL,
  CONSTRAINT ck_ag_periodo CHECK (fim > inicio),
  INDEX idx_ag_status (status),
  INDEX idx_ag_lote (lote)
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------
-- log_acoes: histórico de tudo que o gerente fez (útil na apresentação
-- e para depurar).
-- ---------------------------------------------------------------------
CREATE TABLE log_acoes (
  id              INT AUTO_INCREMENT PRIMARY KEY,
  data_hora       DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
  usuario_id      INT          NULL,
  porta_id        INT          NULL,
  agendamento_id  INT          NULL,
  acao            ENUM('bloquear','desbloquear','status','login','acesso_negado')
                  NOT NULL,
  origem          ENUM('web','cron') NOT NULL DEFAULT 'web',
  sucesso         TINYINT(1)   NOT NULL,
  mensagem        VARCHAR(255) NULL,
  ip_cliente      VARCHAR(45)  NULL,
  CONSTRAINT fk_log_usuario FOREIGN KEY (usuario_id)     REFERENCES usuarios(id)     ON DELETE SET NULL,
  CONSTRAINT fk_log_porta   FOREIGN KEY (porta_id)       REFERENCES portas(id)       ON DELETE SET NULL,
  CONSTRAINT fk_log_ag      FOREIGN KEY (agendamento_id) REFERENCES agendamentos(id) ON DELETE SET NULL
) ENGINE=InnoDB;

-- ---------------------------------------------------------------------
-- vw_portas: visão "pronta" para a tela do professor.
-- O PHP faz:  SELECT * FROM vw_portas WHERE sala_id = ?
-- ---------------------------------------------------------------------
CREATE VIEW vw_portas AS
SELECT
  p.id            AS porta_id,
  p.numero        AS porta_numero,
  p.if_index,
  p.tipo,
  p.protegida,
  p.com_defeito,
  p.observacao,
  s.id            AS switch_id,
  s.nome          AS switch_nome,
  s.ip            AS switch_ip,
  s.community_leitura,
  s.community_escrita,
  sa.id           AS sala_id,
  sa.nome         AS sala_nome,
  h.id            AS host_id,
  h.nome          AS host_nome,
  h.ip            AS host_ip,
  h.mac           AS host_mac,
  -- portas livres também entram: senão um aluno pluga o notebook numa porta
  -- vazia e escapa do "bloquear sala inteira"
  (p.protegida = 0 AND p.com_defeito = 0 AND p.tipo IN ('host','livre')
   AND p.sala_id IS NOT NULL) AS bloqueavel
FROM portas p
JOIN switches s  ON s.id  = p.switch_id
LEFT JOIN salas sa ON sa.id = p.sala_id
LEFT JOIN hosts h  ON h.porta_id = p.id;
