#include <stdio.h>
#include <stdlib.h>

int main(){
	float casaUm[12] = {80, 250.40, 350.20, 400, 200, 150, 130, 135.50, 250, 300, 580, 400};
    float casaDois[12] = {280, 200, 210, 230.50, 200.20, 250.60, 230, 210.80, 240.44, 230, 210, 600};
    int i, mesCasaUm, mesCasaDois;
    float sMediaUm = 0, sMediaDois = 0, mediaUm, mediaDois, maisCaraUm = 0, maisCaraDois = 0;;

    for (i = 0; i < 12; i++){
        sMediaUm = sMediaUm + casaUm[i];
        if (casaUm[i] > maisCaraUm){
            maisCaraUm = casaUm[i];
            mesCasaUm = i + 1;
        }
    }
    mediaUm = sMediaUm/12;

    for (i = 0; i < 12; i++){
        sMediaDois = sMediaDois + casaDois[i];
        if (casaDois[i] > maisCaraDois){
            maisCaraDois = casaDois[i];
            mesCasaDois = i + 1;
        }
    }
    mediaDois = sMediaDois/12;
	
    printf("\nMEDIA ANUAL CASA 1 %.2f", mediaUm);
    printf("\nMEDIA ANUAL CASA 2 %.2f\n", mediaDois);
    printf("\nCONTA MAIS CARA CASA 1 %.2f", maisCaraUm);
    printf("\nCONTA MAIS CARA CASA 2 %.2f", maisCaraDois);

    if (mesCasaUm == mesCasaDois){
        printf("\nMES DE MAIOR CONSUMO COINCIDEM\nMES: %i", mesCasaUm);
    }
    else printf("\nMES DE MAIOR CONSUMO NAO COINCIDEM");

	return 0;
}