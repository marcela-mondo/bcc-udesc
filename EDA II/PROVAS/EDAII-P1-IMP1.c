/*Dado um conjunto de N números inteiros representando o tamanho de luvas, 
faça um programa para contar o máximo possível de pares de luvas existentes no conjunto. 
Note que uma luva só pode ser par de outra luva do mesmo tamanho e só pode fazer parte de um único par. 
Exemplo: entrada = {6, 5, 2, 3, 5, 2, 2, 1}, saída = 2 pares (tamanho 5 e tamanho 2)*/
#include <stdio.h>
#include <stdlib.h>

#define TAM 100 

int ParLuvas (int *luvas, int n){
    int qnt[TAM] = {0};
    int pares = 0;
    
    for(int i=0; i<n; i++){
        if(luvas[i] < TAM){
            qnt[luvas[i]]++;
        }
    }
    
    for (int i=0; i<TAM; i++){
        pares += qnt[i]/2;
    }
    return pares;
}

int main()
{
    int n;
    
    printf("\nQuantidade de luvas: ");
    scanf("%d", &n);
    
    int *luvas = (int*)malloc(n * sizeof(int));
    if (luvas == NULL){
        printf("\nErro ao alocar memoria");
    }
    
    printf("\nTamanho das luvas: ");
    for(int i=0; i<n; i++){
        scanf("%d", &luvas[i]);
    }
    
    printf("\nEntrada = { ");
        for(int i=0; i<n; i++){
        printf("%d ", luvas[i]);
    }
    printf(" }");
    
    printf("\nPares = %d", ParLuvas(luvas, n));
    free(luvas);

    return 0;
}