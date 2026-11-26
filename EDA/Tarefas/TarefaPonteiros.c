#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct curso{
    char *nome;
    float tempo;
} *cc;

struct estudante {
    char matricula[10];
    char* nome;
    struct curso *cursando;
} *e;

void imprimeEstudante(struct estudante *e);

int main(){
  e = malloc(sizeof(struct estudante));
  cc = malloc(sizeof(struct curso));
  e->cursando = malloc(sizeof(struct curso));
  e->nome = malloc(100 * sizeof(char));
  cc->nome = malloc(100 * sizeof(char));
  
  if(e == NULL || cc == NULL || e->cursando == NULL || e->nome == NULL || cc->nome == NULL){
    printf("\nErro ao alocar memoria!");
    return 1;
  }

  printf("\nDigite sua matricula: ");
  scanf("%s", e->matricula);
  printf("\nDigite seu nome: ");
  scanf("%s", e->nome);
  e->cursando = cc;
  printf("\nDigite o nome do curso: ");
  scanf("%s", (e->cursando)->nome);
  printf("\nDigite o tempo do curso: ");
  scanf("%f", &(e->cursando)->tempo);
  imprimeEstudante(e);

   free(e);
   free(cc);
   return 0;
}

void imprimeEstudante( struct estudante *e){
    printf("\nEstudante: %s", e->nome);
    printf("\nMatricula: %s", e->matricula);
    printf("\nCurso: %s", (e->cursando)->nome);
    printf("\nTempo: %.1f", (e->cursando)->tempo);
}