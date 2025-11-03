#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Cada nó armazena três informações:
   nesse caso um número (num),
   ponteiro para subárvore à direita (sad)
   e ponteiro para subárvore à esquerda (sae).*/
typedef struct tree
{
  int num;
  struct tree* sad;
  struct tree* sae;
} Tree;

/* A estrutura da árvore é representada por um ponteiro
   para o nó raiz. Com esse ponteiro, temos acesso aos
   demais nós. */

/* Função que cria uma árvore */
Tree* createTree()
{
  /* Uma árvore é representada pelo endereço do nó raiz,
     essa função cria uma árvore com nenhum elemento,
     ou seja, cria uma árvore vazia, por isso retorna NULL. */
  return NULL;
}

/* Função que verifica se uma árvore é vazia */
int treeIsEmpty(Tree* t)
{
  /* Retorna 1 se a árvore for vazia e 0 caso contrário */
  return t == NULL;

}

/* Função que mostra a informação da árvore */
void showTree(Tree* t)
{
  /* Essa função imprime os elementos de forma recursiva */
  
  printf("<"); /* notação para organizar na hora de mostrar os elementos */
  if(!treeIsEmpty(t)) /* se a árvore não for vazia... */
  {
    /* Mostra os elementos em pré-ordem */
    printf("%d ", t->num); /* mostra a raiz */
    showTree(t->sae); /* mostra a sae (subárvore à esquerda) */
    showTree(t->sad); /* mostra a sad (subárvore à direita) */
  }
  printf(">"); /* notação para organizar na hora de mostrar os elementos */
}

/* Função que insere um dado na árvore */
void insertTree(Tree** t, int num)
{
  /* Essa função insere os elementos de forma recursiva */
  if(*t == NULL)
  {
    *t = (Tree*)malloc(sizeof(Tree)); /* Aloca memória para a estrutura */
    (*t)->sae = NULL; /* Subárvore à esquerda é NULL */
    (*t)->sad = NULL; /* Subárvore à direita é NULL */
    (*t)->num = num; /* Armazena a informação */
  } else {
    if(num < (*t)->num) /* Se o número for menor então vai pra esquerda */
    {
      /* Percorre pela subárvore à esquerda */
      insertTree(&(*t)->sae, num);
    }
    if(num > (*t)->num) /* Se o número for maior então vai pra direita */
    {
      /* Percorre pela subárvore à direita */
      insertTree(&(*t)->sad, num);
    }
  }
}

void print_tree(struct tree *node, int level) {
    if (node == NULL) return;
    print_tree(node->sae, level + 1);
    for (int i = 0; i < level; i++) printf("    ");
    printf("%d\n", node->num);
    print_tree(node->sad, level + 1);
}

/* Função que verifica se um elemento pertence ou não à árvore */
int isInTree(Tree* t, int num, int *n) {
  if(n!=NULL)
    (*n)++;
  
  if(treeIsEmpty(t)) { /* Se a árvore estiver vazia, então retorna 0 */
    return 0;
  }
  if (num==t->num)
    return 1;
  
  if(num<t->num)
    return isInTree(t->sae,num,n);
  
  return isInTree(t->sad,num,n);
}

void teste1(){
  Tree * t[10];
  //inicializando com null cada arvore.
  for(int i = 0; i < 10; i++){
    t[i] = NULL;
  }
  printf("TESTE 1:\n");
  //percorrendo todas as árvores de 0 a 9;
    for(int i=1;i<=10;i++){
      //inserindo os elementos ordenados na árvore
      for(int j = 1; j <= i*10000; j++ ){
        insertTree(&t[i - 1],j);
      }
      
     //contando número de comparações
      int ncomp = 0;
      isInTree(t[i - 1],100001,&ncomp);
      fprintf(stdin, "n = %d elementos - Numero de comparações: %d\n", i*10000, ncomp);
    }
}

void teste2(){
  Tree * t[10];
  //inicializando com null cada arvore.
  for(int i = 0; i < 10; i++){
    t[i] = NULL;
  }
  printf("TESTE 2:\n");
    //percorrendo todas as árvores de 0 a 9;
    for(int i=1;i<=10;i++){
      //inserindo os elementos aleatórios na árvore
      for(int j = 1; j <= i*10000; j++){
        int aux = rand() % 1000000 + 1;
        insertTree(&t[i - 1],aux);
      }
      //contando número de comparações
      int ncomp = 0;
      isInTree(t[i - 1],100001,&ncomp);
      fprintf(stdin, "n = %d elementos - Numero de comparações: %d\n", i*10000, ncomp);
    }
  }


int main()
{
  srand(time(NULL));
  
  teste1();
  teste2();
  
  return 0;
}

