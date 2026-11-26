#include <stdio.h>

int fatorial(int x){
    if(x == 0){
        return 1;
    }
    else{
        return x * fatorial(x-1);
    }
}

int main(){
    int n1 = 1, n2 = 0, fat;
    
    do{
        printf("\nForneca dois numeros inteiros positivos\nOBS: N2 < N1");
        printf("\n\nValor de N1 = ");
        scanf("%d", &n1);
        printf("\nValor de N2 = ");
        scanf("%d", &n2);
    }while((n1 > n2) || (n1 <= 0) || (n2 <= 0));
    
    int i = n1;
    while(i <= n2){
        printf("%d ", fatorial(i));
        i++;
        
    }
    
    return 0;
}
