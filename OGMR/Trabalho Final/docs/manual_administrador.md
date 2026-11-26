# Manual do Administrador

**Port Warden**: controle de acesso à Internet por sala via SNMP.

> Este guia é para quem instala, configura e mantém o sistema.
> Para o uso no dia a dia, consulte o *Manual do Usuário*.

---

## 1. Visão geral

O Port Warden liga ou desliga portas de rede em switches gerenciáveis por meio do protocolo **SNMP v2c**, enviando comandos `SET` para o objeto `ifAdminStatus` (`1.3.6.1.2.1.2.2.1.7.<ifIndex>`, MIB-II):

* **Valor `2`**: desliga a porta (bloqueia).
* **Valor `1`**: religa a porta (libera).

### Como os componentes se conectam

```
Navegador (máquina autorizada) ──► Apache + PHP ──► MySQL (banco "portwarden")
                                       │
                                       ├─ bloqueio imediato: gerente.jar ─┐
                                       └─ agendado: crontab do www-data   │
                                              └─► php cli/executar.php ───┤
                                                                          ▼ SNMP SET
                                                                switch (ifAdminStatus)
```

| Componente | Caminho / Local | Função |
|---|---|---|
| Site (PHP) | `/var/www/html/port-warden/` | Interface web, regras e integração com o cron |
| Configuração local | `/var/www/html/port-warden/config.local.php` | Senha do banco, modo simulado e ajustes locais |
| Gerente SNMP (Java) | `/opt/port-warden/lib/gerente.jar` | Executa consultas (`GET`) e alterações (`SET`) no switch |
| Dados de execução | `/opt/port-warden/dados/` | Logs, cache e arquivos de trava |
| Agendamentos | `crontab` do usuário `www-data` | Dispara bloqueios e desbloqueios nos horários marcados |
| Banco de dados | MySQL (`portwarden`, usuário `portwarden`) | Armazena cadastros, agendamentos e histórico |

---

## 2. Instalação

### 2.1 Requisitos

* Sistema operacional: Linux Mint 22.3 ou Ubuntu 24.04/26.04 LTS, configurado no fuso **`America/Sao_Paulo`**.
* Rede: servidor na **mesma rede local** (mesmo segmento de camada 2, sem roteador entre eles) das máquinas autorizadas (veja a seção 7.1).
* Hardware de rede: switch gerenciável com SNMP v2c ativado.

Instale os pacotes necessários e ajuste o fuso horário:

```bash
sudo apt install -y apache2 php libapache2-mod-php php-mysql mysql-server phpmyadmin openjdk-21-jdk
sudo timedatectl set-timezone America/Sao_Paulo
```

### 2.2 Banco de dados

Na pasta `banco/` do repositório, execute um dos comandos abaixo:

```bash
bash criar_banco.sh            # cria a estrutura e carrega dados de exemplo
bash criar_banco.sh --vazio    # cria apenas a estrutura das tabelas
```

> **Atenção:** este script **apaga** qualquer banco `portwarden` existente. Antes de rodá-lo, altere a senha padrão `TroqueEstaSenha` em `00_banco_e_usuario.sql`.

### 2.3 Sistema

Na raiz do repositório, execute:

```bash
sudo bash instalar.sh
```

O script copia os arquivos do sistema, ajusta permissões, bloqueia o acesso direto via navegador às pastas `inc/` e `cli/` e verifica PHP, banco de dados, crontab e Java. Ele pode ser executado novamente em atualizações sem sobrescrever o `config.local.php`.

### 2.4 Configuração (`config.local.php`)

Declare no `config.local.php` **apenas** os parâmetros que precisarem ser diferentes do padrão:

| Constante | Padrão | Finalidade |
|---|---|---|
| `DB_DSN` | `mysql:host=localhost;dbname=portwarden;charset=utf8mb4` | String de conexão com o MySQL |
| `DB_USUARIO` / `DB_SENHA` | `portwarden` / `TroqueEstaSenha` | Usuário e senha do banco |
| `DIR_DADOS` | `/opt/port-warden/dados` | Logs, cache e travas. Mantenha fora do diretório web e **nunca** em `/tmp` |
| `JAVA_BIN` | `/usr/bin/java` | Caminho do executável Java |
| `GERENTE_JAR` | `/opt/port-warden/lib/gerente.jar` | Caminho do utilitário SNMP |
| `GERENTE_SIMULAR` | `true` | `true` simula um switch de 24 portas. Defina como **`false` em produção** |
| `PHP_BIN` | `/usr/bin/php` | Executável PHP usado nas tarefas agendadas |
| `CRON_SIMULADO` | `false` | `true` grava comandos de cron em arquivo (útil em testes no Windows) |
| `STATUS_INTERVALO_MS` | `5000` | Intervalo de atualização automática da interface (em milissegundos) |
| `STATUS_CACHE_SEGUNDOS` | `4` | Tempo de reaproveitamento da última leitura SNMP |
| `DURACAO_MAXIMA_DIAS` | `30` | Limite máximo de duração de um bloqueio |
| `MACS_FIXOS` | `127.0.0.1` → MAC do professor (F203) | **Apenas para testes.** Use `[]` em produção |

### 2.5 Switch

Na interface web do switch (por exemplo, D-Link em `http://10.90.90.90`):

1. Ative o **SNMP** (em equipamentos D-Link, costuma vir desativado de fábrica).
2. Configure uma *community* de **leitura** e outra de **escrita**, substituindo os valores padrão `public` e `private`.
3. Cadastre essas mesmas *communities* na tabela `switches` (veja a seção 3.2).

Teste a comunicação a partir do servidor:

```bash
java -jar /opt/port-warden/lib/gerente.jar -c <community_leitura> status 10.90.90.90
```

---

## 3. Banco de dados

### Modelo relacional

```
salas 1───N portas N───1 switches
  │           │
  │           1
  │           │
  N           0..1
maquinas_autorizadas   hosts          usuarios 1───N agendamentos N───1 portas
```

Cada porta pertence a **um switch** e pode estar vinculada a **uma sala**. Dessa forma, uma mesma sala pode receber portas de vários switches, e um switch pode atender mais de uma sala.

### 3.1 `salas`

| Coluna | Descrição |
|---|---|
| `id` | Identificador da sala |
| `nome` | Nome único exibido na tela (ex.: `F203`) |
| `descricao` | Descrição livre |

### 3.2 `switches`

| Coluna | Descrição |
|---|---|
| `nome` | Identificador/apelido único do switch (ex.: `SW-BANCADA`) |
| `modelo`, `patrimonio`, `localizacao` | Campos informativos |
| `ip` | Endereço IP único de gerência do switch |
| `porta_snmp` | Porta UDP do agente SNMP (padrão `161`) |
| `community_leitura` | *Community* usada para consultar o estado das portas |
| `community_escrita` | *Community* usada para bloquear ou desbloquear portas (`SET`) |
| `qtd_portas` | Quantidade de portas (informativo) |

### 3.3 `portas`

Representa cada porta física de cada switch.

| Coluna | Descrição |
|---|---|
| `switch_id` | Switch ao qual a porta pertence |
| `numero` | Número físico impresso no painel do switch (exibido aos usuários) |
| `if_index` | Índice SNMP da interface usado no OID. Confirme com `gerente.jar status`, pois nem sempre coincide com `numero` |
| `sala_id` | Sala atendida pela porta (`NULL` para portas sem sala, como uplinks ou servidores) |
| `tipo` | `host` (estação), `autorizada` (professor), `servidor`, `uplink` ou `livre` |
| `protegida` | `1` impede qualquer bloqueio na porta (`servidor`, `autorizada`, `uplink`) |
| `com_defeito` | `1` oculta a porta das opções de bloqueio |
| `observacao` | Texto exibido ao passar o mouse sobre o cartão da porta |

Uma porta é considerada **bloqueável** quando `protegida = 0`, `com_defeito = 0`, `sala_id` está preenchido e `tipo` é `host` ou `livre`.

### 3.4 `hosts`

Cadastro das máquinas conhecidas para facilitar a busca por nome, IP ou MAC.

| Coluna | Descrição |
|---|---|
| `nome` | Nome da máquina (ex.: `F203-PC01`) |
| `ip` | Endereço IP (informativo) |
| `mac` | Endereço MAC único, em **minúsculas e separado por `:`** (ex.: `28:f1:0e:fc:9e:18`) |
| `porta_id` | Porta onde o equipamento está conectado (`NULL` se desconectado; uma máquina por porta) |

### 3.5 `maquinas_autorizadas`

Define quais computadores podem acessar o sistema. É possível cadastrar **uma ou mais máquinas por sala**.

| Coluna | Descrição |
|---|---|
| `sala_id` | Sala associada à máquina autorizada |
| `mac` | MAC da máquina (único, minúsculo, separado por `:`) |
| `descricao` | Identificação da máquina (ex.: *PC do professor - F203*) |
| `ativa` | `0` desativa o registro sem excluí-lo |

> **Importante:** sempre que a máquina do professor for substituída, atualize o MAC correspondente nesta tabela (veja a seção 4.4).

### 3.6 `usuarios`

| Coluna | Descrição |
|---|---|
| `login` | Identificador de acesso na tela de login (único) |
| `nome` | Nome exibido no topo da interface |
| `senha_hash` | Hash bcrypt gerado pelo PHP (nunca armazene a senha em texto puro) |
| `ativo` | `0` bloqueia o login e encerra sessões ativas |

### 3.7 `agendamentos` (gerenciado pelo sistema)

| Coluna | Descrição |
|---|---|
| `porta_id`, `usuario_id` | Porta alvo e usuário que solicitou o agendamento |
| `lote` | Agrupa várias portas de uma mesma ação (permite cancelar o grupo inteiro) |
| `inicio`, `fim` | Intervalo do bloqueio, ajustado a minutos cheios |
| `status` | Fluxo típico: `agendado` → `ativo` → `concluido` (ou `cancelado` / `erro`) |

Cada agendamento cria entradas no crontab do `www-data` marcadas com `# port-warden:<id>:bloquear` e `# port-warden:<id>:desbloquear`.

### 3.8 `log_acoes` (gerenciado pelo sistema)

Registra logins, bloqueios, desbloqueios e tentativas com **acesso negado**, incluindo data/hora, origem (`web` ou `cron`), indicador de sucesso e mensagem.

### 3.9 `vw_portas` (view)

Consolida dados de portas, switch, sala e host em uma única consulta, incluindo a coluna calculada `bloqueavel`, utilizada para renderizar a tela.

---

## 4. Tarefas do dia a dia

As consultas SQL abaixo podem ser executadas na aba **SQL** do **phpMyAdmin** (`http://<servidor>/phpmyadmin`, com o usuário `portwarden`) e também estão reunidas em `banco/03_consultas_uteis.sql`.

### 4.1 Consultar `ifIndex` e MACs aprendidos no switch

```bash
java -jar /opt/port-warden/lib/gerente.jar -c <community_leitura> status 10.90.90.90
java -jar /opt/port-warden/lib/gerente.jar -c <community_leitura> macs 10.90.90.90
```

O comando `macs` exibe os endereços MAC aprendidos e o respectivo `if_index` da porta.

### 4.2 Cadastrar uma nova sala

```sql
INSERT INTO salas (nome, descricao) VALUES ('F205', 'Laboratório F205');
```

Em seguida, vincule as portas da sala (seção 4.3) e cadastre a máquina autorizada (seção 4.4).

### 4.3 Cadastrar um novo switch e suas portas

```sql
INSERT INTO switches (nome, modelo, ip, community_leitura, community_escrita, qtd_portas)
VALUES ('SW-F205', 'D-Link DGS-3100-24', '10.90.90.91', 'leitura123', 'escrita123', 24);

-- cria as 24 portas replicando a numeração de um switch de referência (ajuste o switch_id de origem)
INSERT INTO portas (switch_id, numero, if_index, tipo)
SELECT (SELECT id FROM switches WHERE nome = 'SW-F205'), numero, numero, 'livre'
FROM portas WHERE switch_id = 1;

-- vincula as portas 1 a 20 à sala F205
UPDATE portas
SET sala_id = (SELECT id FROM salas WHERE nome = 'F205')
WHERE switch_id = (SELECT id FROM switches WHERE nome = 'SW-F205') AND numero BETWEEN 1 AND 20;

-- protege a porta de uplink
UPDATE portas SET tipo = 'uplink', protegida = 1, sala_id = NULL
WHERE switch_id = (SELECT id FROM switches WHERE nome = 'SW-F205') AND numero = 24;
```

Se a saída de `status` indicar que o `if_index` difere do número físico da porta, ajuste `if_index` nas portas afetadas.

> **Sala em mais de um switch:** basta atribuir o mesmo `sala_id` às portas correspondentes nos diferentes switches. A ação *Bloquear sala inteira* atuará em todas elas.

### 4.4 Trocar ou adicionar máquina autorizada (computador do professor)

1. Obtenha o endereço MAC da nova máquina:
   * Na própria máquina: `ip link` (Linux) ou `ipconfig /all` (Windows).
   * Pelo log do sistema: tente abrir o sistema a partir da máquina e consulte o MAC registrado:

     ```sql
     SELECT data_hora, mensagem FROM log_acoes
     WHERE acao = 'acesso_negado' ORDER BY id DESC LIMIT 5;
     ```

2. Atualize o MAC da sala:

   ```sql
   UPDATE maquinas_autorizadas SET mac = 'xx:xx:xx:xx:xx:xx'
   WHERE sala_id = (SELECT id FROM salas WHERE nome = 'F203');
   ```

3. Se o cabo foi ligado em **outra porta**, marque a nova porta como protegida:

   ```sql
   UPDATE portas SET tipo = 'autorizada', protegida = 1
   WHERE switch_id = 1 AND numero = <porta_nova>;
   ```

Para **adicionar outra máquina autorizada** na mesma sala:

```sql
INSERT INTO maquinas_autorizadas (sala_id, mac, descricao)
VALUES ((SELECT id FROM salas WHERE nome = 'F203'), 'xx:xx:xx:xx:xx:xx', 'Notebook do monitor');
```

### 4.5 Atualizar uma máquina (`host`)

Quando a máquina é trocada na **mesma porta**:

```sql
UPDATE hosts SET nome = 'F203-PC01', mac = 'xx:xx:xx:xx:xx:xx', ip = '10.90.90.101'
WHERE porta_id = (SELECT id FROM portas WHERE switch_id = 1 AND numero = 3);
```

Quando a máquina é movida para **outra porta**:

```sql
UPDATE hosts SET porta_id = (SELECT id FROM portas WHERE switch_id = 1 AND numero = 9)
WHERE nome = 'F203-PC01';
UPDATE portas SET tipo = 'host' WHERE switch_id = 1 AND numero = 9;
UPDATE portas SET tipo = 'livre' WHERE switch_id = 1 AND numero = 3;
```

> Mesmo que o cadastro de `hosts` esteja desatualizado, o bloqueio atua na **porta física** selecionada, mas o rótulo exibido pode mostrar o nome antigo.

### 4.6 Substituir um switch

1. Libere todas as portas da sala pelo sistema (*Liberar sala inteira*).
2. Atualize o IP e as *communities*, se necessário:

   ```sql
   UPDATE switches SET ip = '10.90.90.95', community_leitura = '...', community_escrita = '...'
   WHERE nome = 'SW-BANCADA';
   ```

3. Execute `gerente.jar status` e corrija `if_index` caso o modelo seja diferente.
4. Se os cabos mudaram de porta, atualize `hosts.porta_id` conforme a seção 4.5.

### 4.7 Marcar porta protegida ou com defeito

```sql
UPDATE portas SET protegida = 1, tipo = 'uplink' WHERE switch_id = 1 AND numero = 23;
UPDATE portas SET com_defeito = 1 WHERE switch_id = 1 AND numero IN (21, 22, 24);
```

### 4.8 Gerenciar usuários

Gerar o hash da senha no servidor antes de inserir ou atualizar:

```bash
php -r 'echo password_hash("SenhaNova", PASSWORD_DEFAULT), PHP_EOL;'
```

```sql
-- cadastrar novo usuário
INSERT INTO usuarios (login, nome, senha_hash) VALUES ('maria', 'Profa. Maria', '<hash>');

-- alterar senha
UPDATE usuarios SET senha_hash = '<hash>' WHERE login = 'professor';

-- desativar usuário
UPDATE usuarios SET ativo = 0 WHERE login = 'maria';
```

---

## 5. Operação e monitoramento

| Finalidade | Comando |
|---|---|
| Ver todo o crontab do `www-data` | `sudo -u www-data crontab -l` |
| Listar agendamentos do sistema | `sudo -u www-data php /var/www/html/port-warden/cli/executar.php listar` |
| Executar um agendamento manualmente | `sudo -u www-data php /var/www/html/port-warden/cli/executar.php <id> desbloquear` |
| Reconciliar agendamentos pendentes/atrasados | `sudo -u www-data php /var/www/html/port-warden/cli/executar.php reconciliar` |
| Acompanhar log do gerente SNMP | `tail -f /opt/port-warden/dados/gerente.log` |
| Acompanhar log do cron | `tail -f /opt/port-warden/dados/cron.log` |
| Consultar últimas ações no banco | `SELECT * FROM log_acoes ORDER BY id DESC LIMIT 50;` |

### Regras de agendamento e contingência

* **Arredondamento e limpeza:** como o `cron` opera em minutos cheios, o horário de início é arredondado para baixo e o de término para cima. Como o `cron` não possui campo de ano, cada linha é removida automaticamente após a execução.
* **Servidor desligado no horário agendado:** na próxima abertura da tela, o sistema reconcilia eventos pendentes (com tolerância de 2 minutos). Para executar a reconciliação mesmo sem a tela aberta, adicione ao crontab do `www-data`:

```cron
*/5 * * * * /usr/bin/php /var/www/html/port-warden/cli/executar.php reconciliar >> /opt/port-warden/dados/cron.log 2>&1
```

---

## 6. Solução de problemas

| Sintoma | Causa provável | Ação recomendada |
|---|---|---|
| "Acesso não autorizado" na máquina do professor | MAC ausente ou diferente do cadastrado | Verifique o MAC em `log_acoes` (seção 4.4) e atualize `maquinas_autorizadas` |
| "Acesso não autorizado" mesmo com MAC correto | Máquina e servidor em sub-redes diferentes (há roteador no meio) | Coloque ambos na mesma rede local (seção 7.1) |
| Cartões amarelos ("Sem leitura") | Switch inacessível, SNMP desligado ou *community* incorreta | Rode `gerente.jar status <ip>`; confira IP, cabo e SNMP ativo |
| Bloqueio falha com "No access" | *Community* sem permissão de escrita | Revise `community_escrita` no switch e no banco |
| Bloqueio falha com "Unable to create object" | `if_index` inexistente no switch | Confira os índices com `gerente.jar status` |
| Bloqueou a porta/máquina errada | `if_index` diferente do número físico da porta | Ajuste `portas.if_index` |
| Agendamento não executou | Cron parado, fuso errado ou permissão | Verifique `systemctl status cron`, `timedatectl`, `sudo -u www-data crontab -l` e `cron.log` |
| Interface sem estilo visual | `bootstrap.min.css` ausente (bancada sem internet) | Execute `bash /var/www/html/port-warden/assets/vendor/baixar_bootstrap.sh` com internet |
| Erro de banco na tela | Senha errada no arquivo de configuração | Confira `DB_SENHA` em `config.local.php` |
| "MODO SIMULADO" no topo da tela | Simulação ativada | Altere `GERENTE_SIMULAR` para `false` em `config.local.php` |
| Página em branco ou erro HTTP 500 | Erro de execução no PHP | Consulte `sudo tail /var/log/apache2/error.log` |

---

## 7. Segurança e limitações

### 7.1 Identificação por MAC

O PHP recebe apenas o IP do cliente; o endereço MAC vem da **tabela ARP do servidor**. Isso exige que o servidor e a máquina autorizada estejam na **mesma rede local** (mesmo segmento, sem roteamento entre eles). Como o MAC pode ser alterado localmente por quem tem privilégios na máquina, o sistema também exige **login e senha**.

### 7.2 Checklist antes de colocar em uso

* [ ] `GERENTE_SIMULAR = false`
* [ ] `MACS_FIXOS = []`
* [ ] Senha do banco alterada em `00_banco_e_usuario.sql` e `config.local.php`
* [ ] Senha do usuário `professor` alterada (seção 4.8)
* [ ] *Communities* SNMP alteradas (diferentes de `public` e `private`)
* [ ] Portas de servidor, professores e uplinks com `protegida = 1`
* [ ] `if_index` conferidos com `gerente.jar status`
* [ ] MACs de exemplo (`aa:bb:cc:...`) substituídos pelos endereços reais