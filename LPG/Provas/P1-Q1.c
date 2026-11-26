#include <stdio.h>
#define RED     "\x1B[31m"
#define RESET   "\x1B[0m"

// 1) Escreva uma função que recebe um ponteiro de inteiros (V), 
// e os endereços de duas variáveis inteiras, min e max.
// Tal função deve armazenar, nas duas variáveis, o valor mínimo e
// máximo de um vetor (de valores inteiros).
// Escreva também a função main que permite utilizar tal função.

// IMPORTANTE: TODO o código DEVERÁ ser formulado através do uso de ARITMÉTICA DE PONTEIROS
// (implementações sem esta abordagem NÃO serão consideradas!).

void min_max(int *p, int *min, int *max, int n) {
    int i;
    for(i=0; i<n; i++){
        if(i==0){
            *min = *p;
            *max = *p;
        }
        else {
            if(*(p+i) < *min)
                *min = *(p+i);
            if(*(p+i) > *max)
                *max = *(p+i);
        }
    }
}

int main() {
    int min, max, n, i;

    printf("Digite o tamanho do vetor (n): ");
    scanf("%d", &n);
    int v[n];
    for(i=0; i<n; i++) {
        printf("Digite o valor da posição [%d] do vetor: ", i+1);
        scanf("%d", v+i);
    }
    min_max(&v, &min, &max, n);
    printf(RED "\n=> [ ");
    for(i=0; i<n; i++) {
        printf("%d", *(v+i));
        printf(" ");
    }
    printf("]\n");
    printf(RESET);
    printf("\nMIN = %d", min);
    printf("\nMAX = %d\n", max);
    return 0;
}