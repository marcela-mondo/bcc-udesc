-- =====================================================================
-- port-warden | 03_consultas_uteis.sql
-- Consultas para conferir o banco e para o manual do administrador.
-- Não é executado pelo criar_banco.sh: copie e cole no phpMyAdmin.
-- =====================================================================

-- 1) Mapa completo de portas
SELECT porta_numero, if_index, tipo, sala_nome, host_nome, host_mac,
       protegida, com_defeito, bloqueavel
FROM vw_portas
ORDER BY switch_nome, porta_numero;

-- 2) Portas que o professor da F203 pode bloquear
SELECT porta_numero, host_nome, host_ip, host_mac
FROM vw_portas
WHERE sala_nome = 'F203' AND bloqueavel = 1;

-- 3) Qual sala uma máquina autorizada controla (o PHP faz isso no login)
SELECT ma.mac, s.nome AS sala
FROM maquinas_autorizadas ma
JOIN salas s ON s.id = ma.sala_id
WHERE ma.mac = 'aa:bb:cc:00:00:02' AND ma.ativa = 1;

-- 4) Switches que atendem cada sala (uma sala pode ter vários)
SELECT DISTINCT sala_nome, switch_nome, switch_ip
FROM vw_portas
WHERE sala_nome IS NOT NULL
ORDER BY sala_nome;

-- 5) Agendamentos pendentes ou ativos
SELECT a.id, v.porta_numero, v.host_nome, a.inicio, a.fim, a.status
FROM agendamentos a
JOIN vw_portas v ON v.porta_id = a.porta_id
WHERE a.status IN ('agendado', 'ativo')
ORDER BY a.inicio;

-- ---------------------------------------------------------------------
-- Tarefas comuns do administrador
-- ---------------------------------------------------------------------

-- Trocar a máquina autorizada de uma sala (novo MAC)
-- UPDATE maquinas_autorizadas SET mac = 'xx:xx:xx:xx:xx:xx' WHERE sala_id = 1;

-- Uma máquina mudou de porta (ex.: F203-PC01 foi para a porta 9)
-- UPDATE hosts SET porta_id = (SELECT id FROM portas WHERE switch_id = 1 AND numero = 9)
-- WHERE nome = 'F203-PC01';
-- UPDATE portas SET tipo = 'host' WHERE switch_id = 1 AND numero = 9;

-- Trocar o IP de um switch
-- UPDATE switches SET ip = '10.90.90.91' WHERE nome = 'SW-BANCADA';
