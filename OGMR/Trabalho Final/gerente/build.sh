#!/usr/bin/env bash
# =====================================================================
# port-warden | gerente/build.sh
# Compila o Gerente.java e gera gerente.jar (já com o SNMP4J dentro,
# então basta:  java -jar gerente.jar ...).
#
# Requisitos: JDK 17 ou mais novo  (java -version / javac -version)
# Uso (na pasta gerente/):  bash build.sh
# =====================================================================
set -euo pipefail
cd "$(dirname "$0")"

LIB="lib/snmp4j-3.8.2.jar"
if [[ ! -f "$LIB" ]]; then
  echo "ERRO: coloque o snmp4j-3.8.2.jar em gerente/lib/" >&2
  exit 1
fi
command -v javac >/dev/null || { echo "ERRO: javac não encontrado. Instale: sudo apt install openjdk-21-jdk" >&2; exit 1; }

echo ">> Compilando..."
rm -rf build
mkdir -p build/classes
javac --release 17 -encoding UTF-8 -Xlint:unchecked -cp "$LIB" -d build/classes src/Gerente.java

echo ">> Empacotando SNMP4J dentro do jar..."
LIB_ABS="$(pwd)/$LIB"
(
  cd build/classes
  jar xf "$LIB_ABS"
  rm -f META-INF/MANIFEST.MF          # o manifest será o nosso
)
mkdir -p build/classes/META-INF && cp lib/SNMP4J-LICENSE.txt lib/SNMP4J-NOTICE.txt build/classes/META-INF/

printf "Main-Class: Gerente\n" > build/manifest.txt
jar cfm gerente.jar build/manifest.txt -C build/classes .
rm -rf build

echo ">> OK: $(pwd)/gerente.jar"
echo
java -jar gerente.jar ajuda
echo
echo ">> Teste sem switch:  java -jar gerente.jar --simular status 10.90.90.90"
