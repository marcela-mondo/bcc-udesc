#include <stdio.h>
#define RED "\x1B[31m"
#define RESET "\x1b[0m"

void cont_regr(int numero, int flag){
    
    if(flag == 1){
        printf(RED " [ %d ]", numero);
        printf(RESET);
    }
    if(flag == 0){
        printf(" %d", numero);
        printf(RESET);
    }
}
int main()
{
    int n, guarda, flag = 0;
    guarda = n;
    printf("Digite o valor de N = ");
    scanf("%d", &n);
    
    while(n >= 1){
        if(n % 4 == 0){
            flag = 1;
            cont_regr(n, flag);
        }
        else{
            flag = 0;
            cont_regr(n, flag);
        }
        
        n--;
    }
 
    
    return 0;
}

