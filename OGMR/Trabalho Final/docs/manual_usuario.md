# Manual do Usuário: Professor

**Port Warden**: controle de acesso à Internet das máquinas da sala de aula.

> Este manual é para quem usa o sistema no dia a dia (o professor).
> Configuração e manutenção estão no *Manual do Administrador*.

---

## 1. O que o sistema faz

Com o Port Warden você corta (e devolve) o acesso à Internet das máquinas da
sua sala, **agora** ou **num horário marcado**, sempre **por um prazo
definido**. Terminado o prazo, o acesso volta sozinho.

O bloqueio é feito **desligando a porta do switch** em que cada máquina está
ligada. Por isso, uma máquina bloqueada perde **toda** a rede, e não só o navegador.

---

## 2. Antes de começar

- O sistema **só abre no computador do professor** cadastrado para a sua sala.
  Em qualquer outro computador aparece a mensagem *"Acesso não autorizado"*.
  
  <img width="1074" height="346" alt="AcessoNaoAutoriz" src="https://github.com/user-attachments/assets/e76b3ba1-37ab-4590-8c8c-37ee38f538b2" />
  
- Você só vê e só controla **as máquinas da sala em que está**.
- Endereço do sistema: `http://<endereço-do-servidor>/port-warden/`
  (o administrador informa o endereço).

---

## 3. Entrar no sistema

1. No computador do professor, abra o navegador e acesse o endereço do sistema.
2. A tela de login mostra o nome da sua sala (ex.: *Sala F203*).
3. Digite **Usuário** e **Senha** e clique em **Entrar**.

Se aparecer *"Usuário ou senha inválidos"*, confira os dados ou procure o administrador.

Para sair, clique em **Sair**, no canto superior direito.

<img width="1186" height="474" alt="Login" src="https://github.com/user-attachments/assets/c4af6e43-7e66-40f6-afa3-5514c1719dfb" />

---

## 4. Conhecendo a tela

A tela tem três partes:

1. **Painel de ações** (em cima): quando bloquear, por quanto tempo e os botões.
2. **Grade de portas**: um cartão para cada porta da sala.
3. **Bloqueios ativos e agendados** (embaixo): a lista do que está valendo ou programado.
   
<img width="1588" height="784" alt="TelaInicial" src="https://github.com/user-attachments/assets/b0152bc0-bbb0-414e-9a28-7af8bc74304a" />

### 4.1 O que cada cartão mostra

- **Porta N**: número da porta no switch.
- **Nome, IP e MAC** da máquina ligada nela (ou *Porta livre*).
- **Situação**, com cor e cadeado:

| Situação | Cor | Significado |
|---|---|---|
| **Liberada** | verde, cadeado aberto | A máquina tem acesso à rede |
| **Bloqueada** | vermelho, cadeado fechado | Sem acesso. Abaixo aparece **"até HH:MM"** |
| **Protegida** | cinza | Porta do seu computador, do servidor ou da ligação com a rede. **Nunca é bloqueada** |
| **Com defeito** | cinza claro | Porta marcada como defeituosa pelo administrador |
| **Sem leitura** | amarelo tracejado | O sistema não conseguiu consultar o switch agora (ver seção 9) |

Outras indicações no cartão:

- **"agendado HH:MM"**: existe um bloqueio programado para começar nesse horário.
- **"sem cabo/desligada"**: a porta está liberada, mas não há nada ligado nela
  (máquina desligada ou cabo solto).

### 4.2 Atualização automática

A tela **se atualiza sozinha a cada poucos segundos**, sem precisar recarregar.
No topo aparece *"atualizado às HH:MM:SS"*, com um pontinho verde que pisca
durante a atualização. O resumo mostra quantas portas estão bloqueadas.

### 4.3 Buscar uma máquina

Use o campo **"Buscar por nome, IP, MAC ou porta"** para encontrar uma
máquina. A grade mostra só os cartões que combinam com o que foi digitado.
Exemplos: `PC03`, `10.90.90.103`, `aa:bb:cc`, `5`.

---

## 5. Bloquear algumas máquinas

1. **Clique nos cartões** das máquinas que quer bloquear. Os escolhidos ficam
   com borda azul e a caixinha marcada.
   - **selecionar todas** marca todas as portas que aparecem na tela.
     Combine com a busca: digite `PC0` e clique em *selecionar todas*.
   - **limpar** desmarca tudo.
2. Escolha **Agora** e a duração.
3. Clique em **Bloquear selecionadas** e confirme.

Embaixo do painel aparece quantas portas estão selecionadas.

<img width="1147" height="598" alt="SalaBloqueada" src="https://github.com/user-attachments/assets/83232092-f0f9-4c1b-831c-06980abf76cb" />

---

## 6. Bloquear a sala inteira (um clique)

1. Em **Quando bloquear**, deixe **Agora** marcado.
2. Em **Por quanto tempo**, informe a duração e escolha **minutos**, **horas** ou **dias**.
3. Clique em **Bloquear sala inteira**.
4. Confirme a mensagem *"Bloquear N porta(s) agora, por …?"*.

Todas as portas da sala são bloqueadas, **inclusive as livres** (assim ninguém
escapa ligando um notebook numa porta vazia). As portas protegidas e as com
defeito ficam de fora.

---

## 7. Agendar um bloqueio para depois

1. Em **Quando bloquear**, clique em **Agendar**. Aparece o campo **Início**.
2. Escolha a **data e hora de início**.
3. Informe **por quanto tempo** o bloqueio deve durar.
4. Clique em **Bloquear sala inteira** ou selecione as máquinas e clique em
   **Bloquear selecionadas**.

As portas continuam *Liberadas*, com a indicação **"agendado HH:MM"**, e o
agendamento aparece na tabela com a situação **Agendada**. No horário marcado
o bloqueio começa sozinho, mesmo que a tela esteja fechada.

**Bom saber:**

- Se você escolher um horário que **já passou**, o bloqueio começa **na hora**.
- Os horários são contados em **minutos cheios**. Um bloqueio de 50 minutos
  feito às 14:03:40 vai até **14:54**.
- O prazo máximo de um bloqueio é de **30 dias**.

---

## 8. Desbloquear antes do prazo

O desbloqueio acontece **sozinho** no fim do prazo. Para devolver o acesso
antes, há três formas:

| Quero… | Faça |
|---|---|
| Liberar **algumas máquinas** | Selecione os cartões e clique em **Desbloquear selecionadas** |
| Liberar **a sala toda** | Clique em **Liberar sala inteira** |
| Liberar/cancelar **um item da lista** | Na tabela de bloqueios, clique em **Liberar** (bloqueio ativo) ou **Cancelar** (agendado) |
| Cancelar **tudo de um mesmo clique** | Na tabela, clique em **Cancelar grupo (N)** |

Ao desbloquear uma porta, **todos os agendamentos futuros dela também são
cancelados**.

---

## 9. Perguntas frequentes

**Apareceu "Acesso não autorizado" no meu computador.**
Este computador não está cadastrado como computador do professor da sala
(ou foi trocado). Peça ao administrador para atualizar o cadastro.

**Não consigo selecionar uma porta.**
Portas **Protegidas** e **Com defeito** não podem ser bloqueadas, então não
têm caixinha. Isso evita que você corte a sua própria conexão com o sistema.

**Os cartões estão amarelos ("Sem leitura").**
O sistema não conseguiu falar com o switch. Os bloqueios já feitos continuam
valendo. Se persistir, avise o administrador.

**Bloqueei duas vezes a mesma máquina, com prazos diferentes.**
Ela fica bloqueada até o **maior** dos prazos. O fim do primeiro não libera a
máquina enquanto o segundo estiver valendo.

**O sistema pediu login de novo.**
A sessão expira se ficar muito tempo parada ou se o computador for trocado.
Basta entrar de novo: os bloqueios não são afetados.

**Fechei o navegador. Os bloqueios continuam?**
Sim. Os bloqueios e agendamentos ficam no servidor e o fim do prazo acontece
sozinho.

**Uma mensagem vermelha disse que algumas portas falharam.**
A mensagem lista quais portas e o motivo. As outras foram bloqueadas
normalmente. Tente de novo e, se persistir, avise o administrador.
