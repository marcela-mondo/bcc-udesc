-- =====================================================================
-- port-warden | 02_dados_exemplo.sql
-- Dados iniciais da bancada da disciplina (switch D-Link DGS-3100-24).
-- Executar:  mysql -u portwarden -p portwarden < 02_dados_exemplo.sql
--
-- O QUE PRECISA SER CONFIRMADO NO LABORATÓRIO ANTES DE CONFIAR:
--   [ ] IP do switch (padrão de fábrica: 10.90.90.90)
--   [ ] communities de leitura/escrita
--   [ ] if_index de cada porta (walk em 1.3.6.1.2.1.2.2.1.2)
--       Aqui foi assumido if_index = número da porta.
--   [ ] MACs reais das máquinas (os abaixo são FICTÍCIOS: aa:bb:cc:...)
--
-- Cenário de exemplo: o mesmo switch atende DUAS salas (F203 e F204),
-- como o enunciado sugere.
-- =====================================================================

SET NAMES utf8mb4;

-- ---------------------------------------------------------------------
-- Salas
-- ---------------------------------------------------------------------
INSERT INTO salas (id, nome, descricao) VALUES
  (1, 'F203', 'Laboratório de ensino - bancada do trabalho'),
  (2, 'F204', 'Laboratório vizinho (exemplo de switch compartilhado)');

-- ---------------------------------------------------------------------
-- Switch
-- ---------------------------------------------------------------------
INSERT INTO switches
  (id, nome, modelo, patrimonio, ip, porta_snmp,
   community_leitura, community_escrita, qtd_portas, localizacao)
VALUES
  (1, 'SW-BANCADA', 'D-Link DGS-3100-24', 'UDESC 104818', '10.90.90.90', 161,
   'public', 'private', 24, 'Bancada OGMR');

-- ---------------------------------------------------------------------
-- Portas (1 a 24)
--   1        servidor (VM com Apache/PHP/MySQL/gerente)  -> protegida
--   2        máquina autorizada do professor (F203)      -> protegida
--   3 a 12   máquinas da F203
--   13 a 20  máquinas da F204
--   7, 8     funcionam, mas só a 100 Mbps (anotação na fita)
--   21,22,24 com defeito (anotação na fita)
--   23       uplink para a rede / outra sala              -> protegida
-- ---------------------------------------------------------------------
INSERT INTO portas (switch_id, numero, if_index, sala_id, tipo, protegida, com_defeito, observacao) VALUES
  (1,  1,  1, NULL, 'servidor',   1, 0, 'Servidor web + gerente SNMP. NUNCA bloquear.'),
  (1,  2,  2, 1,    'autorizada', 1, 0, 'Máquina do professor (F203).'),
  (1,  3,  3, 1,    'host',       0, 0, NULL),
  (1,  4,  4, 1,    'host',       0, 0, NULL),
  (1,  5,  5, 1,    'host',       0, 0, NULL),
  (1,  6,  6, 1,    'host',       0, 0, NULL),
  (1,  7,  7, 1,    'host',       0, 0, 'Fast Ethernet (100 Mbps).'),
  (1,  8,  8, 1,    'host',       0, 0, 'Fast Ethernet (100 Mbps).'),
  (1,  9,  9, 1,    'livre',      0, 0, NULL),
  (1, 10, 10, 1,    'livre',      0, 0, NULL),
  (1, 11, 11, 1,    'livre',      0, 0, NULL),
  (1, 12, 12, 1,    'livre',      0, 0, NULL),
  (1, 13, 13, 2,    'autorizada', 1, 0, 'Máquina do professor (F204).'),
  (1, 14, 14, 2,    'host',       0, 0, NULL),
  (1, 15, 15, 2,    'host',       0, 0, NULL),
  (1, 16, 16, 2,    'livre',      0, 0, NULL),
  (1, 17, 17, 2,    'livre',      0, 0, NULL),
  (1, 18, 18, 2,    'livre',      0, 0, NULL),
  (1, 19, 19, 2,    'livre',      0, 0, NULL),
  (1, 20, 20, 2,    'livre',      0, 0, NULL),
  (1, 21, 21, NULL, 'livre',      0, 1, 'Com defeito (anotação no switch). Porta combo.'),
  (1, 22, 22, NULL, 'livre',      0, 1, 'Com defeito (anotação no switch). Porta combo.'),
  (1, 23, 23, NULL, 'uplink',     1, 0, 'Uplink. NUNCA bloquear.'),
  (1, 24, 24, NULL, 'livre',      0, 1, 'Com defeito (anotação no switch). Porta combo.');

-- ---------------------------------------------------------------------
-- Hosts (MACs FICTÍCIOS - substituir pelos reais)
-- Os porta_id são buscados pelo número da porta para não depender de id.
-- ---------------------------------------------------------------------
INSERT INTO hosts (nome, ip, mac, porta_id)
SELECT 'SERVIDOR',   '10.90.90.92',  'aa:bb:cc:00:00:01', id FROM portas WHERE switch_id = 1 AND numero = 1  UNION ALL
SELECT 'PROF-F203',  '10.90.90.93',  'aa:bb:cc:00:00:02', id FROM portas WHERE switch_id = 1 AND numero = 2  UNION ALL
SELECT 'F203-PC01',  '10.90.90.101', 'aa:bb:cc:00:00:03', id FROM portas WHERE switch_id = 1 AND numero = 3  UNION ALL
SELECT 'F203-PC02',  '10.90.90.102', 'aa:bb:cc:00:00:04', id FROM portas WHERE switch_id = 1 AND numero = 4  UNION ALL
SELECT 'F203-PC03',  '10.90.90.103', 'aa:bb:cc:00:00:05', id FROM portas WHERE switch_id = 1 AND numero = 5  UNION ALL
SELECT 'F203-PC04',  '10.90.90.104', 'aa:bb:cc:00:00:06', id FROM portas WHERE switch_id = 1 AND numero = 6  UNION ALL
SELECT 'F203-PC05',  '10.90.90.105', 'aa:bb:cc:00:00:07', id FROM portas WHERE switch_id = 1 AND numero = 7  UNION ALL
SELECT 'F203-PC06',  '10.90.90.106', 'aa:bb:cc:00:00:08', id FROM portas WHERE switch_id = 1 AND numero = 8  UNION ALL
SELECT 'PROF-F204',  '10.90.90.113', 'aa:bb:cc:00:00:13', id FROM portas WHERE switch_id = 1 AND numero = 13 UNION ALL
SELECT 'F204-PC01',  '10.90.90.114', 'aa:bb:cc:00:00:14', id FROM portas WHERE switch_id = 1 AND numero = 14 UNION ALL
SELECT 'F204-PC02',  '10.90.90.115', 'aa:bb:cc:00:00:15', id FROM portas WHERE switch_id = 1 AND numero = 15;

-- Um computador cadastrado mas desconectado (porta_id NULL)
INSERT INTO hosts (nome, ip, mac, porta_id) VALUES
  ('NOTEBOOK-RESERVA', NULL, 'aa:bb:cc:00:00:99', NULL);

-- ---------------------------------------------------------------------
-- Máquinas autorizadas (mesmos MACs fictícios dos PCs dos professores)
-- ---------------------------------------------------------------------
INSERT INTO maquinas_autorizadas (sala_id, mac, descricao, ativa) VALUES
  (1, 'aa:bb:cc:00:00:02', 'PC do professor - F203', 1),
  (2, 'aa:bb:cc:00:00:13', 'PC do professor - F204', 1);

-- ---------------------------------------------------------------------
-- Usuários
--   login: professor   senha: professor123
-- Hash gerado com:  php -r 'echo password_hash("professor123", PASSWORD_DEFAULT);'
-- TROQUE a senha antes da apresentação.
-- ---------------------------------------------------------------------
INSERT INTO usuarios (login, nome, senha_hash, ativo) VALUES
  ('professor', 'Professor da Disciplina',
   '$2y$10$QONNrOc4rp6AoisTqBXu1ObuWT1b7U3k0vI.oST.OCGVqBd9h.mJy', 1);

-- ---------------------------------------------------------------------
-- Agendamentos de exemplo (para a tela não começar vazia)
--   - um já concluído (histórico)
--   - um futuro, daqui a 1 dia, por 1 hora, na porta 5
-- ---------------------------------------------------------------------
INSERT INTO agendamentos (porta_id, usuario_id, lote, inicio, fim, status)
SELECT p.id, u.id, 'exemplo-1',
       NOW() - INTERVAL 2 DAY, NOW() - INTERVAL 2 DAY + INTERVAL 50 MINUTE, 'concluido'
FROM portas p, usuarios u
WHERE p.switch_id = 1 AND p.numero = 3 AND u.login = 'professor';

INSERT INTO agendamentos (porta_id, usuario_id, lote, inicio, fim, status)
SELECT p.id, u.id, 'exemplo-2',
       NOW() + INTERVAL 1 DAY, NOW() + INTERVAL 1 DAY + INTERVAL 1 HOUR, 'agendado'
FROM portas p, usuarios u
WHERE p.switch_id = 1 AND p.numero = 5 AND u.login = 'professor';
