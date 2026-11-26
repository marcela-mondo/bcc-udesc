-- =====================================================================
-- port-warden | 00_banco_e_usuario.sql
-- Cria o banco "portwarden" e o usuário que o PHP e o gerente usam.
-- Executar como root:  sudo mysql < 00_banco_e_usuario.sql
--
-- ATENÇÃO: este script APAGA o banco "portwarden" se ele já existir.
-- Troque a senha abaixo e use a mesma em web/config.php.
-- =====================================================================

DROP DATABASE IF EXISTS portwarden;

CREATE DATABASE portwarden
  CHARACTER SET utf8mb4
  COLLATE utf8mb4_unicode_ci;

CREATE USER IF NOT EXISTS 'portwarden'@'localhost' IDENTIFIED BY 'udesc';
ALTER USER 'portwarden'@'localhost' IDENTIFIED BY 'udesc';

GRANT ALL PRIVILEGES ON portwarden.* TO 'portwarden'@'localhost';
FLUSH PRIVILEGES;
