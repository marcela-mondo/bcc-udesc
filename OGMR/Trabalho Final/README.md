# port-warden

Controle de acesso à Internet por sala, desligando portas físicas do switch
via **SNMP SET** (`ifAdminStatus`, MIB-II), com agendamento pelo **crontab** e
interface web restrita à máquina autorizada do professor.

Trabalho final de OGMR (Gerência de Redes), UDESC.

```
Navegador (professor) ──► PHP (Apache) ──► MySQL
                              │
                              ├─ agora:     java -jar gerente.jar bloquear <switch> <ifIndex>
                              └─ agendado:  crontab ──► php cli/executar.php ──► gerente.jar
                                                                                 │ SNMP SET
                                                                                 ▼
                                                                          switch D-Link
```

| Pasta | Conteúdo |
|---|---|
| [`banco/`](banco/) | Scripts MySQL: estrutura, dados de exemplo, consultas do administrador |
| [`gerente/`](gerente/README.md) | Gerente SNMP em Java (SNMP4J) |
| [`web/`](web/README.md) | Interface web, APIs, regras de agendamento e integração com o cron |
| `docs/` | Manuais do administrador e do usuário, dados do switch |

## Instalação rápida (Linux Mint / Ubuntu)

```bash
sudo apt install -y apache2 php libapache2-mod-php php-mysql mysql-server phpmyadmin openjdk-21-jdk
bash banco/criar_banco.sh
sudo bash instalar.sh
```

Acesse `http://IP-DA-VM/port-warden/` a partir da máquina autorizada.
Login de exemplo: `professor` / `professor123`.
