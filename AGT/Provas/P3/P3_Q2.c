#include <stdio.h>
#include <stdlib.h>
#define MATERIAS 4
#define ALUNOS 15

float mediaGeral(float aluno[]){
	int quantAlunos7 = 0, i;

		if (aluno >= 7){
			quantAlunos7 = quantAlunos7 + 1;
		}
		return quantAlunos7;
	
}

float mediaDiscip(float alunos, float mediamateriaum, float mediamateriadois, float mediamateriatres, float mediamateriaquatro){
	int contalunos1, contalunos2, contalunos3, contalunos4;
	if (aluno >= mediamateriaum){
		contalunos = contalunos1 + 1;
	}
	
	if (aluno >= mediamateriadois){
		contalunos = contalunos2 + 1;
	}

	if (aluno >= mediamateriatres){
		contalunos = contalunos3 + 1;
	}
	
	if (aluno >= mediamateriaquatro){
		contalunos = contalunos4 + 1;
	}
	
	return contalunos1, contalunos2, contalunos3, contalunos4;
}

	
int main() {
	float notas[MATERIAS][ALUNOS]= {{8.9, 3, 7, 8.5, 9, 1, 7.5, 5.5, 6.5, 4.4, 4, 1.5, 7, 7.8, 6,3},
								  {9, 5, 9.7, 5, 7.9, 8.5, 5.5, 9.5, 6, 1.2, 5.6, 8, 9.2, 7.6, 7},
								  {7, 7, 8, 3, 10, 9.5, 6.2, 9, 8.7, 6.5, 3, 2, 9, 8, 8.5},
								  {8, 6, 10, 5.5, 8.5, 6.5, 9, 8, 7, 5, 5.5, 4, 10, 10, 9}
									};
									
	int i, j;
	float mediaPFN = 0, mediaAGT = 0, mediaICD = 0, mediaLMA = 0, mediaAluno = 0;

	for (i = 0; i < ALUNOS; i++){
		mediaPFN += notas[0][i]; 
		mediaAGT += notas[1][i]; 
		mediaICD += notas[2][i];  
		mediaLMA += notas[3][i]; 
	}
	
	mediaPFN = mediaPFN/15;
	mediaAGT = mediaAGT/15;
	mediaICD = mediaICD/15;
	mediaLMA = mediaLMA/15;

	for (i = 0; i < ALUNOS; i++){
		mediaAluno = 0;
		mediaAluno = notas[0][i] + notas[1][i] + notas[2][i] + notas[3][i];
		mediaAluno = mediaAluno/4;
		printf("\nA media do aluno %i eh: %.2f", i+1, mediaGeral(mediaAluno));
		printf("\nQuantidade de alunos que ficaram na media calculada: %.2f", mediaDiscip(mediaPFN, mediaAGT, mediaICD, mediaLMA, mediaAluno));
	}
	
	printf("\n");
	printf("\nA media geral de PFN eh: %.2f", mediaPFN);
	printf("\nA media geral de AGT eh: %.2f", mediaAGT);
	printf("\nA media geral de ICD eh: %.2f", mediaICD);
	printf("\nA media geral de LMA eh: %.2f", mediaLMA);
	
	return 0;
}
