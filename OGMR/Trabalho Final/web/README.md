# web/ — Interface web e regras do sistema (PHP + Bootstrap + JS)

```
web/
├── config.php               configurações padrão (não coloque senhas aqui)
├── config.local.exemplo.php modelo do config.local.php (senha, simulação)
├── login.php / logout.php
├── index.php                tela da sala (o conteúdo é desenhado pelo app.js)
├── api/
│   ├── status.php           GET  estado das portas + agendamentos (a tela chama a cada 5 s)
│   ├── bloquear.php         POST agora ou agendado; portas escolhidas ou "sala"
│   ├── desbloquear.php      POST libera agora e cancela pendências
│   └── cancelar.php         POST cancela um agendamento ou um grupo (lote)
├── cli/
│   └── executar.php         chamado pelo CRONTAB na hora marcada
├── inc/
│   ├── iniciar.php          carrega tudo
│   ├── autorizacao.php      IP -> MAC (tabela ARP) -> máquina autorizada -> sala; login
│   ├── agendamentos.php     regras: bloquear, desbloquear, cancelar, executar, reconciliar
│   ├── cron.php             cria/remove linhas "# port-warden:<id>:<acao>" no crontab
│   ├── gerente.php          chama o gerente.jar e lê o JSON
│   ├── db.php / util.php / layout.php / pagina_negado.php
└── assets/
    ├── app.js / app.css
    └── vendor/bootstrap.min.css   (baixar_bootstrap.sh)
```

## Como uma requisição é tratada

1. **Máquina autorizada?** O PHP pega o IP do cliente, procura o MAC na tabela
   ARP do servidor (`/proc/net/arp`) e confere em `maquinas_autorizadas`.
   Se não for, aparece "Acesso não autorizado" e nem a tela de login é mostrada.
2. **Login.** Senha conferida com `password_verify`. A sessão fica presa ao MAC
   e à sala: trocar de máquina exige login de novo.
3. **Sala.** O professor só enxerga e só mexe nas portas da sala da máquina
   de onde está acessando. Portas protegidas (servidor, professor, uplink) e
   com defeito nunca são bloqueadas.

## Como um bloqueio funciona

| Situação | O que acontece |
|---|---|
| **Bloquear agora** | SET imediato pelo gerente + linha no cron para desbloquear no fim |
| **Agendar** | Duas linhas no cron: bloquear no início e desbloquear no fim |
| **Cron dispara** | `cli/executar.php <id> <acao>` faz o SET, atualiza o banco e **apaga a própria linha** (o cron não tem ano) |
| **Liberar manualmente** | SET de desbloqueio + cancela todos os agendamentos daquela porta e remove as linhas do cron |
| **Dois bloqueios na mesma porta** | Quando o primeiro termina, a porta só é liberada se nenhum outro bloqueio ainda estiver valendo |
| **Servidor desligado na hora do cron** | Ao abrir a tela, `reconciliar()` executa o que ficou atrasado (mais de 2 min) |

Horários: o cron trabalha em minutos cheios, então o início é arredondado para baixo e
o fim para cima (um bloqueio de 50 min feito às 14:03:40 vai até 14:54).

## Instalar na VM

```bash
sudo bash instalar.sh      # na raiz do repositório
```

Depois, edite `/var/www/html/port-warden/config.local.php` e acesse
`http://IP-DA-VM/port-warden/` a partir da máquina do professor.

## Testar sem switch e sem a máquina do professor

- `GERENTE_SIMULAR = true`: o gerente finge ser um switch de 24 portas.
- `MACS_FIXOS`: acessos de `127.0.0.1` (o navegador da própria VM) contam como
  o MAC `aa:bb:cc:00:00:02`, que nos dados de exemplo é o professor da F203.
- Login de exemplo: `professor` / `professor123`.

Comandos úteis:

```bash
sudo -u www-data crontab -l                                    # agendamentos no cron
sudo -u www-data php /var/www/html/port-warden/cli/executar.php listar
sudo -u www-data php /var/www/html/port-warden/cli/executar.php 15 desbloquear   # simula o cron
tail -f /opt/port-warden/dados/gerente.log /opt/port-warden/dados/cron.log
```

## Desenvolvendo no Windows (XAMPP)

Crie `config.local.php` com os caminhos do Windows e `CRON_SIMULADO = true`
(veja `config.local.exemplo.php`). As linhas do "cron" vão para
`DIR_DADOS/crontab_simulado.txt`, e você roda o `cli/executar.php` à mão.
O MAC é obtido com `arp -a`.

## Antes da apresentação

- [ ] `GERENTE_SIMULAR = false`
- [ ] `MACS_FIXOS = []`
- [ ] MAC real da máquina do professor em `maquinas_autorizadas`
- [ ] `if_index` conferidos com `java -jar gerente.jar status 10.90.90.90`
- [ ] Senhas trocadas (banco e usuário `professor`)
- [ ] `bootstrap.min.css` presente em `assets/vendor/`

## Limitação conhecida

A identificação por MAC só funciona se a máquina do professor estiver na
**mesma rede local** que o servidor. Se houver um roteador no meio, a tabela
ARP mostra o MAC do roteador.
