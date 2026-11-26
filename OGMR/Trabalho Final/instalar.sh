#!/usr/bin/env bash
# =====================================================================
# port-warden | instalar.sh
# Instala/atualiza o sistema na VM Linux (Mint/Ubuntu) a partir da pasta
# do repositório. Pode ser rodado de novo a cada atualização do código:
# o config.local.php existente é preservado.
#
# Pré-requisitos (uma vez):
#   sudo apt install -y apache2 php libapache2-mod-php php-mysql \
#        mysql-server phpmyadmin openjdk-21-jdk
#   bash banco/criar_banco.sh
#
# Uso (na raiz do repositório):  sudo bash instalar.sh
# =====================================================================
set -euo pipefail

[[ $EUID -eq 0 ]] || { echo "Rode com sudo: sudo bash instalar.sh" >&2; exit 1; }
cd "$(dirname "$0")"

WEB=/var/www/html/port-warden
OPT=/opt/port-warden
USUARIO_REAL="${SUDO_USER:-}"

echo ">> [1/7] Conferindo dependências..."
for cmd in php java apache2ctl; do
  command -v "$cmd" >/dev/null || { echo "ERRO: '$cmd' não encontrado. Veja os pré-requisitos no topo deste script." >&2; exit 1; }
done
command -v crontab >/dev/null || { echo "ERRO: 'crontab' não encontrado (sudo apt install cron)." >&2; exit 1; }

echo ">> [2/7] Gerente SNMP..."
if [[ ! -f gerente/gerente.jar ]]; then
  (cd gerente && bash build.sh)
fi
mkdir -p "$OPT/lib" "$OPT/dados"
cp gerente/gerente.jar "$OPT/lib/gerente.jar"

echo ">> [3/7] Site em $WEB ..."
mkdir -p "$WEB"
LOCAL_BKP=""
if [[ -f "$WEB/config.local.php" ]]; then
  LOCAL_BKP="$(mktemp)"; cp "$WEB/config.local.php" "$LOCAL_BKP"
fi
cp -r web/. "$WEB/"
rm -f "$WEB/config.local.php"
if [[ -n "$LOCAL_BKP" ]]; then
  mv "$LOCAL_BKP" "$WEB/config.local.php"
  echo "   config.local.php existente preservado."
else
  cp "$WEB/config.local.exemplo.php" "$WEB/config.local.php"
  echo "   config.local.php criado a partir do exemplo: EDITE a senha do banco."
fi

echo ">> [4/7] Bootstrap local (o switch da bancada não tem internet)..."
if [[ ! -s "$WEB/assets/vendor/bootstrap.min.css" ]]; then
  if bash "$WEB/assets/vendor/baixar_bootstrap.sh"; then
    cp "$WEB/assets/vendor/bootstrap.min.css" web/assets/vendor/ 2>/dev/null || true
  else
    echo "   AVISO: sem internet agora. Rode depois: bash $WEB/assets/vendor/baixar_bootstrap.sh"
  fi
fi

echo ">> [5/7] Permissões..."
chown -R root:www-data "$WEB"
find "$WEB" -type d -exec chmod 755 {} +
find "$WEB" -type f -exec chmod 644 {} +
chmod 640 "$WEB/config.local.php"               # senha do banco: só root e www-data leem
chown -R www-data:www-data "$OPT/dados"
chmod 2775 "$OPT/dados"
if [[ -n "$USUARIO_REAL" ]]; then
  usermod -aG www-data "$USUARIO_REAL"
  echo "   $USUARIO_REAL adicionado ao grupo www-data (vale no próximo login)."
fi

echo ">> [6/7] Apache: bloqueando acesso direto a inc/, cli/ e config..."
cat > /etc/apache2/conf-available/port-warden.conf <<EOF
# port-warden: pastas que nunca devem ser servidas pelo navegador
<Directory $WEB/inc>
    Require all denied
</Directory>
<Directory $WEB/cli>
    Require all denied
</Directory>
<Directory $WEB>
    <FilesMatch "^config(\.local)?(\.exemplo)?\.php$">
        Require all denied
    </FilesMatch>
</Directory>
EOF
a2enconf -q port-warden
systemctl reload apache2

echo ">> [7/7] Teste rápido como www-data..."
sudo -u www-data php "$WEB/cli/executar.php" listar >/dev/null && echo "   PHP + banco + crontab: OK" \
  || echo "   AVISO: falhou. Confira DB_SENHA em $WEB/config.local.php"
sudo -u www-data java -jar "$OPT/lib/gerente.jar" --simular status 10.90.90.90 >/dev/null 2>&1 \
  && echo "   Java + gerente: OK" || echo "   AVISO: o www-data não conseguiu rodar o gerente.jar"

IP=$(hostname -I 2>/dev/null | awk '{print $1}')
cat <<EOF

Pronto!
  Site:          http://${IP:-IP-DA-VM}/port-warden/
  Config:        $WEB/config.local.php   (senha do banco, simulação)
  Logs:          $OPT/dados/gerente.log  e  $OPT/dados/cron.log
  Agendamentos:  sudo -u www-data crontab -l

Lembretes:
  - O MAC da máquina do professor precisa estar em maquinas_autorizadas.
  - Para usar o switch de verdade: GERENTE_SIMULAR = false no config.local.php.
EOF
