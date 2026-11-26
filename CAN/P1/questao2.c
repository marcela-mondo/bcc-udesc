#include <stdio.h>
#include <math.h>

//função f(x)= x^3 - exp(-2x) + 3
double f(double x){
    return pow(x,3)- exp(-2*x)+3;
}

//derivada de f(x): f'(x)=3*x^2-1
double derivadaF(double x){
    return 3*pow(x,2)+2*exp(-2*x);
}

void newtonRaphson(double x0, double tolerancia, int max_interacoes){
    double x, fx, dfx;
    printf("\nMétodo Newton Raphson");
    for(int i=1; i<=max_interacoes; i++){
        fx=f(x0);
        dfx=derivadaF(x0);
        
        x=x0- (fx/dfx);
        printf("\nIteração %d: x= %.6f, f(x) = %.6f", i, x, f(x));
        
        if(fabs(x-x0) < tolerancia){
            printf("\nConvergido em %d iterações. Raiz aproximada: %6f", i, x);
            return;
        }
        x0=x;
    }
}

void secante(double x0, double x1, double tolerancia, int max_interacoes){
    double x2, fx0, fx1;
    printf("\n\nMétodo Secante");
    for(int i=1; i<=max_interacoes; i++){
        double fx0=f(x0);
        double fx1=f(x1);
        
        x2=x1-fx1*(x1-x0)/(fx1-fx0);
        printf("\nIteração %d: x = %.6f, f(x) = %.6f", i, x2, f(x2));
        
        if(fabs(x2-x1) < tolerancia){
            printf("\nConvergido em %d iterações. \nRaiz aproximada: %.6f", i, x2);
            return;
        }
        x0=x1;
        x1=x2;
    }
}


int main()
{
    double tolerancia=1e-4; //criterio de parada (0,0001)
    int max_interacoes=2; 
    
    printf("\nQUESTÃO 2");
    newtonRaphson(1,tolerancia,max_interacoes);
    secante(1,2,tolerancia,max_interacoes);
    
    return 0;
}