//QUESTÃO 1 
//caso o professor nao esteja conseguindo compilar, trocar o nome do arquivo para "main.c" !!

#include <stdio.h>
#include <math.h>

#define N 5

int main(){
    double x[N] = {0, 1, 2, 3, 4};
    double y[N] = {0.55, 0.47, 0.38, 0.33, 0.27};
    int i, k;
    
/*y = a*exp(-b*x) -> ln(y) = ln(a) - b*x
fazendo Y = ln(y), A = ln(a), B = -b -> Y = A+B*x */

    double sx = 0, sy = 0, sxx = 0, sxy = 0, Y[N];

    for(i=0; i<N; i++){
        Y[i] = log(y[i]);
        sx += x[i];
        sy += y[i];
        sxx += x[i] * x[i];
        sxy += x[i] * y[i];
    }

//minimos quadrados
    double B = (N*sxy-sx*sy)/(N*sxx-sx*sx);
    double A = (sy-B*sx)/N;
    double a = exp(A);
    double b = -B;

    printf("\nValores iniciais");
    printf("\na0 = %f", a);
    printf("\n\nb0 = %f", b);

//gauss newton
    for(k=1; k<=2; k++){
    double r[N], J[N][2], jtj[2][2] = {{0,0}, {0,0}}, jtr[2]={0, 0};
    
        for(i=0; i<N; i++){
            double f=a*exp(-b*x[i]);
            r[i] = y[i]-f;
        
            //derivadas parciais de f
            J[i][0] = exp(-b*x[i]);
            J[i][1] = -a*x[i]*exp(-b*x[i]);
        }
        
        for(i=0; i<N; i++){
            jtj[0][0] += J[i][0]*J[i][0];
            jtj[0][1] += J[i][0]*J[i][1];
            jtj[1][0] += J[i][1]*J[i][0];
            jtj[1][1] += J[i][1]*J[i][1];
            jtr[0] += J[i][0]*r[i];
            jtr[1] += J[i][1]*r[i];
        }
        
        double det = jtj[0][0]*jtj[1][1]-jtj[0][1]*jtj[1][0];
        double da = (jtr[0]*jtj[1][1]-jtj[0][1]*jtr[1])/det;
        double db = (jtj[0][0]*jtr[1]-jtr[0]*jtr[0]*jtj[1][0])/det;
        a=a+da;
        b=b+db;
        
        double soma = 0;
        for(i=0; i<N; i++){
            double e=y[i]-a*exp(-b*x[i]);
            soma+=e*e;
        }
        printf("\n\nIteracao %d: ", k);
        printf("\nda = %f  \ndb = %f", da, db);
        printf("\na = %f", a);
        printf("\nb = %f", b);
        printf("\n\nsoma dos residuos^2 = %e", soma);
    }
    
    printf("\nResultado após 2 iterações:");
    printf("\na = %f", a);
    printf("\nb = %f", b);
    printf("\ny = %f * exp(-%f * x)", a, b);
    
    return 0;
}



