#include <stdio.h>

int main(void) {

  char produtos[4][35] = {"Apirina", "Sal de Fruta Eno Tônica",
                          "Fio Dental Colgate",
                          "Escova de Dente Colgate Slim Soft"};
  char informacoes[7][35] = {"Nome Comercial", "Tipo Embalagem/Produto",
                             "Quantidade",     "Fabricante/Laboratório",
                             "Tipo",           "Uso",
                             "Descrição"};
  char info_escova[7][50] = {"Escova de Dente Colgate Slim Soft 4 unidades",
                             "Embalagem plastificada",
                             "4 unidades",
                             "Colgate",
                             "Genérico",
                             "Adulto, pediátrico",
                             "Macia"};
  char info_fiodental[7][35] = {"Fio Dental Colgate Total 50m",
                                "Embalagem plastificada",
                                "50 m",
                                "Colgate",
                                "genérico",
                                "adulto, pediátrico",
                                "Fio encerado"};
  char info_aspirina[7][35] = {
      "Aspirina", "medicamento", "100 Comprimidos",        "EMS",
      "Genérico", "Pediátrico",  "Sabor morango, infantil"};
  char info_eno[7][26] = {
      "Sal de Fruta Eno Tônica", "medicamento", "100g", "Eno", "Genérico",
      "Adulto, pediátrico",      "20 doses"};
  float total = 0, preco[] = {8.99, 22.79, 10.89, 25.49};
  char nome[500];
  int contadores[4] = {0, 0, 0, 0};
  int i, j, escolher_tipo_produto, idade, escolher_menu_higiene,
      escolher_menu_medicamentos, recomprar, contador = 0, escolher_recomprar;

  printf("\nREALIZE SEU CADASTRO");
  printf("\nCadastre seu nome: ");
  scanf("%s", nome);
  printf("\nCadastre sua idade: ");
  scanf("%d", &idade);

  do { 
    printf("\n-----------------------------------");
    printf("\n%s BEM VINDO(A) A FARMÁCIA RENEGADOS", nome);
    printf("\n-----------------------------------");
    printf("\nESCOLHA O TIPO DE PRODUTO");
    printf("\n01. MEDICAMENTOS");
    printf("\n02. PRODUTOS DE HIGIENE");
    printf("\n03. PARAR DE COMPRAR");
    printf("\nDigite aqui: ");
    scanf("%i", &escolher_tipo_produto);
    switch (escolher_tipo_produto) {
    case 1:
      if (idade < 18) {
        printf("\nVOCÊ É DE MENOR E NÃO PODE COMPRAR ESSE PRODUTO\n");
        break;
      }
      printf("\nMEDICAMENTOS");
      printf("\n01. %s", produtos[0]);
      printf("\n02. %s", produtos[1]);
      printf("\n03. VOLTAR");
      printf("\nDigite aqui: ");
      scanf(" %i", &escolher_menu_medicamentos);
      switch (escolher_menu_medicamentos) {
      case 1:
        printf("\nTipo Produto %s", produtos[0]);
        printf("\nPreço: R$ %.2f", preco[0]);
        for (i = 0; i < 7; i++) {
          printf("\n%s:", informacoes[i]);
          printf(" %s", info_aspirina[i]);
          printf("\n");
        }
        do {
          printf("\n01. COMPRAR");
          printf("\n02. VOLTAR");
          printf("\nDigite aqui: ");
          scanf("%i", &recomprar);
          switch (recomprar) {
          case 1:
            printf("\nQuantas quantidades deseja comprar?");
            printf("\nDigite aqui: ");
            scanf("%i", &contador);
            contadores[0] += contador;
          }
        } while (recomprar != 2);        
        break;


      case 2:
        printf("\nTipo Produto %s", produtos[1]);
        printf("\nPreço: R$ %.2f", preco[1]);
        for (i = 0; i < 7; i++) {
          printf("\n%s:", informacoes[i]);
          printf(" %s", info_eno[i]);
          printf("\n");
        }
        do {
          printf("\n01. COMPRAR");
          printf("\n02. VOLTAR");
          printf("\nDigite aqui: ");
          scanf("%i", &recomprar);
          switch (recomprar) {
          case 1:
            printf("\nQuantas quantidades deseja comprar?");
            printf("\nDigite aqui: ");
            scanf("%i", &contador);
            contadores[1] += contador;
          }
        } while (recomprar != 2);

        break;
      default:
        break;
      }
      break;

    case 2:
      printf("\nPRODUTOS DE HIGIENE");
      printf("\n01. %s", produtos[2]);
      printf("\n02. %s", produtos[3]);
      printf("\n03. VOLTAR");
      printf("\nDigite aqui: ");
      scanf("%i", &escolher_menu_higiene);
      switch (escolher_menu_higiene) {
      case 2:
        printf("\nTipo Produto %s", produtos[3]);
        printf("\nPreço: R$ %.2f", preco[3]);
        for (i = 0; i < 7; i++) {
          printf("\n%s:", informacoes[i]);
          printf(" %s", info_escova[i]);
          printf("\n");
        }
        do {
          printf("\n01. COMPRAR");
          printf("\n02. VOLTAR");
          printf("\nDigite aqui: ");
          scanf("%i", &recomprar);
          switch (recomprar) {
          case 1:
            printf("\nQuantas quantidades deseja comprar?");
            printf("\nDigite aqui: ");
            scanf("%i", &contador);
            contadores[3] += contador;
          }
        } while (recomprar != 2);

        break;

      case 1:
        printf("\nTipo Produto %s", produtos[2]);
        printf("\nPreço: R$ %.2f", preco[2]);
        for (i = 0; i < 7; i++) {
          printf("\n%s:", informacoes[i]);
          printf(" %s", info_fiodental[i]);
          printf("\n");
        }
        do {
          printf("\n01. COMPRAR");
          printf("\n02. VOLTAR");
          printf("\nDigite aqui: ");
          scanf("%i", &recomprar);
          switch (recomprar) {
          case 1:
            printf("\nQuantas quantidades deseja comprar?");
            printf("\nDigite aqui: ");
            scanf("%i", &contador);
            contadores[2] += contador;
          }
        } while (recomprar != 2);

        break;
      default:
        break;
      }
      break;
    }

  } while (escolher_tipo_produto != 3); 

  for (j = 0; j < 4; j++) {
    total = total + (contadores[j] * preco[j]);
    printf("\nR$ %.2f - %d X %s", (contadores[j] * preco[j]), contadores[j],
           produtos[j]);
  }
  printf("\n-----------------------------------\n");
  printf("TOTAL: R$ %.2f ", total);

  if (total == 0) {
    printf("\nNenhum produto foi comprado.\n");
    printf("Obrigado por visitar a farmácia Renegados, volte sempre!");
  }

  return 0;
}