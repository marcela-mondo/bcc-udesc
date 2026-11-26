#!/usr/bin/env bash
# =====================================================================
# port-warden | criar_banco.sh
# Cria (ou RECRIA do zero) o banco "portwarden" com dados de exemplo.
#
# Uso (na pasta banco/):
#   bash criar_banco.sh            -> estrutura + dados de exemplo
#   bash criar_banco.sh --vazio    -> só a estrutura, sem dados
#
# ATENÇÃO: apaga o banco "portwarden" existente.
# =====================================================================
set -euo pipefail

cd "$(dirname "$0")"

echo ">> ATENÇÃO: o banco 'portwarden' será APAGADO e recriado."
read -r -p ">> Continuar? (s/N) " resp
[[ "${resp,,}" == "s" ]] || { echo "Cancelado."; exit 1; }

echo ">> [1/3] Criando banco e usuário (pede a senha do sudo)..."
sudo mysql < 00_banco_e_usuario.sql

echo ">> [2/3] Criando tabelas..."
sudo mysql portwarden < 01_schema.sql

if [[ "${1:-}" == "--vazio" ]]; then
  echo ">> [3/3] Pulando dados de exemplo (--vazio)."
else
  echo ">> [3/3] Inserindo dados de exemplo..."
  sudo mysql portwarden < 02_dados_exemplo.sql
fi

echo
echo ">> Pronto! Resumo:"
sudo mysql portwarden -e "
  SELECT 'salas' AS tabela, COUNT(*) AS linhas FROM salas
  UNION ALL SELECT 'switches', COUNT(*) FROM switches
  UNION ALL SELECT 'portas', COUNT(*) FROM portas
  UNION ALL SELECT 'hosts', COUNT(*) FROM hosts
  UNION ALL SELECT 'maquinas_autorizadas', COUNT(*) FROM maquinas_autorizadas
  UNION ALL SELECT 'usuarios', COUNT(*) FROM usuarios
  UNION ALL SELECT 'agendamentos', COUNT(*) FROM agendamentos;"

echo
echo ">> phpMyAdmin: http://localhost/phpmyadmin  (usuário: portwarden)"
echo ">> Login do sistema de exemplo: professor / professor123"
