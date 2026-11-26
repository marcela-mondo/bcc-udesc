#!/usr/bin/env bash
# Baixa o CSS do Bootstrap para esta pasta.
# Rode UMA vez com internet. Na bancada (switch sem internet) o sistema
# usa o arquivo local; sem ele, a tela fica sem estilo.
set -euo pipefail
cd "$(dirname "$0")"
VERSAO="5.3.3"
curl -fsSL -o bootstrap.min.css "https://cdn.jsdelivr.net/npm/bootstrap@${VERSAO}/dist/css/bootstrap.min.css"
echo "OK: $(pwd)/bootstrap.min.css ($(wc -c < bootstrap.min.css) bytes)"
