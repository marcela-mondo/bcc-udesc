#include <stdio.h>
#define RED "\x1B[31m"
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m"

void pos(int tamanho, int vetor[tamanho]){
    int i, cont_pos, cont_neg;
    
    for (i = 0; i < tamanho; i++){
        if(vetor[i] >= 0){
            cont_pos++;
        }
        if(vetor[i] < 0){
            cont_neg++;
        }
    }
    printf(RESET);
    printf("\n%d Valores sao POSITIVOS", cont_pos);
    printf("\n%d Valores sao NEGATIVOS", cont_neg);
}

void cor(int tamanho, int vetor[tamanho]){
    int i;
    for (i = 0; i < tamanho; i++){
        if(vetor[i] >= 0){
            printf(BLUE "\n %d", vetor[i]);
        }
        if(vetor[i] < 0){
            printf(RED "\n %d", vetor[i]);
            
        }
    
    }
}

int main(){
    int i, j, n, soma = 0;
    
    printf("\nQuantos valores deseja digitar? ");
    scanf("%d", &n);
    
    int vet[n];

    printf("Digite %d valores inteiros: \n", n);
    
    for(i = 0; i < n; i++){
        scanf("%d", &vet[i]);
    }
    
    cor(n, vet);
    pos(n, vet);
    
    printf(RESET);
    for(i = 0; i < n; i++){
        soma = soma + vet[i];
     }
     printf("\nSOMA = %d", soma);
    
    return 0;
}
