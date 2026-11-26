//EXERCICIO 1

/*Implementar na linguagem C o algoritmo insertion-sort para classificação interna de um
vetor contendo M registros
○ Os valores para cada posição do vetor pode ser atribuído aleatoriamente
○ Utilize a include <time.h> e as funções srand e rand*/

// M -> tamanho

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void insertionSort(int vetor[], int tamanho);
void printVetor(int vetor[], int tamanho);

int main()
{
    int vetor[100];
    srand(time(NULL));
    int tamanho = rand() % 100;
   
    printf("\nVetor original: ");
    for(int i=0; i<tamanho; i++){
        vetor[i] = rand() % 100;
        printf("%d, ", vetor[i]);
    }
   
    insertionSort(vetor, tamanho);
    return 0;
}

void insertionSort(int vetor[], int tamanho){
    for(int i = 1; i<tamanho; i++){
        int atual = vetor[i];
        int j = i-1;
       
        while(j>=0 && vetor[j] > atual){
            vetor[j+1] = vetor[j];
            j=j-1;
        }
        vetor[j+1] = atual;
    }
    printVetor(vetor, tamanho);
}

void printVetor(int vetor[], int tamanho){
    printf("\nInsertion Sort: ");
    for(int i=0; i<tamanho; i++){

        printf("%d, ", vetor[i]);
    }
}