#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char codigo[10];
    char nome[30];
    int creditos;
}disciplina;

typedef struct {
    char nome[20];
    char cpf[12];
    int ano;
    disciplina *cursando;
}estudante;

estudante cadAluno(estudante *aluno);
void exibirDados(estudante *aluno);
void registraDisc(disciplina *materias[4]);
void matricularDisc(estudante *aluno, disciplina *materias[4]);


int main(){
    int op, quant = 0;
    estudante *aluno = NULL;
    disciplina *disciplinas[4] = {NULL};
    
    for(int i=0; i<4; i++){
        disciplinas[i] = malloc(sizeof(disciplina));
        if (disciplinas[i] == NULL) {
            printf("Erro ao alocar memória para disciplina %d\n", i);
        }
    }
    
    registraDisc(disciplinas);
    
    do{
        printf("\n");
        printf("\n1. Cadastrar aluno");
        printf("\n2. Matricular-se em disciplina");
        printf("\n3. Exibir dados");
        printf("\n0. Sair");
        printf("\nDigite aqui: ");
        scanf("%d", &op);   
        
    switch (op){
    case 1:
        aluno = malloc(sizeof(estudante));
        if(aluno == NULL){
            printf("\nErro ao alocar memoria!");
        }
        aluno->cursando = NULL;
        cadAluno(aluno);
        break;

    case 2:
        matricularDisc(aluno, disciplinas);
        break;
        
    case 3:
        exibirDados(aluno);
        break;
    
    default:
        break;
    }
    }while(op>0);
    
    free(aluno);
    for (int i = 0; i < 4; i++) {
        free(disciplinas[i]);
    }
 
    return 0;
}

estudante cadAluno(estudante *aluno){
    printf("\nNome: ");
    scanf("%s", aluno->nome);
    printf("\nCPF: ");
    scanf("%s", &aluno->cpf);
    printf("\nAno de ingresso: ");
    scanf("%d", &aluno->ano);
}

void exibirDados(estudante *aluno){
        printf("\n--------------");
        printf("\nnome: %s ", aluno->nome);
        printf("\ncpf: %s ", aluno->cpf);
        printf("\nano: %d ", aluno->ano);
        printf("\nDisciplina Matriculada:");
        printf("\nCódigo: %s", aluno->cursando->codigo);
        printf("\nNome: %s", aluno->cursando->nome);
        printf("\nCréditos: %d\n", aluno->cursando->creditos);
        printf("\n--------------");
}

void registraDisc(disciplina *materias[4]){
    strcpy(materias[0]->codigo, "EDA1001");
    strcpy(materias[1]->codigo, "EST0008");
    strcpy(materias[2]->codigo, "POO0001");
    strcpy(materias[3]->codigo, "SID0001");
    
    strcpy(materias[0]->nome, "ESTRUTURA DE DADOS I");
    strcpy(materias[1]->nome, "PROBABILIDADE E ESTATISTICA");
    strcpy(materias[2]->nome, "PROGRAMACAO ORIENTADA A OBJETOS");
    strcpy(materias[3]->nome, "SISTEMAS DIGITAIS");
    
    for(int i = 0; i<4; i++){
        materias[i]->creditos = 4;
    }
}

void matricularDisc(estudante *aluno, disciplina *materias[4]){
    char cod[10];
    int encontrado = 0;
    printf("\nCódigo da disciplina: ");
    scanf("%s", cod);
    for(int i=0; i<4; i++){
        if(strcmp(cod, materias[i]->codigo) == 0){
        aluno->cursando = malloc(sizeof(disciplina));
        if (aluno->cursando == NULL) {
                printf("Erro ao alocar memória para a disciplina do aluno.\n");
                return;
            }
            
            memcpy(aluno->cursando, materias[i], sizeof(disciplina));
            printf("Aluno matriculado com sucesso na disciplina %s.\n", materias[i]->nome);
            encontrado = 1;
            break;
        }
    }
    
    if (!encontrado) {
        printf("\nCódigo da disciplina invalido");
    }
    
}
