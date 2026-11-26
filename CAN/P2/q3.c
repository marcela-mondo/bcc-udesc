//QUESTAO 3 
//caso o professor nao esteja conseguindo compilar, trocar o nome do arquivo para "main.c" !!

#include <stdio.h>
#include <math.h>

#define N 8 

void resolveSistema(double A[N][N], double B[N], double X[N]){
    int i, j, k, max_lin;
    double max_val, fator;
    
    for(i=0; i<N; i++){
        max_val=fabs(A[i][i]);
        max_lin=i;
        for(k=i+1; k<N; k++){
            if(fabs(A[k][i]) > max_val){
            max_val=fabs(A[k][i]);
            max_lin=k;
            }
        }
    
    if(max_lin != i){
        for(j=0; j<N; j++){
            double temp = A[i][j];
            A[i][j]=A[max_lin][j];
            A[max_lin][j]=temp;
        }
        double temp=B[i];
        B[i]=B[max_lin];
        B[max_lin]=temp;
    }
    for(k=i+1; k<N; k++){
        fator=A[k][i]/A[i][i];
        for(j=i; j<N; j++){
            A[k][j] -= fator*A[i][j];
        }
        B[k]-=fator*B[i];
    }
}
for(i=N-1; i>=0; i--){
    X[i]=B[i];
    for(j=i+1; j<N; j++){
        X[i]-=A[i][j]*X[j];
    }
    X[i]/=A[i][i];
}
}

void printMatriz(double A[N][N], double B[N]){
    int i, j;
    printf("A*X = B:\n");
    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            printf("%6.1f ", A[i][j]);
        }
        printf(" %6.1f\n", B[i]);
    }
    printf("\n");
}

int main(){
    double A[N][N]={0}, B[N]={0}, X[N];
    
    //1. S0(1)=2 -> a0 = 2
    A[0][0]=1;
    B[0]=2;
    
    //2. S0(2)=3 -> a0+b0(1) + c0(1)^2 + d0(1)^3
    A[1][0]=1; 
    A[1][1]=1; 
    A[1][2]=1; 
    A[1][3]=1; 
    B[1]=3;
    
    //3. S1(2)=3 -> a1=3
    A[2][4]=1; 
    B[2]=3;
    
    //4. S1(3)=5 -> a1+b1(1)+c1(1)^2+d1(1)^3=5
    A[3][4]=1; 
    A[3][5]=1; 
    A[3][6]=1; 
    A[3][7]=1; 
    B[3]=5;
    
    //da 1 derivada x=2, b0+2c0*1+2c0*1+3*d0*1^2-b1=0
    A[4][1]=1; 
    A[4][2]=2; 
    A[4][3]=3; 
    A[4][5]=-1; 
    B[4]=0;
    
    //da 2 derivada x=2, 2*c0+6*d0*1-2*c1=0
    A[5][2]=2; 
    A[5][3]=6; 
    A[5][6]=-2; 
    B[5]=0;

    //x=1, 2*c0=0
    A[6][2]=2; 
    B[6]=0;

    //x=3, 2*c1+6*d1*1=0
    A[7][6]=2;
    A[7][7]=6; 
    B[7]=0;
    
    printMatriz(A, B);
    resolveSistema(A, B, X);
    
    printf("\nCoeficientes");
    printf("\nS0(x): a0 = %.4f, b0 = %.4f, c0 = %.4f, d0 = %.4f", X[0], X[1], X[2], X[3]);
    printf("\nS1(x): a1 = %.4f, b1 = %.4f, c1 = %.4f, d1 = %.4f", X[4], X[5], X[6], X[7]);

    return 0;
}