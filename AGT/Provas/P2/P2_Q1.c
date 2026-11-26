#include <stdio.h>
#include <stdlib.h>

int main(){
	int n, impares = 0, i, total = 0, contprimos = 0, ehprimo = 1;
	
	do{
		scanf("%d", &n);
		
        if (n > 0) {
            total++;

            if (n%2 != 0) impares++;
    
            ehprimo = 1;
            if (n == 1) ehprimo = 0;
            for (i = 2; i < n; i++){
                if (n%i == 0){
                    ehprimo = 0;
                    break;
                }
            }
            if (ehprimo == 1) contprimos++;
        }
	} while (n > 0);
	
   
    printf("\nImpares: %d", impares);
    printf("\nPrimos: %d", contprimos);
    printf("\nTotal: %d", total);
        
    return 0;
}
