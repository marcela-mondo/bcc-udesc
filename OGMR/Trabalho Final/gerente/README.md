# gerente/ — Gerente SNMP (Java + SNMP4J)

Programa de linha de comando que liga e desliga portas do switch enviando
**SNMP SET em `ifAdminStatus`** (MIB-II, `1.3.6.1.2.1.2.2.1.7.<ifIndex>`).
É chamado pelo PHP (bloqueio imediato) e pelo crontab (agendado).

```
gerente/
├── src/Gerente.java          código-fonte (um arquivo só)
├── lib/snmp4j-3.8.2.jar      API SNMP4J (+ licença e NOTICE)
├── build.sh                  compila e gera gerente.jar
└── gerente.jar               pronto para usar (SNMP4J já embutido)
```

## Compilar

Requer JDK 17 ou mais novo (`sudo apt install openjdk-21-jdk`).

```bash
bash build.sh
```

## Comandos

| Comando | O que faz | Community padrão |
|---|---|---|
| `bloquear <ip> <ifIndex>` | SET ifAdminStatus = 2 (down) e confere com GET | `private` |
| `desbloquear <ip> <ifIndex>` | SET ifAdminStatus = 1 (up) e confere com GET | `private` |
| `consultar <ip> <ifIndex>` | estado de uma porta | `public` |
| `status <ip>` | todas as interfaces: descrição, tipo, admin, oper | `public` |
| `macs <ip>` | MACs aprendidos → porta (BRIDGE-MIB, com fallback Q-BRIDGE) | `public` |
| `ajuda` | mostra o uso | — |

Opções: `-c <community>`, `-p <porta UDP>` (161), `-t <timeout ms>` (1500),
`-r <tentativas>` (2), `--simular`.

### Exemplos

```bash
java -jar gerente.jar status 10.90.90.90
java -jar gerente.jar bloquear 10.90.90.90 3
java -jar gerente.jar -c minhaCommunity desbloquear 10.90.90.90 3
java -jar gerente.jar macs 10.90.90.90
```

Saída (sempre **um JSON** no stdout):

```json
{"ok":true,"comando":"bloquear","ip":"10.90.90.90","if_index":3,"admin":"down","oper":"down","descricao":"...","simulado":false}
```

Em caso de erro: `{"ok":false, ..., "erro":"sem resposta (timeout)..."}`.

Códigos de saída: **0** sucesso · **1** uso incorreto · **2** erro SNMP.

O log (uma linha com data e hora) vai para o **stderr**.

- **`admin`** é o estado configurado (o que o SET muda).
- **`oper`** é o link físico: fica `down` se o cabo estiver solto, mesmo com `admin = up`.

## Modo simulado (desenvolver sem o switch)

```bash
java -jar gerente.jar --simular bloquear 10.90.90.90 3
java -jar gerente.jar --simular status 10.90.90.90      # 24 portas falsas
# ou:  export PORTWARDEN_SIMULAR=1
```

O estado fica em `/tmp/port-warden-sim-<usuario>.properties`, então o `status`
"lembra" o que foi bloqueado. Cada usuário Linux tem o seu arquivo: o que o
PHP (`www-data`) bloqueia não aparece se você rodar o comando como você mesmo.
Para compartilhar, defina `PORTWARDEN_SIM_ARQ=/caminho/arquivo`.

## Chamando pelo PHP

```php
$cmd = sprintf('%s -jar %s -c %s bloquear %s %d 2>> %s',
    '/usr/bin/java',
    escapeshellarg('/opt/port-warden/lib/gerente.jar'),
    escapeshellarg($community_escrita),
    escapeshellarg($switch_ip),
    (int) $if_index,
    escapeshellarg('/opt/port-warden/dados/gerente.log'));

exec($cmd, $saida, $codigo);
$resultado = json_decode(implode("\n", $saida), true);

if ($codigo !== 0 || empty($resultado['ok'])) {
    // mostrar $resultado['erro'] na tela
}
```

Sempre use `escapeshellarg()` e `(int)`: nunca concatene texto vindo do
navegador direto no comando.

## Chamando pelo crontab

```cron
# min hora dia mês dia_semana  comando                                                     # marcador
30 14 24 11 *  /usr/bin/java -jar /opt/port-warden/lib/gerente.jar bloquear 10.90.90.90 3 >> /opt/port-warden/dados/gerente.log 2>&1  # port-warden:15
```

Cuidados com o cron:
1. Use **caminhos completos** (`/usr/bin/java`): o cron tem um PATH mínimo.
2. O caractere **`%` é especial** no crontab (vira quebra de linha). Não use `%` no comando.
3. O cron **não tem ano**: a linha acima roda de novo em 24/11 do ano que vem.
   A linha precisa ser removida depois de executar (o marcador `# port-warden:<id>` serve para isso).
4. O gerente **não mexe no banco**. Para atualizar o status do agendamento e
   remover a linha do cron, o cron deve chamar um script PHP de linha de comando
   (ex.: `php web/cli/executar.php <id_agendamento>`) que chama o gerente.

## O que conferir no switch real (DGS-3100-24)

```bash
java -jar gerente.jar status 10.90.90.90   # anotar ifIndex de cada porta
java -jar gerente.jar macs 10.90.90.90     # conferir em que porta está cada máquina
```

Se `status` der timeout: confira o IP, o cabo, se o SNMP está habilitado na
interface web do switch e a community de leitura. Se o `bloquear` responder
`No access`, a community usada não tem permissão de escrita.
