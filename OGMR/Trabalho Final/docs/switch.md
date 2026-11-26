# Switch da bancada: dados levantados no laboratório

Preencher na primeira aula com o switch (Fase 2 do plano).

| Item | Valor |
|---|---|
| Modelo | D-Link DGS-3100-24 |
| Patrimônio | UDESC 104818 |
| IP | 10.90.90.90 (confirmar) |
| Máscara | 255.0.0.0 |
| SNMP habilitado? | ☐ sim ☐ não |
| Community de leitura | |
| Community de escrita | |
| Portas com defeito (fita) | 21, 22, 24 |
| Portas a 100 Mbps (fita) | 7, 8 |

## Porta física x ifIndex

Saída de `java -jar gerente.jar status 10.90.90.90`:

```
(colar aqui)
```

| Porta (frente) | ifIndex | ifDescr | Uso |
|---|---|---|---|
| 1 | | | servidor |
| 2 | | | professor |
| 3 | | | cobaia |
| 23 | | | uplink |

## MACs aprendidos

Saída de `java -jar gerente.jar macs 10.90.90.90`:

```
(colar aqui)
```

## Teste de bloqueio (porta 3)

- [ ] `bloquear 10.90.90.90 3` derrubou o ping da cobaia
- [ ] `desbloquear 10.90.90.90 3` restabeleceu
- [ ] LED da porta apagou/acendeu
