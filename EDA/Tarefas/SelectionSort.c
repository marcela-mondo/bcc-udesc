#include <stdio.h>
#define TAMANHO 10

void bolha (int*  , const int );
void troca (int *elementolPtr, int *elemento2Ptr);
void SelectionSort (int*, const int);

int main (){
  int i, a[TAMANHO] = { 2, 6, 4, 8, 10, 12, 89, 68, 45, 37 };

  printf ("Itens de dados na ordem original\n");
  for (i = 0; i <= TAMANHO - 1; i++)
  printf ("%4d", a[i]);

  bolha (a, TAMANHO);
  printf ("\nltens de dados em ordem ascendente\n");
  for (i = 0; i <= TAMANHO - 1; i++)
  printf ("%4d", a[i]);

  SelectionSort(a, TAMANHO);
  
  printf ("\n");
  return 0;
} 

void bolha (int *array,  const int tamanho){
  int pass, j;

  for (pass = 1; pass <= tamanho - 1; pass++)
  for (j = 0; j <= tamanho - 2; j++)
    if (array[j] > array[j + 1])
    troca (&array[j], &array[j + 1]);
}

void troca (int *elementolPtr, int *elemento2Ptr)
{
  int temp;
  temp = *elementolPtr;
  *elementolPtr = *elemento2Ptr;
  *elemento2Ptr = temp;
}

void SelectionSort (int *array, const int tamanho){
  int i, Imenor = 0, trocar = 0;;

  for(i=0; i<tamanho; i++){
    Imenor = i;
    for(int j = i+1; j<tamanho; j++){
      if(array[j] < array[Imenor]){
        Imenor = j;
      }
    }

    if(Imenor != 1){
        trocar = array[i];
        array[i] = array[Imenor];
        array[Imenor] = trocar;
    }
  }
  
  printf("\nSelection Sort\n");
  for(i=0;i<tamanho;i++){
    printf("%4d", array[i]);
    
  }
}