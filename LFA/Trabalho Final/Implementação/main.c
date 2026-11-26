#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> 
#include <time.h>

#define RESET "\x1B[0m"
#define GRAY "\x1B[90m"
#define BOLD "\033[1m"
#define MAGENTA "\033[35m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"

typedef struct {
  char prod[50];
  double preco;
  int estoque;
} produtos;

typedef struct{
  float debito;
  float credito;
  float limiteCredito;
} cartao;

typedef struct{
  char produto[50];
  double preco;
  int quantidade;
  char formaPagamento[50];
  float troco;
  float reembolso;
} notaFiscal;

void ListarProdutos(produtos *fprod, int tamanho);
void VerificaProduto(produtos *fprod, int tamanho, int escolha);
void Rep_Sai_Est(produtos *fprod, int tamanho, int escolha);
void Pagamento(produtos *fprod, int tamanho, int escolha);
void EntregaProduto(produtos *fprod, int escolha, float dinheiroPagado, int cartao_ou_dinheiro, cartao *meuCartao, float dinheiroTroco);
void GerarNotaFiscal(produtos *fprod, int escolha, float valorPagado, int cartao_ou_dinheiro, float troco, int teve_erro);

int main(){
  int tamanhoProd = 1, tamanhoEstoque = 1, escolha = 0, escolhaDois;
  produtos *arqProdutos;
  FILE *arquivo;

  //Conta quantas linhas o arquivo tem
  char charac, letra = '\n';
  int vezes = 0;
  arquivo = fopen("produtos.txt", "r");
  while (fread(&charac, sizeof(char), 1, arquivo)) {
    if (charac == letra) {
      tamanhoProd++;
    }
  }
  fclose(arquivo);

  arqProdutos = malloc(sizeof(produtos) * tamanhoProd);
  if (arqProdutos == NULL) {
    printf("\nErro ao alocar memória!");
  }

  do{
    ListarProdutos(arqProdutos, tamanhoProd);
    printf("\n\nEscolha: ");
    scanf("%d", &escolha);
    if(escolha > tamanhoProd){
      printf(RED "\nSeleção inválida!\n" RESET);
    } else{
      if(escolha == 0){
        VerificaProduto(arqProdutos, tamanhoProd, escolha);
        break;
      }
      printf(BOLD "\nTem certeza que deseja selecionar o produto %d?\n" GREEN "1- SIM\n" RED "2- NAO\n" RESET "Digite: ", escolha);
      scanf("%d", &escolhaDois);
      if(escolhaDois == 1 || escolhaDois == 0){
        VerificaProduto(arqProdutos, tamanhoProd, escolha);
      } 
    }
  } while (escolha != 0);
  free(arqProdutos);
  return 0;
}

void ListarProdutos(produtos *fprod, int tamanho) {
  int escolhaLista;
  FILE *fileprod = fopen("produtos.txt", "r");
  if (fileprod == NULL) {
    printf(RED "\nErro ao abrir o arquivo produtos.txt\n" RESET);
    return;
  }

  printf(BOLD MAGENTA "▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄\n" RESET);
  printf(BOLD MAGENTA "▄                                        ▄\n▄     "
                      "MÁQUINA DE VENDAS AUTOMÁTICA       ▄\n" RESET);
  printf(BOLD MAGENTA "▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄\n" RESET);
  for (int i = 0; i < tamanho; i++) {
    fscanf(fileprod, "%s %lf %d", fprod[i].prod, &fprod[i].preco,
           &fprod[i].estoque);
    printf("\n %d - R$%.2lf - %s", i + 1, fprod[i].preco, fprod[i].prod);
  }
  printf("\n 0 - SAIR");
  printf("\n▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄");

  fclose(fileprod);
}

//Função que faz a verificação dos produtos antes da compra/reposição
void VerificaProduto(produtos *fprod, int tamanho, int escolha) {
  if (escolha==0){
    printf(RED "\nMáquina desligada...");
    return;
  }
  printf("\n...Verificando produto...");
  produtos *produto = &fprod[escolha - 1];

  FILE *fileprod = fopen("produtos.txt", "r");
  if (fileprod == NULL) {
    printf(RED "\nErro ao abrir o arquivo produtos.txt\n" RESET);
    return;
  }

  for (int i = 0; i < tamanho; i++) {
    fscanf(fileprod, "%s %lf %d", fprod[i].prod, &fprod[i].preco,
           &fprod[i].estoque);
  }

  int i = escolha-1;
  if (fprod[i].estoque == 0) {
    printf(RED "\nProduto fora de estoque" RESET);
    printf(YELLOW "\nENTRANDO EM MANUTENÇÃO\n");
    printf("\nRepondo...\n" RESET);

    while (fprod[i].estoque < 5) {
      fprod[i].estoque++;
      sleep(1);
      printf(GRAY "Estoque atual: %d\n" RESET, fprod[i].estoque);
    }

    printf(GREEN "Estoque do produto %s foi reabastecido para %d unidades!\n" RESET, fprod[i].prod, fprod[i].estoque);
    printf(MAGENTA "\nSelecione novemente para realizar a compra\n" RESET);
  } else {
    printf(GREEN "\nEstoque do produto %s: %d\n" RESET, fprod[i].prod,
           fprod[i].estoque);
           Rep_Sai_Est(fprod, tamanho, escolha);
  }

  FILE *atualiza = fopen("produtos.txt", "w");
  if (atualiza == NULL) {
    printf(RED "\nErro ao atualizar o arquivo produtos.txt\n" RESET);
    return;
  }
  for (int j = 0; j < tamanho; j++) {
      fprintf(atualiza, "%s %.2lf %d", fprod[j].prod, fprod[j].preco,
        fprod[j].estoque);
    if(j<tamanho-1){
      fprintf(atualiza, "\n");
    }
  }
  fclose(atualiza);
}


//Função que desconta o produto do estoque e atualiza o arquivo 
void Rep_Sai_Est(produtos *fprod, int tamanho, int escolha) {
    if (escolha == 0) {
        printf(RED "\nPrograma terminado..." RESET);
        return;
    }

    FILE *fileprod = fopen("produtos.txt", "r");
    if (fileprod == NULL) {
        printf(RED "\nErro ao abrir o arquivo produtos.txt\n" RESET);
        return;
    }

    // Carrega tds os produtos 
    for (int i = 0; i < tamanho; i++) {
        fscanf(fileprod, "%s %lf %d", fprod[i].prod, &fprod[i].preco,
               &fprod[i].estoque);
    }
    fclose(fileprod); 

    int i = escolha - 1;

    // desconta -1 do estoque
    if (fprod[i].estoque > 0) {
        fprod[i].estoque--; 
        printf(GREEN "\nProduto %s confirmado com sucesso!" RESET, fprod[i].prod);
        printf(GREEN "\nEstoque atual: %d" RESET, fprod[i].estoque);
    } 
   
    FILE *atualiza = fopen("produtos.txt", "w");
    if (atualiza == NULL) {
        printf(RED "\nErro ao atualizar o arquivo produtos.txt\n" RESET);
        return;
    }

    // reescreve o novo estoque
    for (int j = 0; j < tamanho; j++) {
        fprintf(atualiza, "%s %.2lf %d", fprod[j].prod, fprod[j].preco,
                fprod[j].estoque);

        if (j < tamanho - 1) { 
            fprintf(atualiza, "\n");
        }
    }
    fclose(atualiza); 
    Pagamento(fprod, tamanho, escolha); 
}

void Pagamento(produtos *fprod, int tamanho, int escolha){ 
  int forma_pag, pag_confirmado, pag_negado = 0, cartao_ou_dinheiro; /*cartao_ou_dinheiro == 1 é dinheiro, == 2 é deb, == 3 é cred*/ 
  int dinheiroinsuf = 0; //para chamar a função EntregaProduto
  cartao meuCartao;
  float dinheiroPagado;

  FILE *filecartao = fopen("cartao.txt", "r");
  if (filecartao == NULL) {
      printf(RED "\nErro ao abrir o arquivo cartao.txt\n" RESET);
      return;
  }
    fscanf(filecartao, "%f %f %f", &meuCartao.debito, &meuCartao.credito, &meuCartao.limiteCredito);
    fclose(filecartao); 
  
  do{
    dinheiroinsuf = 0;
    pag_negado = 0;
  printf("\nO total a ser pago -> R$ %.2lf", fprod[escolha-1].preco);
  printf("\nSelecione a forma de pagamento\n1- Dinheiro\n2- Cartão\n3- Sair\nDigite: ");
  scanf("%d", &forma_pag);
    
  switch(forma_pag){
    case 1:
      printf("\nInsira o dinheiro: ");
      scanf("%f", &dinheiroPagado);
      float dinheiroTroco=0;
      if(dinheiroPagado > fprod[escolha-1].preco){
        dinheiroTroco = dinheiroPagado - fprod[escolha-1].preco;
          printf(GREEN "\nTroco entregue: %.2f" RESET, dinheiroTroco);
          printf(GREEN "\nPagamento confirmado com sucesso!" RESET);
          dinheiroinsuf = 0;
        }
        if(dinheiroPagado < fprod[escolha-1].preco){
          printf(RED "\nDinheiro insuficiente\n" RESET);
          dinheiroinsuf = 1;
        } 
      if(dinheiroPagado == fprod[escolha-1].preco){
          printf(GREEN "\nPagamento confirmado com sucesso!\n" RESET);
          dinheiroinsuf = 0;
        }
      cartao_ou_dinheiro = 1;
      pag_negado = 0;
       if(dinheiroinsuf == 1){
         printf(RED"\nDEVOLVENDO R$%.2f...\n", dinheiroPagado);
         sleep(1);
         break;
       }
      EntregaProduto(fprod, escolha, dinheiroPagado, cartao_ou_dinheiro, &meuCartao, dinheiroTroco);
      
    break;
  
    case 2:
      printf("\nInsira o cartão..."); 
      printf(GREEN "\nCartão inserido" RESET "\n1- Debito\n2- Credito\n3- Sair");
      printf(BOLD "\nDEBITO: R$ %.2f \nCREDITO: R$ %.2f", meuCartao.debito, meuCartao.credito);
      printf(RESET "\nDigite: ");
      scanf("%d", &forma_pag);
      switch(forma_pag){
        case 1:
          dinheiroPagado = 0;
            dinheiroPagado = meuCartao.debito - fprod[escolha-1].preco;
            if(dinheiroPagado < 0){
                printf(RED "\nDEBITO INSUFICIENTE!\n" RESET); 
                pag_negado = 1;
                dinheiroinsuf = 1;
              return;
            } else{
              meuCartao.debito = dinheiroPagado;
              filecartao = fopen("cartao.txt", "w");
              if (filecartao == NULL) {
                printf(RED "\nErro ao atualizar o arquivo cartao.txt\n" RESET);
                return;
              }
              fprintf(filecartao, "%.2f %.2f %.2f", meuCartao.debito, meuCartao.credito, meuCartao.limiteCredito);
              fclose(filecartao);
              pag_negado = 0;
              cartao_ou_dinheiro = 2;
              EntregaProduto(fprod, escolha, fprod[escolha-1].preco, cartao_ou_dinheiro, &meuCartao, dinheiroTroco);
              }  
            break;

        case 2:
          dinheiroPagado = 0;
          dinheiroPagado = meuCartao.credito + fprod[escolha-1].preco;
          if(dinheiroPagado > meuCartao.limiteCredito){
            printf(RED "\nCRÉDITO INSUFICIENTE!\n" RESET); 
            pag_negado = 1;
            dinheiroinsuf = 1;
            return;
          } else{
            meuCartao.credito = dinheiroPagado;
            filecartao = fopen("cartao.txt", "w");
            if (filecartao == NULL) {
              printf(RED "\nErro ao atualizar o arquivo cartao.txt\n" RESET);
              return;
            }
            fprintf(filecartao, "%.2f %.2f %.2f", meuCartao.debito, meuCartao.credito, meuCartao.limiteCredito);
            fclose(filecartao);
            pag_negado = 0;
            cartao_ou_dinheiro = 3;
            EntregaProduto(fprod, escolha, fprod[escolha-1].preco, cartao_ou_dinheiro, &meuCartao, dinheiroTroco);
            }  
            break;
        case 3:
          pag_negado = 1;
        break;
          }
      }
  } while (pag_negado == 1);
  sleep(2);
  //system("clear");
}

//Função para entregar o produto e o "erro no sistema"
void EntregaProduto(produtos *fprod, int escolha, float dinheiroPagado, int cartao_ou_dinheiro, cartao *meuCartao, float dinheiroTroco){
  float troco = 0;
  //gerar numero aleatorio (simular aleatoriamente o erro de pagamento do sistema)
  //se erro_no_sistema == 1, entao da erro no sistema
  int erro_no_sistema = 0;
  srand(time(NULL));
  erro_no_sistema = rand() % 3 + 1; 

  
  if(erro_no_sistema==1){
    printf(RED "\nERRO NO SISTEMA" RESET);
    printf("\nReembolsando o valor pagado de" RED " R$%.2f\n", dinheiroPagado);
    if(cartao_ou_dinheiro == 1){
      printf(GREEN "\nR$%.2f REEMBOLSADO!\n" RESET, dinheiroPagado);
    }
    if(cartao_ou_dinheiro == 2){
      meuCartao->debito = meuCartao->debito + dinheiroPagado;
      printf(GREEN "\nR$%.2f REEMBOLSADO!\n" RESET, dinheiroPagado);
    }
    if(cartao_ou_dinheiro == 3){
    meuCartao->credito = meuCartao->credito - dinheiroPagado; 
    printf(GREEN "\nR$%.2f REEMBOLSADO!\n" RESET, dinheiroPagado);
    }
  }else{
    printf(GREEN "\nCompra do produto %s finalizada!", fprod[escolha-1].prod);
    printf(MAGENTA "\nEntregando produto...\n" RESET);
    sleep(1);
  }

  FILE *filecartao = fopen("cartao.txt", "w");
  if (filecartao == NULL) {
    printf(RED "\nErro ao atualizar o arquivo cartao.txt\n" RESET);
    return;
  }
  fprintf(filecartao, "%.2f %.2f %.2f", meuCartao->debito, meuCartao->credito, meuCartao->limiteCredito);
  fclose(filecartao); 

GerarNotaFiscal(fprod, escolha, dinheiroPagado, cartao_ou_dinheiro, dinheiroTroco, erro_no_sistema);
}

void GerarNotaFiscal(produtos *fprod, int escolha, float valorPagado, int cartao_ou_dinheiro, float troco, int teve_erro){
  notaFiscal nota;
  nota.reembolso = 0;

  strcpy(nota.produto, fprod[escolha - 1].prod);
  nota.preco = fprod[escolha - 1].preco;
  nota.quantidade = 1;
  if(cartao_ou_dinheiro == 1){
    strcpy(nota.formaPagamento, "Dinheiro");
  } 
  if (cartao_ou_dinheiro == 2){
    strcpy(nota.formaPagamento, "Cartão de débito");
  }
  if (cartao_ou_dinheiro == 3){
    strcpy(nota.formaPagamento, "Cartão de crédito");
  }
  nota.troco = troco;
  nota.reembolso = valorPagado;

  printf(GRAY "\n========================================\n");
  printf(GREEN BOLD "\tNOTA FISCAL - MAQUINA VENDAS" RESET);
  printf(GRAY "\n----------------------------------------\n" RESET);
  printf("\nPRODUTO: %s", nota.produto);
  printf("\nPREÇO: R$%.2f", nota.preco);
  printf("\nQUANTIDADE: %d unidade", nota.quantidade);
  printf("\nFORMA DE PAGAMENTO: %s", nota.formaPagamento);
  if(cartao_ou_dinheiro == 1){
    printf("\nTROCO: R$%.2f", nota.troco);
  }
  if(teve_erro == 1){
    printf(RED "\nREEMBOLSO: R$%.2f" RESET, nota.reembolso );
  } else{
    nota.reembolso = 0.0;
  }
   printf(GRAY "\n========================================\n" RESET);


  FILE *filenota = fopen("notafiscal.txt", "a");
  if (filenota == NULL) {
      printf(RED "\nErro ao abrir o arquivo notafiscal.txt\n" RESET);
      return;
  }
    fprintf(filenota, "Produto: %s R$%.2lf %d unidade - Pagamento: %s, Troco: R$%.2f - Reembolso: R$%.2f\n", nota.produto, nota.preco, nota.quantidade, nota.formaPagamento, nota.troco, nota.reembolso);
    fclose(filenota); 
}