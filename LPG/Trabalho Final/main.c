#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#define RESET "\x1B[0m"
#define GRAY "\x1B[30m"
#define BOLD "\033[1m"
#define MAGENTA "\033[35m"
#define RED "\x1B[31m"

typedef struct{
  char id[5];
  char descricao[100]; 
  int lotacaoMax;
  struct{
    int computadores;
    int arCondicionado;
    int tablets;
    int projetores;
    int mesas;
    int cadeiras;
  } equipamentos;  
} sala;

typedef struct{
  char nomeUsuario[50]; 
  char idSala[5];
  struct{
    int dia;
    int mes;
    int ano;
    int hora; 
    int minuto;
  } data;
} reservas;

void ListarSalas(sala *fpSalas, int tamanho);//printar o arquivo das salas registradas
void ListarReservas(reservas *fpReservas, int tamanho);//printar o arquivo de salas reservadas
void ReservasSala(reservas *fpReservas, int tamanho);//printar todas as reservas de uma só sala
void AgendarSala(reservas *fpReservas, int tamanho);//alterar o arquivo de reservas e salvar
void CancelarReserva(reservas *fpReservas, int tamanho);// alterar o arquivo de disponibilidade de salas e salvar
void RegistrarSala(sala *fpSalas, int tamanho);//alterar o arquivo de salas existentes

int DigiteAqui(){
  int menu;
  printf(GRAY "\nDigite Aqui: ");
  scanf("%d", &menu);
  printf(RESET);
  return menu;
}

int main(void) {
  int escolha = 1, tamanhoSalas = 1, tamanhoReservas = 1;
  sala *arqSalas;
  reservas *arqReservas;

  //Verificação da quantidade de linhas dos arquivos
  FILE *arquivo;
  char charac, letra = '\n';
  int vezes=0;
  arquivo = fopen("salas.txt", "r");
  while(fread(&charac, sizeof(char), 1, arquivo)){
    if(charac == letra){
        tamanhoSalas++;
    }
  }
  fclose(arquivo);

  arquivo = fopen("reservas.txt", "r");
  while(fread(&charac, sizeof(char), 1, arquivo)){
    if(charac == letra){
        tamanhoReservas++;
    }
  }
  fclose(arquivo);

  //alocando memoria das salas
  arqSalas = malloc(sizeof(sala) * tamanhoSalas);
  if(arqSalas == NULL){
    printf("\nErro ao alocar memória");
  }

  //alocando memoria das reservas
  arqReservas = malloc(sizeof(sala) * tamanhoReservas);
  if(arqReservas == NULL){
    printf("\nErro ao alocar memória");
  }

  do{
    system("clear");
    printf("-------------------------------------" BOLD "\n\t\tMENU PRINCIPAL\n" RESET "-------------------------------------");
    printf("\n\t\t1. Listar Salas");
    printf("\n\t\t2. Listas Reservas");
    printf("\n\t\t3. Reservas de uma Sala");
    printf("\n\t\t4. Agendar Sala");
    printf("\n\t\t5. Cancelar Reserva");
    printf("\n\t\t6. Registrar Sala");
    printf("\n\t\t0. Fechar Programa");
    printf("\n-------------------------------------");
    escolha = DigiteAqui();
    switch (escolha) {
    case 1:
      system("clear");
      ListarSalas(arqSalas, tamanhoSalas);
      break;

    case 2:
      system("clear");
      ListarReservas(arqReservas, tamanhoReservas);
      break;

    case 3:
      system("clear");
      ReservasSala(arqReservas, tamanhoReservas);
      break;

    case 4:
      system("clear");
      //Aumentando o tamanho e realocando antes de chamar a função
      tamanhoReservas++;
      arqReservas = realloc(arqReservas, sizeof(sala) * tamanhoReservas);
      if(arqReservas == NULL){
        printf("\nErro ao realocar memória\n");
      }
      AgendarSala(arqReservas, tamanhoReservas);
      break;

    case 5:
      system("clear");
      CancelarReserva(arqReservas, tamanhoReservas);
      break;

    case 6:
      system("clear");
      //Aumentando o tamanho e realocando antes de chamar a função
      tamanhoSalas++;
      arqSalas = realloc(arqSalas, sizeof(sala) * tamanhoSalas);
      if(arqSalas == NULL){
        printf("\nErro ao realocar memória\n");
      }
      RegistrarSala(arqSalas, tamanhoSalas);
      break;

    default:
      break;
    }
  } while (escolha != 0);

  if (escolha == 0) {
    system("clear");
    printf(MAGENTA "\t\tObrigado por usar o SAS!");
  }
  system("clear");
  free(arqSalas);
  free(arqReservas);
  return 0;
}

//****************************** MENU LISTAR SALAS *************************************
void ListarSalas(sala *fpSalas, int tamanho){
  int escolhaListasSalas;
  
  printf("-------------------------------------" BOLD
         "\n\t\tLISTAR SALAS\n" RESET "-------------------------------------");
  printf(MAGENTA "\nAqui voce encontrará a listagem de\ntodas as salas "
                 "disponíveis no sistema e\nsuas informaçoes.\n" RESET);

  FILE *fileSalas = fopen("salas.txt", "r");
  if (fileSalas == NULL) {
    printf(RED "\nErro ao abrir o arquivo salas.txt\n" RESET);
    return;
  }

  for(int i = 0; i<tamanho; i++){
    fscanf(fileSalas, "%s %s %d %d %d %d %d %d %d", fpSalas[i].id, fpSalas[i].descricao, &fpSalas[i].lotacaoMax, &fpSalas[i].equipamentos.computadores, &fpSalas[i].equipamentos.arCondicionado, &fpSalas[i].equipamentos.tablets, &fpSalas[i].equipamentos.projetores, &fpSalas[i].equipamentos.mesas, &fpSalas[i].equipamentos.cadeiras);
    printf("\n-------------------------------------");
    printf("\nID: %s", fpSalas[i].id);
    printf("\nDESCRICAO: %s", fpSalas[i].descricao);
    printf("\nLOTAÇAO MAXIMA: %d", fpSalas[i].lotacaoMax);
    printf("\nEQUIPAMENTOS");
    printf("\nCOMPUTADOR: %d", fpSalas[i].equipamentos.computadores);
    printf("\nAR CONDICIONADO: %d", fpSalas[i].equipamentos.arCondicionado);
    printf("\nTABLET: %d", fpSalas[i].equipamentos.tablets);
    printf("\nPROJETOR: %d", fpSalas[i].equipamentos.projetores);
    printf("\nMESA: %d", fpSalas[i].equipamentos.mesas);
    printf("\nCADEIRA: %d", fpSalas[i].equipamentos.cadeiras);
    printf("\n-------------------------------------");
  }

  fclose(fileSalas);
  printf(GRAY "\n\nDIGITE 0 PARA VOLTAR" RESET);
  escolhaListasSalas = DigiteAqui();
}

//****************************** MENU LISTAR RESERVAS *************************************
void ListarReservas(reservas *fpReservas, int tamanho){
  int escolherLReservas;

  printf("-------------------------------------" BOLD
         "\n\t\tLISTAR RESERVAS\n" RESET
         "-------------------------------------");
  printf(MAGENTA "\nAqui voce encontrará a listagem de\ntodas as salas "
                 "reservas feitas\npara todas as salas." RESET);

  FILE *fileReservas = fopen("reservas.txt", "r");
  if(fileReservas == NULL){
    printf(RED "\nErro ao abrir o arquivo reservas.txt\n" RESET);
    return;
  }

  for(int i=0; i<tamanho; i++){
    fscanf(fileReservas, "%s %s %d %d %d %d %d", fpReservas[i].nomeUsuario, fpReservas[i].idSala, &fpReservas[i].data.dia, &fpReservas[i].data.mes, &fpReservas[i].data.ano, &fpReservas[i].data.hora, &fpReservas[i].data.minuto);
    printf("\n-------------------------------------");
    printf("\nNOME DO USUARIO: %s", fpReservas[i].nomeUsuario);
    printf("\nID SALA: %s", fpReservas[i].idSala);
    printf("\nDATA: %d/%d/%d", fpReservas[i].data.dia, fpReservas[i].data.mes, fpReservas[i].data.ano);
    printf("\nHORÁRIO: %.2d:%.2d", fpReservas[i].data.hora, fpReservas[i].data.minuto);
    printf("\n-------------------------------------");
  }

  fclose(fileReservas);
  printf(GRAY "\n\nDIGITE 0 PARA VOLTAR" RESET);
  escolherLReservas = DigiteAqui();
}

//****************************** MENU RESERVAS SALAS *************************************
void ReservasSala(reservas *fpReservas, int tamanho) {
  int escolherReservas;
  char pesquisaID[4];

  printf("-------------------------------------" BOLD "\n\t\tRESERVAS\n" RESET
         "-------------------------------------");
  printf(MAGENTA
         "\nAqui voce poderá visualizar todas as reserva de uma sala.\n");

  printf(GRAY "\nPS: TODAS AS LETRAS DEVEM SER MAIUSCULAS!");
  printf(MAGENTA "\nID DA SALA QUE DESEJA PESQUISAR: " RESET);
  scanf("%s", pesquisaID);

  FILE *fileReservas = fopen("reservas.txt", "r");
  if(fileReservas == NULL){
    printf(RED "\nErro ao abrir o arquivo reservas.txt\n" RESET);
    return;
  }

  int flag = 0;
  for(int i = 0; i<tamanho; i++){
    fscanf(fileReservas, "%s %s %d %d %d %d %d", fpReservas[i].nomeUsuario, fpReservas[i].idSala, &fpReservas[i].data.dia, &fpReservas[i].data.mes, &fpReservas[i].data.ano, &fpReservas[i].data.hora, &fpReservas[i].data.minuto);
    if(strcmp(pesquisaID, fpReservas[i].idSala) == 0){
        printf("\n-------------------------------------");
        printf("\nNOME DO USUARIO: %s", fpReservas[i].nomeUsuario);
        printf("\nID SALA: %s", fpReservas[i].idSala);
        printf("\nDATA: %d/%d/%d", fpReservas[i].data.dia, fpReservas[i].data.mes, fpReservas[i].data.ano);
        printf("\nHORÁRIO: %.2d:%.2d", fpReservas[i].data.hora, fpReservas[i].data.minuto);
        printf("\n-------------------------------------");
      flag = 1;
    }
  }

  if(flag == 0){
    printf(RED "\nSala não encontrada! Digite 0 para voltar ao menu principal\n\n" RESET);
  }

  fclose(fileReservas);
  printf(GRAY "\n\nDIGITE 0 PARA VOLTAR" RESET);
  escolherReservas = DigiteAqui();
}

//****************************** MENU AGENDAR SALAS *************************************
void AgendarSala(reservas *fpReservas, int tamanho){
  int escolhaAgendamentos, conflito = 0;

  printf("-------------------------------------" BOLD
         "\n\t\tAGENDAMENTOS\n" RESET "-------------------------------------");
  printf(MAGENTA
         "\nAqui voce poderá fazer a agendamento\nda reserva de uma sala.\n" RESET);

  FILE *fileReservas = fopen("reservas.txt", "a+");
  if(fileReservas == NULL){
    printf(RED "\nErro ao abrir o arquivo reservas.txt\n" RESET);
    return;
  }
  printf("\nAgendando sua sala...\n");
  printf(GRAY "\nOBS:Nomes devem ser digitados com _ ou sem espaço " RESET "\nNOME DO USUÁRIO: ");
  scanf("%s", fpReservas[tamanho].nomeUsuario);
  fprintf(fileReservas, "\n%s", fpReservas[tamanho].nomeUsuario);
  printf("ID SALA (LETRAS MAIUSCULAS): ");
  scanf("%s", fpReservas[tamanho].idSala);
  fprintf(fileReservas, " %s", fpReservas[tamanho].idSala);
  printf("\nDATA (dd/mm/aaaa)\n");
  printf("DIA: ");
  scanf("%d", &fpReservas[tamanho].data.dia);
  printf("MES: ");
  scanf("%d", &fpReservas[tamanho].data.mes);
  printf("ANO: ");
  scanf("%d", &fpReservas[tamanho].data.ano);
  do{
  printf(GRAY "\nOBS: O horario devera ser digitado separado. \nEX:(17 00)" RESET);
  printf("\nHora da reserva: ");
  scanf("%d", &fpReservas[tamanho].data.hora);
  printf("\nMinuto da reserva: ");
  scanf("%d", &fpReservas[tamanho].data.minuto);
    
  if ((fpReservas[tamanho].data.hora < 7 || (fpReservas[tamanho].data.hora == 7 && fpReservas[tamanho].data.minuto < 30)) || fpReservas[tamanho].data.hora >= 21) {
      printf(RED "\nHorário inválido! As reservas devem ser feitas entre 07:30 e 21:00.\n" RESET);
    }
  }while ((fpReservas[tamanho].data.hora < 7 || (fpReservas[tamanho].data.hora == 7 && fpReservas[tamanho].data.minuto < 30)) || fpReservas[tamanho].data.hora >= 21);

  FILE *fileteste = fopen("reservas.txt", "r");
  if(fileteste == NULL){
    printf(RED "\nErro ao abrir o arquivo reservas.txt\n" RESET);
    return;
  }

  reservas testeReserva;
  while(fscanf(fileteste, "%s %s %d %d %d %d %d", testeReserva.nomeUsuario, testeReserva.idSala, &testeReserva.data.dia, &testeReserva.data.mes, &testeReserva.data.ano, &testeReserva.data.hora, &testeReserva.data.minuto) != EOF){
    if(strcmp(testeReserva.idSala, fpReservas[tamanho].idSala) == 0 && testeReserva.data.dia == fpReservas[tamanho].data.dia && testeReserva.data.mes == fpReservas[tamanho].data.mes && testeReserva.data.ano == fpReservas[tamanho].data.ano && testeReserva.data.hora == fpReservas[tamanho].data.hora && testeReserva.data.minuto == fpReservas[tamanho].data.minuto){
      conflito = 1;
      break;
    }
  }
  fclose(fileteste);

  if(conflito){
    printf(RED "\nErro: Já existe uma reserva para esta sala no horário selecionado, tente novamente.\n" RESET);
    printf(GRAY "\n\nDIGITE 0 PARA VOLTAR" RESET);
    escolhaAgendamentos = DigiteAqui();
    return;
  }


  fprintf(fileReservas, " %d %d %d %d %d", fpReservas[tamanho].data.dia, fpReservas[tamanho].data.mes, fpReservas[tamanho].data.ano, fpReservas[tamanho].data.hora, fpReservas[tamanho].data.minuto);
  printf(MAGENTA "\nSala agendada com sucesso!" RESET);
  fclose(fileReservas);

}

//****************************** MENU CANCELAR RESERVA ************************************
void CancelarReserva(reservas *fpReservas, int tamanho) {
  int escolhaCancReserva;
  char nomeUsuario[50];
  int dia, mes, ano, hora, minuto;
  int reservasEncontradas = 0;
  int indices[tamanho];
  printf("-------------------------------------" BOLD
         "\n\t\tCANCELAMENTOS\n" RESET "-------------------------------------");
  printf(
      MAGENTA
      "\nAqui voce poderá fazer o cancelamento\nda reserva de uma sala.\n " RESET);

    printf("\nNOME DO USUÁRIO: ");
    scanf("%s", nomeUsuario);
    printf("DATA (dd/mm/aaaa)\n");
    printf("DIA: ");
    scanf("%d", &dia);
    printf("MES: ");
    scanf("%d", &mes);
    printf("ANO: ");
    scanf("%d", &ano);

     //apenas ler pra comparar
    FILE *fileReservas = fopen("reservas.txt", "r");
    if (fileReservas == NULL) {
        printf(RED "\nErro ao abrir o arquivo reservas.txt\n" RESET);
        return;
    }

    // Carregar todas as reservas
    for (int i = 0; i < tamanho; i++) {
        fscanf(fileReservas, "%s %s %d %d %d %d %d", fpReservas[i].nomeUsuario, fpReservas[i].idSala, &fpReservas[i].data.dia, &fpReservas[i].data.mes, &fpReservas[i].data.ano, &fpReservas[i].data.hora, &fpReservas[i].data.minuto);
    }
    fclose(fileReservas);

    int flag = 0;
    // Lista as reservas
    printf("\nReservas encontradas:\n");
    for (int i = 0; i < tamanho; i++) {
        if (strcmp(fpReservas[i].nomeUsuario, nomeUsuario) == 0 &&
            fpReservas[i].data.dia == dia &&
            fpReservas[i].data.mes == mes &&
            fpReservas[i].data.ano == ano) {
            reservasEncontradas++;
            indices[reservasEncontradas - 1] = i;
            printf("\n-------------------------------------");
            printf(BOLD "\n\t[ %d ]" RESET "\n\tSala: %s\n\tData: %02d/%02d/%04d\n\tHorário: %.2d:%.2d", reservasEncontradas, fpReservas[i].idSala, fpReservas[i].data.dia, fpReservas[i].data.mes, fpReservas[i].data.ano, fpReservas[i].data.hora, fpReservas[i].data.minuto);
            printf("\n-------------------------------------");
            flag = 1;
        }
    }

    if (reservasEncontradas == 0) {
        printf(RED "\nNenhuma reserva encontrada para o usuário %s na data %02d/%02d/%04d\n" RESET, nomeUsuario, dia, mes, ano);
    }


    if(flag == 1){
      printf("\nDigite o número da reserva que deseja cancelar: ");
      scanf("%d", &escolhaCancReserva);

      if (escolhaCancReserva < 1 || escolhaCancReserva > reservasEncontradas){
          printf(RED "\nOpção inválida!\n" RESET);
          escolhaCancReserva = DigiteAqui();
          return;
      }

    // Remove a reserva escolhida
    int removind = indices[escolhaCancReserva - 1];
    for (int i = removind; i < tamanho - 1; i++) {
        fpReservas[i] = fpReservas[i + 1];
    }
    tamanho--;

    // Salva as reservas atualizadas no arquivo
    fileReservas = fopen("reservas.txt", "w");
    if (fileReservas == NULL) {
        printf(RED "\nErro ao abrir o arquivo reservas.txt\n" RESET);
        return;
    }
    for (int i = 0; i < tamanho; i++) {
        fprintf(fileReservas, "%s %s %d %d %d %d %d\n", fpReservas[i].nomeUsuario, fpReservas[i].idSala, fpReservas[i].data.dia, fpReservas[i].data.mes, fpReservas[i].data.ano, fpReservas[i].data.hora, fpReservas[i].data.minuto);
    }
    fclose(fileReservas);

    printf(MAGENTA "\nReserva cancelada com sucesso!\n" RESET);
}

    printf(GRAY "\n\nDIGITE 0 PARA VOLTAR" RESET);
    escolhaCancReserva = DigiteAqui();
}


//****************************** MENU REGISTRAR SALAS *************************************
void RegistrarSala(sala *fpSalas, int tamanho) {
  int escolhaRegistReserva;
  printf("-------------------------------------" BOLD
         "\n\t\tREGISTRO DE SALAS\n" RESET "-------------------------------------");
  printf(MAGENTA
         "\nAqui voce poderá registrar salas.\n " RESET);

  FILE *fileSalas = fopen("salas.txt", "a+");
  if (fileSalas == NULL) {
      printf(RED "\nErro ao abrir o arquivo salas.txt\n" RESET);
      return;
  }

  // Carregar todas as salas existentes para comparação
  for (int i = 0; i < tamanho - 1; i++) {
      fscanf(fileSalas, "%s %s %d %d %d %d %d %d %d", fpSalas[i].id, fpSalas[i].descricao, &fpSalas[i].lotacaoMax, &fpSalas[i].equipamentos.computadores, &fpSalas[i].equipamentos.arCondicionado, &fpSalas[i].equipamentos.tablets, &fpSalas[i].equipamentos.projetores, &fpSalas[i].equipamentos.mesas, &fpSalas[i].equipamentos.cadeiras);
  }

    printf("\nRegistrando nova sala...");
    printf("\nID (LETRAS MAIUSCULAS): ");
    scanf("%s", fpSalas[tamanho - 1].id);
    printf("\nDESCRIÇÃO: ");
    scanf(" %[^\n]", fpSalas[tamanho - 1].descricao);
    printf("\nLOTAÇÃO MÁXIMA: ");
    scanf("%d", &fpSalas[tamanho - 1].lotacaoMax);
    printf("\nQuantidade de computadores: ");
    scanf("%d", &fpSalas[tamanho - 1].equipamentos.computadores);
    printf("\nQuantidade de ares condicionados: ");
    scanf("%d", &fpSalas[tamanho - 1].equipamentos.arCondicionado);
    printf("\nQuantidade de tablets: ");
    scanf("%d", &fpSalas[tamanho - 1].equipamentos.tablets);
    printf("\nQuantidade de projetores: ");
    scanf("%d", &fpSalas[tamanho - 1].equipamentos.projetores);
    printf("\nQuantidade de mesas: ");
    scanf("%d", &fpSalas[tamanho - 1].equipamentos.mesas);
    printf("\nQuantidade de cadeiras: ");
    scanf("%d", &fpSalas[tamanho - 1].equipamentos.cadeiras);

  // Verificar duplicidade
  for (int i = 0; i < tamanho - 1; i++) {
      if (strcmp(fpSalas[i].id, fpSalas[tamanho - 1].id) == 0) {
          printf(MAGENTA "\nUma sala com esse ID já está registrada. Tente outro ID." RESET);
          fclose(fileSalas);
          return;
      }
  }

  // Adicionar nova sala ao arquivo
  fprintf(fileSalas, "\n%s %s %d %d %d %d %d %d %d", fpSalas[tamanho - 1].id, fpSalas[tamanho - 1].descricao, fpSalas[tamanho - 1].lotacaoMax, fpSalas[tamanho - 1].equipamentos.computadores, fpSalas[tamanho - 1].equipamentos.arCondicionado, fpSalas[tamanho - 1].equipamentos.tablets, fpSalas[tamanho - 1].equipamentos.projetores, fpSalas[tamanho - 1].equipamentos.mesas, fpSalas[tamanho - 1].equipamentos.cadeiras);
  printf(MAGENTA "\nSala registrada com sucesso!" RESET);
  fclose(fileSalas);

  printf(GRAY "\n\nDIGITE 0 PARA VOLTAR" RESET);
  escolhaRegistReserva = DigiteAqui();
  }