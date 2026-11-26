#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ELEMENTOS 10000
#define NUM_AMOSTRAS 10
#define NUM_TAMANHOS 18

static long long contador_ops = 0;
#define CONTAR() (contador_ops++)

//ARVORE AVL
typedef struct NoAVL{ 
  int chave;
  int altura; 
  struct NoAVL *esq, *dir; 
}NoAVL;

static int avl_altura(NoAVL *no){ return no ? no->altura : 0;}

static void avl_atualiza_altura(NoAVL *no){
    int alt_esq = avl_altura(no->esq), alt_dir = avl_altura(no->dir);
    no->altura = 1 + (alt_esq > alt_dir ? alt_esq : alt_dir);
}

static NoAVL *avl_criar_no(int chave){
    NoAVL *no = calloc(1, sizeof *no);
    no->chave = chave;
    no->altura = 1;
    return no;
}

//rotacao para a direita
static NoAVL *avl_rotacao_direita(NoAVL *y){ CONTAR();
    NoAVL *x = y->esq, *tmp = x->dir;
    x->dir = y; y->esq = tmp;
    avl_atualiza_altura(y); avl_atualiza_altura(x);
    return x;
}

//rotacao para a esquerda
static NoAVL *avl_rotacao_esquerda(NoAVL *x){ CONTAR();
    NoAVL *y = x->dir, *tmp = y->esq;
    y->esq = x; x->dir = tmp;
    avl_atualiza_altura(x); avl_atualiza_altura(y);
    return y;
}

static NoAVL *avl_balancear(NoAVL *no){
    CONTAR();
    avl_atualiza_altura(no);
    int fator = avl_altura(no->esq) - avl_altura(no->dir);
    if(fator > 1){
        if(avl_altura(no->esq->esq) < avl_altura(no->esq->dir))
            no->esq = avl_rotacao_esquerda(no->esq);
        return avl_rotacao_direita(no);
    }
    if(fator < -1){
        if(avl_altura(no->dir->dir) < avl_altura(no->dir->esq))
            no->dir = avl_rotacao_direita(no->dir);
        return avl_rotacao_esquerda(no);
    }
    return no;
}

static NoAVL *avl_inserir(NoAVL *no, int chave){
    CONTAR();
    if(!no) return avl_criar_no(chave);
    if(chave < no->chave)      no->esq = avl_inserir(no->esq, chave);
    else if(chave > no->chave) no->dir = avl_inserir(no->dir, chave);
    else return no; 
    return avl_balancear(no);
}

static NoAVL *avl_minimo(NoAVL *no){ while(no->esq){ CONTAR(); no = no->esq; } return no; }

static NoAVL *avl_remover(NoAVL *no, int chave){
    CONTAR();
    if(!no) return NULL;
    if(chave < no->chave)      no->esq = avl_remover(no->esq, chave);
    else if(chave > no->chave) no->dir = avl_remover(no->dir, chave);
    else{
        if(!no->esq || !no->dir){
            NoAVL *tmp = no->esq ? no->esq : no->dir;
            free(no);
            return tmp;
        }
        NoAVL *suc = avl_minimo(no->dir);
        no->chave = suc->chave;
        no->dir = avl_remover(no->dir, suc->chave);
    }
    return avl_balancear(no);
}

static void avl_liberar(NoAVL *no){
    if(!no) return;
    avl_liberar(no->esq);
    avl_liberar(no->dir);
    free(no);
}

//Arvore Rubro-Negras
#define VERMELHO 1
#define PRETO 0

typedef struct NoRN{ 
  int chave;
  int cor;
  struct NoRN *esq, *dir, *pai; 
}NoRN;

typedef struct{ 
  NoRN *raiz, *nulo; 
}ArvoreRN;

static ArvoreRN *rn_criar(){
    ArvoreRN *arv = malloc(sizeof *arv);
    arv->nulo = calloc(1, sizeof(NoRN));
    arv->nulo->cor = PRETO;
    arv->raiz = arv->nulo;
    return arv;
}

static void rn_rotacao_esquerda(ArvoreRN *arv, NoRN *x){ CONTAR();
    NoRN *y = x->dir; x->dir = y->esq;
    if(y->esq != arv->nulo) y->esq->pai = x;
    y->pai = x->pai;
    if(x->pai == arv->nulo)      arv->raiz = y;
    else if(x == x->pai->esq)   x->pai->esq = y;
    else                         x->pai->dir = y;
    y->esq = x; x->pai = y;
}

static void rn_rotacao_direita(ArvoreRN *arv, NoRN *x){ CONTAR();
    NoRN *y = x->esq; x->esq = y->dir;
    if(y->dir != arv->nulo) y->dir->pai = x;
    y->pai = x->pai;
    if(x->pai == arv->nulo)      arv->raiz = y;
    else if(x == x->pai->dir)   x->pai->dir = y;
    else                         x->pai->esq = y;
    y->dir = x; x->pai = y;
}

static void rn_corrigir_insercao(ArvoreRN *arv, NoRN *z){
    while(z->pai->cor == VERMELHO){ CONTAR();
        if(z->pai == z->pai->pai->esq){
            NoRN *tio = z->pai->pai->dir;
            if(tio->cor == VERMELHO){
                z->pai->cor = PRETO; tio->cor = PRETO;
                z->pai->pai->cor = VERMELHO; z = z->pai->pai;
            } else {
                if(z == z->pai->dir){ z = z->pai; rn_rotacao_esquerda(arv, z); }
                z->pai->cor = PRETO; z->pai->pai->cor = VERMELHO;
                rn_rotacao_direita(arv, z->pai->pai);
            }
        } else {
            NoRN *tio = z->pai->pai->esq;
            if(tio->cor == VERMELHO){
                z->pai->cor = PRETO; tio->cor = PRETO;
                z->pai->pai->cor = VERMELHO; z = z->pai->pai;
            } else {
                if(z == z->pai->esq){ z = z->pai; rn_rotacao_direita(arv, z); }
                z->pai->cor = PRETO; z->pai->pai->cor = VERMELHO;
                rn_rotacao_esquerda(arv, z->pai->pai);
            }
        }
    }
    arv->raiz->cor = PRETO;
}

static void rn_inserir(ArvoreRN *arv, int chave){ CONTAR();
    NoRN *z = malloc(sizeof *z);
    z->chave = chave; z->cor = VERMELHO; z->esq = z->dir = z->pai = arv->nulo;
    NoRN *pai = arv->nulo, *atual = arv->raiz;
    while(atual != arv->nulo){ CONTAR(); pai = atual; atual = chave < atual->chave ? atual->esq : atual->dir; }
    z->pai = pai;
    if(pai == arv->nulo)          arv->raiz = z;
    else if(chave < pai->chave)  pai->esq = z;
    else                          pai->dir = z;
    rn_corrigir_insercao(arv, z);
}

static void rn_transplantar(ArvoreRN *arv, NoRN *u, NoRN *v){
    if(u->pai == arv->nulo)      arv->raiz = v;
    else if(u == u->pai->esq)   u->pai->esq = v;
    else                         u->pai->dir = v;
    v->pai = u->pai;
}

static NoRN *rn_minimo(ArvoreRN *arv, NoRN *x){ while(x->esq != arv->nulo){ CONTAR(); x = x->esq; } return x; }

static void rn_corrigir_remocao(ArvoreRN *arv, NoRN *x){
    while(x != arv->raiz && x->cor == PRETO){ CONTAR();
        if(x == x->pai->esq){
            NoRN *irm = x->pai->dir;
            if(irm->cor == VERMELHO){ irm->cor = PRETO; x->pai->cor = VERMELHO; rn_rotacao_esquerda(arv, x->pai); irm = x->pai->dir; }
            if(irm->esq->cor == PRETO && irm->dir->cor == PRETO){ irm->cor = VERMELHO; x = x->pai; }
            else {
                if(irm->dir->cor == PRETO){ irm->esq->cor = PRETO; irm->cor = VERMELHO; rn_rotacao_direita(arv, irm); irm = x->pai->dir; }
                irm->cor = x->pai->cor; x->pai->cor = PRETO; irm->dir->cor = PRETO;
                rn_rotacao_esquerda(arv, x->pai); x = arv->raiz;
            }
        } else {
            NoRN *irm = x->pai->esq;
            if(irm->cor == VERMELHO){ irm->cor = PRETO; x->pai->cor = VERMELHO; rn_rotacao_direita(arv, x->pai); irm = x->pai->esq; }
            if(irm->dir->cor == PRETO && irm->esq->cor == PRETO){ irm->cor = VERMELHO; x = x->pai; }
            else {
                if(irm->esq->cor == PRETO){ irm->dir->cor = PRETO; irm->cor = VERMELHO; rn_rotacao_esquerda(arv, irm); irm = x->pai->esq; }
                irm->cor = x->pai->cor; x->pai->cor = PRETO; irm->esq->cor = PRETO;
                rn_rotacao_direita(arv, x->pai); x = arv->raiz;
            }
        }
    }
    x->cor = PRETO;
}

static void rn_remover(ArvoreRN *arv, int chave){ CONTAR();
    NoRN *z = arv->raiz;
    while(z != arv->nulo && z->chave != chave){ CONTAR(); z = chave < z->chave ? z->esq : z->dir; }
    if(z == arv->nulo) return;
    NoRN *y = z, *x; int cor_original = y->cor;
    if(z->esq == arv->nulo){ x = z->dir; rn_transplantar(arv, z, z->dir); }
    else if(z->dir == arv->nulo){ x = z->esq; rn_transplantar(arv, z, z->esq); }
    else {
        y = rn_minimo(arv, z->dir); cor_original = y->cor; x = y->dir;
        if(y->pai == z) x->pai = y;
        else { rn_transplantar(arv, y, y->dir); y->dir = z->dir; y->dir->pai = y; }
        rn_transplantar(arv, z, y); y->esq = z->esq; y->esq->pai = y; y->cor = z->cor;
    }
    free(z);
    if(cor_original == PRETO) rn_corrigir_remocao(arv, x);
}

static void rn_liberar_nos(ArvoreRN *arv, NoRN *no){
    if(no == arv->nulo) return;
    rn_liberar_nos(arv, no->esq);
    rn_liberar_nos(arv, no->dir);
    free(no);
}

static void rn_liberar(ArvoreRN *arv){ rn_liberar_nos(arv, arv->raiz); free(arv->nulo); free(arv); }

//Arvore B
#define B_MAX_CHAVES 22

typedef struct NoB {
    int num_chaves;
    int chaves[B_MAX_CHAVES + 1];
    int folha;
    struct NoB *filhos[B_MAX_CHAVES + 2];
} NoB;

typedef struct{ 
  NoB *raiz; 
  int grau_min; 
}ArvoreB;

static NoB *b_criar_no(int eh_folha){
    NoB *no = calloc(1, sizeof *no);
    no->folha = eh_folha;
    return no;
}

static ArvoreB *b_criar(int grau_min){
    ArvoreB *arv = malloc(sizeof *arv);
    arv->grau_min = grau_min;
    arv->raiz = b_criar_no(1);
    return arv;
}

//Divide o filho cheio na posicao i do no x
static void b_dividir_filho(ArvoreB *arv, NoB *x, int i){
    CONTAR();
    int t = arv->grau_min;
    NoB *filho = x->filhos[i];
    NoB *novo  = b_criar_no(filho->folha);
    novo->num_chaves = t - 1;
    //Copia a metade direita das chaves para o novo no
    for(int j = 0; j < t - 1; j++){ CONTAR(); novo->chaves[j] = filho->chaves[j + t]; }
    if(!filho->folha)
        for(int j = 0; j < t; j++){ CONTAR(); novo->filhos[j] = filho->filhos[j + t]; }
    filho->num_chaves = t - 1;
    //Aabre espaco em x para o novo filho
    for(int j = x->num_chaves; j >= i + 1; j--){ CONTAR(); x->filhos[j + 1] = x->filhos[j]; }
    x->filhos[i + 1] = novo;
    for(int j = x->num_chaves - 1; j >= i; j--){ CONTAR(); x->chaves[j + 1] = x->chaves[j]; }
    x->chaves[i] = filho->chaves[t - 1];
    x->num_chaves++;
}

//Insere em no nao cheio
static void b_inserir_no_nao_cheio(ArvoreB *arv, NoB *x, int chave){
    CONTAR();
    int i = x->num_chaves - 1;
    if(x->folha){
        while(i >= 0 && chave < x->chaves[i]){ CONTAR(); x->chaves[i + 1] = x->chaves[i]; i--; }
        x->chaves[i + 1] = chave;
        x->num_chaves++;
    } else {
        while(i >= 0 && chave < x->chaves[i]){ CONTAR(); i--; }
        i++;
        if(x->filhos[i]->num_chaves == 2 * arv->grau_min - 1){
            b_dividir_filho(arv, x, i);
            if(chave > x->chaves[i]) i++;
        }
        b_inserir_no_nao_cheio(arv, x->filhos[i], chave);
    }
}

static void b_inserir(ArvoreB *arv, int chave){ CONTAR();
    NoB *raiz = arv->raiz;
    if(raiz->num_chaves == 2 * arv->grau_min - 1){
        //Raiz cheia: cria nova raiz e divide
        NoB *nova_raiz = b_criar_no(0);
        arv->raiz = nova_raiz;
        nova_raiz->filhos[0] = raiz;
        b_dividir_filho(arv, nova_raiz, 0);
        b_inserir_no_nao_cheio(arv, nova_raiz, chave);
    } else {
        b_inserir_no_nao_cheio(arv, raiz, chave);
    }
}

//Retorna o predecessor (maior chave na subarvore esquerda)
static NoB *b_predecessor(NoB *x, int idx){
    NoB *cur = x->filhos[idx];
    while(!cur->folha) cur = cur->filhos[cur->num_chaves];
    return cur;
}

//Retorna o sucessor (menor chave na subarvore direita)
static NoB *b_sucessor(NoB *x, int idx){
    NoB *cur = x->filhos[idx + 1];
    while(!cur->folha) cur = cur->filhos[0];
    return cur;
}

//Funde filhos[idx] com filhos[idx+1] usando chaves[idx] como separador
static void b_fundir(ArvoreB *arv, NoB *x, int idx){
    CONTAR();
    int t = arv->grau_min;
    NoB *esq = x->filhos[idx], *dir = x->filhos[idx + 1];
    esq->chaves[t - 1] = x->chaves[idx];
    for(int i = 0; i < dir->num_chaves; i++){ CONTAR(); esq->chaves[t + i] = dir->chaves[i]; }
    if(!esq->folha)
        for(int i = 0; i <= dir->num_chaves; i++){ CONTAR(); esq->filhos[t + i] = dir->filhos[i]; }
    esq->num_chaves = 2 * t - 1;
    //Remove a chave separadora e o ponteiro para dir em x 
    for(int i = idx + 1; i < x->num_chaves; i++){ CONTAR(); x->chaves[i - 1] = x->chaves[i]; x->filhos[i] = x->filhos[i + 1]; }
    x->num_chaves--;
    free(dir);
}

//Garante que filhos[idx] tenha pelo menos t chaves antes de descer
static void b_completar_filho(ArvoreB *arv, NoB *x, int idx){
    int t = arv->grau_min;
    if(idx > 0 && x->filhos[idx - 1]->num_chaves >= t){
        CONTAR();
        NoB *filho = x->filhos[idx], *irm_esq = x->filhos[idx - 1];
        for(int i = filho->num_chaves - 1; i >= 0; i--){ CONTAR(); filho->chaves[i + 1] = filho->chaves[i]; }
        if(!filho->folha)
            for(int i = filho->num_chaves; i >= 0; i--){ CONTAR(); filho->filhos[i + 1] = filho->filhos[i]; }
        filho->chaves[0] = x->chaves[idx - 1];
        if(!filho->folha) filho->filhos[0] = irm_esq->filhos[irm_esq->num_chaves];
        x->chaves[idx - 1] = irm_esq->chaves[irm_esq->num_chaves - 1];
        filho->num_chaves++;
        irm_esq->num_chaves--;
    } else if(idx < x->num_chaves && x->filhos[idx + 1]->num_chaves >= t){
        
        CONTAR();
        NoB *filho = x->filhos[idx], *irm_dir = x->filhos[idx + 1];
        filho->chaves[filho->num_chaves] = x->chaves[idx];
        if(!filho->folha) filho->filhos[filho->num_chaves + 1] = irm_dir->filhos[0];
        x->chaves[idx] = irm_dir->chaves[0];
        for(int i = 1; i < irm_dir->num_chaves; i++){ CONTAR(); irm_dir->chaves[i - 1] = irm_dir->chaves[i]; }
        if(!irm_dir->folha)
            for(int i = 1; i <= irm_dir->num_chaves; i++){ CONTAR(); irm_dir->filhos[i - 1] = irm_dir->filhos[i]; }
        filho->num_chaves++;
        irm_dir->num_chaves--;
    } else {
        
        if(idx < x->num_chaves) b_fundir(arv, x, idx);
        else                     b_fundir(arv, x, idx - 1);
    }
}

static void b_remover_no(ArvoreB *arv, NoB *x, int chave){
    CONTAR();
    int t = arv->grau_min, i = 0;
    while(i < x->num_chaves && chave > x->chaves[i]){ CONTAR(); i++; }
    if(i < x->num_chaves && x->chaves[i] == chave){
        if(x->folha){
           
            for(int j = i + 1; j < x->num_chaves; j++){ CONTAR(); x->chaves[j - 1] = x->chaves[j]; }
            x->num_chaves--;
        } else if(x->filhos[i]->num_chaves >= t){
            
            NoB *pred = b_predecessor(x, i);
            x->chaves[i] = pred->chaves[pred->num_chaves - 1];
            b_remover_no(arv, x->filhos[i], pred->chaves[pred->num_chaves - 1]);
        } else if(x->filhos[i + 1]->num_chaves >= t){
          
            NoB *suc = b_sucessor(x, i);
            x->chaves[i] = suc->chaves[0];
            b_remover_no(arv, x->filhos[i + 1], suc->chaves[0]);
        } else {
            
            b_fundir(arv, x, i);
            b_remover_no(arv, x->filhos[i], chave);
        }
    } else if(!x->folha){
      
        int ultimo = (i == x->num_chaves);
        if(x->filhos[i]->num_chaves < t) b_completar_filho(arv, x, i);
        if(ultimo && i > x->num_chaves) b_remover_no(arv, x->filhos[i - 1], chave);
        else                             b_remover_no(arv, x->filhos[i], chave);
    }
}

static void b_remover(ArvoreB *arv, int chave){
    if(!arv->raiz || arv->raiz->num_chaves == 0) return;
    b_remover_no(arv, arv->raiz, chave);
   
    if(arv->raiz->num_chaves == 0){
        NoB *antiga_raiz = arv->raiz;
        arv->raiz = arv->raiz->folha ? NULL : arv->raiz->filhos[0];
        free(antiga_raiz);
    }
}

static void b_liberar_nos(NoB *no){
    if(!no) return;
    if(!no->folha)
        for(int i = 0; i <= no->num_chaves; i++) b_liberar_nos(no->filhos[i]);
    free(no);
}

static void b_liberar(ArvoreB *arv){ b_liberar_nos(arv->raiz); free(arv); }

//Mediçao das mediçoes
static int tamanhos[NUM_TAMANHOS] = {10, 50, 100, 200, 300, 500, 700, 1000,
                                     1500, 2000, 3000, 4000, 5000, 6000,
                                     7000, 8000, 9000, 10000};

int main(){
    printf("=== Analise de Complexidade: AVL | Rubro-Negra | Arvore B ===\n");
    printf("Amostras: %d | Tamanhos: %d a %d\n\n", NUM_AMOSTRAS, tamanhos[0], tamanhos[NUM_TAMANHOS - 1]);

    long long avl_ins[NUM_TAMANHOS]  = {}, avl_rem[NUM_TAMANHOS]  = {};
    long long rn_ins[NUM_TAMANHOS]   = {}, rn_rem[NUM_TAMANHOS]   = {};
    long long b2_ins[NUM_TAMANHOS]   = {}, b2_rem[NUM_TAMANHOS]   = {};
    long long b5_ins[NUM_TAMANHOS]   = {}, b5_rem[NUM_TAMANHOS]   = {};
    long long b10_ins[NUM_TAMANHOS]  = {}, b10_rem[NUM_TAMANHOS]  = {};

    int *chaves = malloc(MAX_ELEMENTOS * sizeof(int));

    for(int amostra = 0; amostra < NUM_AMOSTRAS; amostra++){
        srand(42 + amostra * 1337);
        for(int i = 0; i < MAX_ELEMENTOS; i++)
            chaves[i] = rand() % (MAX_ELEMENTOS * 10) + 1;

        for(int ti = 0; ti < NUM_TAMANHOS; ti++){
            int n = tamanhos[ti];

            //AVL
            NoAVL *avl = NULL;
            contador_ops = 0;
            for(int i = 0; i < n; i++) avl = avl_inserir(avl, chaves[i]);
            avl_ins[ti] += contador_ops;

            contador_ops = 0;
            for(int i = 0; i < n; i++) avl = avl_remover(avl, chaves[i]);
            avl_rem[ti] += contador_ops;
            avl_liberar(avl);

            //RUBRO-NEGRA
            ArvoreRN *rn = rn_criar();
            contador_ops = 0;
            for(int i = 0; i < n; i++) rn_inserir(rn, chaves[i]);
            rn_ins[ti] += contador_ops;

            contador_ops = 0;
            for(int i = 0; i < n; i++) rn_remover(rn, chaves[i]);
            rn_rem[ti] += contador_ops;
            rn_liberar(rn);

            //AVORE B (com grau min t=2)
            ArvoreB *b2 = b_criar(2);
            contador_ops = 0;
            for(int i = 0; i < n; i++) b_inserir(b2, chaves[i]);
            b2_ins[ti] += contador_ops;

            contador_ops = 0;
            for(int i = 0; i < n; i++) b_remover(b2, chaves[i]);
            b2_rem[ti] += contador_ops;
            b_liberar(b2);

            //AVORE B (com grau minimo t=5) 
            ArvoreB *b5 = b_criar(5);
            contador_ops = 0;
            for(int i = 0; i < n; i++) b_inserir(b5, chaves[i]);
            b5_ins[ti] += contador_ops;

            contador_ops = 0;
            for(int i = 0; i < n; i++) b_remover(b5, chaves[i]);
            b5_rem[ti] += contador_ops;
            b_liberar(b5);

            //AVORE B (com grau minimo t=10)
            ArvoreB *b10 = b_criar(10);
            contador_ops = 0;
            for(int i = 0; i < n; i++) b_inserir(b10, chaves[i]);
            b10_ins[ti] += contador_ops;

            contador_ops = 0;
            for(int i = 0; i < n; i++) b_remover(b10, chaves[i]);
            b10_rem[ti] += contador_ops;
            b_liberar(b10);
        }
        printf("  Amostra %d/%d concluida\n", amostra + 1, NUM_AMOSTRAS);
        fflush(stdout);
    }

    //Calcula a media das amostras
    for(int ti = 0; ti < NUM_TAMANHOS; ti++){
        avl_ins[ti]  /= NUM_AMOSTRAS; avl_rem[ti]  /= NUM_AMOSTRAS;
        rn_ins[ti]   /= NUM_AMOSTRAS; rn_rem[ti]   /= NUM_AMOSTRAS;
        b2_ins[ti]   /= NUM_AMOSTRAS; b2_rem[ti]   /= NUM_AMOSTRAS;
        b5_ins[ti]   /= NUM_AMOSTRAS; b5_rem[ti]   /= NUM_AMOSTRAS;
        b10_ins[ti]  /= NUM_AMOSTRAS; b10_rem[ti]  /= NUM_AMOSTRAS;
    }

    //Salva os resultados em arquivo .CSV 
    FILE *arq_ins = fopen("resultados_insercao.csv",  "w");
    FILE *arq_rem = fopen("resultados_remocao.csv",   "w");
    fprintf(arq_ins, "n,AVL,RN,B_t2,B_t5,B_t10\n");
    fprintf(arq_rem, "n,AVL,RN,B_t2,B_t5,B_t10\n");

    printf("\n%-7s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s\n",
           "n", "AVL_ins", "AVL_rem", "RN_ins", "RN_rem",
           "B2_ins", "B2_rem", "B5_ins", "B5_rem", "B10_ins", "B10_rem");
    printf("%-7s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s\n",
           "-------","------------","------------","------------","------------",
           "------------","------------","------------","------------","------------","------------");

    for(int ti = 0; ti < NUM_TAMANHOS; ti++){
        int n = tamanhos[ti];
        printf("%-7d %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld %12lld\n",
               n,
               avl_ins[ti], avl_rem[ti],
               rn_ins[ti],  rn_rem[ti],
               b2_ins[ti],  b2_rem[ti],
               b5_ins[ti],  b5_rem[ti],
               b10_ins[ti], b10_rem[ti]);
        fprintf(arq_ins, "%d,%lld,%lld,%lld,%lld,%lld\n",
                n, avl_ins[ti], rn_ins[ti], b2_ins[ti], b5_ins[ti], b10_ins[ti]);
        fprintf(arq_rem, "%d,%lld,%lld,%lld,%lld,%lld\n",
                n, avl_rem[ti], rn_rem[ti], b2_rem[ti], b5_rem[ti], b10_rem[ti]);
    }
    fclose(arq_ins);
    fclose(arq_rem);
    printf("\nArquivos CSV gerados: resultados_insercao.csv e resultados_remocao.csv\n");
    free(chaves);
    return 0;
}
