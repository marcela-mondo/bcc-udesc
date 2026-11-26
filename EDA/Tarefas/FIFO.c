#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct No {
    char dado[50];
    struct No *anterior;
    struct No *proximo;
} No;

No* cabeca = NULL;
No* cauda = NULL;

int tamanho = 0;
int capacidade_maxima = 5; 

void inserir(char dado[]){
    No *novoNo;
    if(tamanho >= capacidade_maxima){
        printf("\nFila cheia!");
    }

    novoNo = (No*) malloc(sizeof(No));
    strcpy(novoNo->dado, dado);
    novoNo->proximo = NULL;
    novoNo->anterior = NULL;

    if(cabeca == NULL){
        cabeca = novoNo;    
        cauda = cabeca;
    }else{
        novoNo->proximo = cauda;
        cauda->anterior = novoNo;
        cauda = novoNo;
    }
    tamanho++;
}

No* remover(){
    No* p = cabeca;
    if(p == NULL){
        printf("\nFila vazia!");
    }

    cabeca = cabeca->anterior;
    if(cabeca != NULL){
        cabeca->proximo = NULL;
    }

    p->proximo = NULL;
    p->anterior = NULL;
    tamanho--;
    return p;
}

void valorFrente(){
    if(cabeca == NULL){
        printf("\nFila vazia!");
    }else{
        printf("\nElemento na frente: %s", cabeca->dado);
    }
}

void valorFim(){
    if(cabeca == NULL){
        printf("\nFila vazia!");
    }else{
        printf("\nElemento no fim: %s", cauda->dado);
    }
    printf("\nCauda\n");
}

void todosValores(){
    No* atual = cabeca;
    if(atual == NULL){
        printf("\nFila vazia!");
    }

    if(atual != NULL){
        printf("\nFila: ");
        while(atual != NULL){
            printf("\n%s <- ", atual->dado);
            atual = atual->anterior;
        }
    }
    
}

void estadoFila(){

    if(cabeca == NULL){
        printf("\nFila vazia!");
    }

    if(tamanho >= capacidade_maxima){
        printf("\nFila cheia!");
    }else{
        printf("\nFila aceita mais elementos!");
    }
}

void liberaMemoria(){
    No* p = cabeca;
    while(p != NULL){
        No* temp = p;
        p = p->anterior;
        free(temp);
    }
}

int main(){
    int opcao;
    char dado[50];
    do{
        printf("\n\n ------ MENU ------");
        printf("\n1. Inserir dados");
        printf("\n2. Valor de frente");
        printf("\n3. Valor no fim da fila");
        printf("\n4. Todos os valores");
        printf("\n5. Estado da fila");
        printf("\n6. Tamanho da fila");
        printf("\n0. Sair");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &opcao);
        switch (opcao){
        case 1:
            printf("\nDado a ser inserido: ");
            scanf("%s", dado);
            inserir(dado);
            break;
        case 2:
            valorFrente();
            break;
        case 3:
            valorFim();
            break;
        case 4:
            todosValores();
            break;
        case 5:
            estadoFila();
            break;
        case 6:
            printf("\nTamanho da fila: %d", tamanho);
            break;
        default:
            break;
        }
    }while(opcao != 0);
    liberaMemoria();
    return 0;
}
