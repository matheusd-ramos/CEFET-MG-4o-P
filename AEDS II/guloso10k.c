#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h> // Para DBL_MAX
#include <stdbool.h>
#include <time.h>


long long int comparacoes = 0;

// Estrutura para representar uma aresta
typedef struct Aresta {
    int destino;
    double peso;
    struct Aresta *proxima;
} Aresta;

// Estrutura para lista de adjacência
typedef struct {
    Aresta *primeira;
    int tamanho;
} ListaAdj;

// Estrutura para heap binária (min-heap)
typedef struct {
    int *vertices;  // Array de índices de vértices
    double *distancias;
    int tamanho;
    int capacidade;
} Heap;

// Cria uma heap vazia
Heap* criarHeap(int capacidade) {
    Heap *h = malloc(sizeof(Heap));
    if (!h) return NULL;
    h->vertices = malloc(capacidade * sizeof(int));
    h->distancias = malloc(capacidade * sizeof(double));
    if (!h->vertices || !h->distancias) {
        free(h->vertices);
        free(h->distancias);
        free(h);
        return NULL;
    }
    h->tamanho = 0;
    h->capacidade = capacidade;
    return h;
}

// Libera a heap
void liberarHeap(Heap *h) {
    if (h) {
        free(h->vertices);
        free(h->distancias);
        free(h);
    }
}

// Troca dois elementos na heap
void trocarHeap(Heap *h, int i, int j) {
    int tempV = h->vertices[i];
    double tempD = h->distancias[i];
    h->vertices[i] = h->vertices[j];
    h->distancias[i] = h->distancias[j];
    h->vertices[j] = tempV;
    h->distancias[j] = tempD;
}

// Sobe um elemento na heap (heapify up)
void subirHeap(Heap *h, int indice) {
    while (indice > 0) {
        int pai = (indice - 1) / 2;
        comparacoes++; // Contando comparação do heap
        if (h->distancias[indice] >= h->distancias[pai]) break;
        trocarHeap(h, indice, pai);
        indice = pai;
    }
}

// Desce um elemento na heap (heapify down)
void descerHeap(Heap *h, int indice) {
    while (1) {
        int menor = indice;
        int esq = 2 * indice + 1;
        int dir = 2 * indice + 2;

        comparacoes++; // Contando comparação do heap
        if (esq < h->tamanho && h->distancias[esq] < h->distancias[menor]) {
            menor = esq;
        }
        comparacoes++; // Contando comparação do heap
        if (dir < h->tamanho && h->distancias[dir] < h->distancias[menor]) {
            menor = dir;
        }

        if (menor == indice) break;
        trocarHeap(h, indice, menor);
        indice = menor;
    }
}

// Insere um elemento na heap
void inserirHeap(Heap *h, int vertice, double distancia) {
    if (h->tamanho >= h->capacidade) return;
    
    h->vertices[h->tamanho] = vertice;
    h->distancias[h->tamanho] = distancia;
    h->tamanho++;
    subirHeap(h, h->tamanho - 1);
}

// Remove e retorna o elemento mínimo (raiz)
int removerMinHeap(Heap *h) {
    if (h->tamanho == 0) return -1;
    
    int min = h->vertices[0];
    h->tamanho--;
    
    if (h->tamanho > 0) {
        h->vertices[0] = h->vertices[h->tamanho];
        h->distancias[0] = h->distancias[h->tamanho];
        descerHeap(h, 0);
    }
    
    return min;
}

// Verifica se a heap está vazia
bool heapVazia(Heap *h) {
    return h->tamanho == 0;
}

// Adiciona aresta à lista de adjacência
void adicionarAresta(ListaAdj *lista, int destino, double peso) {
    Aresta *nova = malloc(sizeof(Aresta));
    if (!nova) {
        printf("Erro ao alocar memoria para aresta.\n");
        exit(1);
    }
    nova->destino = destino;
    nova->peso = peso;
    nova->proxima = lista->primeira;
    lista->primeira = nova;
    lista->tamanho++;
}

// Libera memória da lista de adjacência
void liberarGrafo(ListaAdj *grafo, int n) {
    for (int i = 0; i < n; i++) {
        Aresta *atual = grafo[i].primeira;
        while (atual != NULL) {
            Aresta *temp = atual;
            atual = atual->proxima;
            free(temp);
        }
    }
    free(grafo);
}

// Prim MST com Heap e Lista de Adjacência
void primMST_heap(ListaAdj *grafo, int n) {
    // 'chave' armazena o menor peso de aresta para conectar o vértice à MST
    double *chave = malloc(n * sizeof(double));
    // 'incluido' marca se o vértice já está na MST
    bool *incluido = malloc(n * sizeof(bool));
    // 'pai' armazena a MST resultante
    int *pai = malloc(n * sizeof(int));

    if (!chave || !incluido || !pai) {
        printf("Erro ao alocar memoria para Prim.\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        chave[i] = DBL_MAX;
        incluido[i] = false;
        pai[i] = -1;
    }

    // Criar heap e inserir vértice de origem (vértice 0)
    Heap *heap = criarHeap(n);
    if (!heap) {
        printf("Erro ao criar heap para Prim.\n");
        free(chave);
        free(incluido);
        free(pai);
        exit(1);
    }
    
    chave[0] = 0.0;
    inserirHeap(heap, 0, 0.0);
    // pai[0] já é -1 (raiz)

    while (!heapVazia(heap)) {
        int u = removerMinHeap(heap);
        
        // Se já foi incluído, ignora (entrada antiga no heap)
        if (incluido[u]) continue;
        
        incluido[u] = true;

        // Itera sobre os vizinhos de 'u'
        Aresta *atual = grafo[u].primeira;
        while (atual != NULL) {
            int v = atual->destino;
            double peso = atual->peso;

            comparacoes++;
            if (!incluido[v] && peso < chave[v]) {
                chave[v] = peso;
                pai[v] = u;
                inserirHeap(heap, v, chave[v]);
            }
            atual = atual->proxima;
        }
    }

    // Libera memória
    liberarHeap(heap);
    free(chave);
    free(incluido);
    free(pai);
}


int main() {

    FILE *entrada = fopen("instancia_10k.txt", "r");
    if (!entrada) {
        printf("Erro ao abrir arquivo de instancia.\n");
        return 1;
    }

    int n = 0, m = 0;
    if (fscanf(entrada, "%d", &n) != 1) {
        printf("Erro ao ler numero de vertices.\n");
        fclose(entrada);
        return 1;
    }
    if (fscanf(entrada, "%d", &m) != 1) {
        printf("Erro ao ler numero de arestas.\n");
        fclose(entrada);
        return 1;
    }

    // Criar lista de adjacência
    ListaAdj *grafo = calloc(n, sizeof(ListaAdj));
    if (!grafo) {
        printf("Erro ao alocar memoria para o grafo.\n");
        fclose(entrada);
        return 1;
    }

    // Inicializar listas
    for (int i = 0; i < n; i++) {
        grafo[i].primeira = NULL;
        grafo[i].tamanho = 0;
    }

    // Ler arestas do arquivo
    for (int e = 0; e < m; e++) {
        int u, v;
        double w;
        if (fscanf(entrada, "%d %d %lf", &u, &v, &w) != 3) {
            printf("Erro ao ler aresta %d.\n", e + 1);
            break;
        }
        
        if (u >= 0 && u < n && v >= 0 && v < n) {
            adicionarAresta(&grafo[u], v, w);
            adicionarAresta(&grafo[v], u, w); // Adiciona a aresta de volta
        }
    }

    fclose(entrada);

    comparacoes = 0;
    clock_t inicio = clock();

    primMST_heap(grafo, n);

    double tempo = (double) (clock() - inicio)/CLOCKS_PER_SEC ;

    printf("Vertices: %d | Comparacoes: %lld | Tempo: %lf segundos\n", n, comparacoes, tempo);

    liberarGrafo(grafo, n);

    return 0;
}