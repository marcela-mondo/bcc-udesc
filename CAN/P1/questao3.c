#include <stdio.h>
#include <math.h>

#define N 4 

int fatLU(double A[N][N], double L[N][N], double U[N][N], int P[N]){
    for(int i=0; i<N; i++){
        P[i]=i;
        for(int j=0; j<N; j++){
            U[i][j]=A[i][j];
            
            if(i==j){
                L[i][j]=1;
            }else{
                L[i][j]=0;
            }
            
        }
    }
    
    for(int k=0; k<N; k++){
        double maxVal;
        int maxR=k;
        
        maxVal=fabs(U[k][k]);
        for(int i=k+1;i<N;i++){
            if(fabs(U[i][k]) > maxVal){
                maxVal=fabs(U[i][k]);
                maxR=i;
            }
        }
        
        if(maxR!=k){

            for(int j=0;j<N;j++){
                double t=U[k][j];
                U[k][j]=U[maxR][j];
                U[maxR][j]=t;
            }
            
            int tp=P[k];
            P[k]=P[maxR];
            P[maxR]=tp;
            
            for(int j=0; j<k; j++){
                double t=L[k][j];
                L[k][j]=L[maxR][j];
                L[maxR][j]=t;
            }
        }

        for(int i=k+1; i<N; i++){
            L[i][k] = U[i][k] / U[k][k];
            for(int j=k; j<N; j++){
                U[i][j] -= L[i][k] * U[k][j];
            }
        }
    }
    return 1;
}

//resolve ly=b
void resolucaoL(double L[N][N], double b[N], double y[N]){
    for(int i=0; i<N; i++){
        double soma=0;
        for(int j=0; j<i; j++){
            soma += L[i][j] * y[j];
        }
        y[i] = b[i] - soma;
    }
}

//resolve ux=y 
void resolucaoU(double U[N][N], double y[N], double x[N]){
    for(int i=N-1; i>=0; i--){
        double soma=0;
        for(int j=i+1; j<N; j++){
            soma += U[i][j] * x[j];
        }
        x[i] = (y[i] - soma) / U[i][i];
    }
}

void printMatriz(double m[N][N]){
    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            printf("%f  ", m[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    double A[N][N] = {{1,2,1,1},{2,4,5,2},{0,3,1,1},{1,2,4,5}}, L[N][N], U[N][N], AInv[N][N], e[N], y[N], x[N];
    int P[N];
    
    if(!fatLU(A, L, U, P)){
        printf("\nA matriz nao pode ser invertida");
    }
    
    for(int i=0; i<N; i++){
        for(int i=0; i<N; i++){
            e[i] = 0;
        }
        e[i]=1;
        
        double Pb[N];
        for(int k=0; k<N; k++){
            Pb[k]=e[P[k]];
        }
        
        resolucaoL(L, Pb, y);
        resolucaoU(U, y, x);
        
        for(int l=0; l<N; l++){
            AInv[l][i]=x[l];
        }
    }
    
    printf("\nQuestão 3\n\nMatriz inversa\n");
    printMatriz(AInv);

    return 0;
}