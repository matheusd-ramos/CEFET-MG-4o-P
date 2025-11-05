#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
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
    double *distancias;  // Distâncias correspondentes
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
        comparacoes++;
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

        comparacoes++;
        if (esq < h->tamanho && h->distancias[esq] < h->distancias[menor]) {
            menor = esq;
        }
        comparacoes++;
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

// Dijkstra otimizado com heap binária
void dijkstra(ListaAdj *grafo, int n, int origem) {
    double *dist = malloc(n * sizeof(double));
    bool *visitado = malloc(n * sizeof(bool));

    if (!dist || !visitado) {
        printf("Erro ao alocar memoria para distancias ou visitados.\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        dist[i] = DBL_MAX;
        visitado[i] = false;
    }
    dist[origem] = 0;

    // Criar heap e inserir vértice de origem
    Heap *heap = criarHeap(n);
    if (!heap) {
        printf("Erro ao criar heap.\n");
        free(dist);
        free(visitado);
        exit(1);
    }
    inserirHeap(heap, origem, 0.0);

    while (!heapVazia(heap)) {
        int u = removerMinHeap(heap);
        
        // Se já foi visitado, ignora (pode ter sido atualizado múltiplas vezes)
        if (visitado[u]) continue;
        
        visitado[u] = true;

        // Itera apenas sobre as arestas existentes do vértice u
        Aresta *atual = grafo[u].primeira;
        while (atual != NULL) {
            int v = atual->destino;
            comparacoes++;
            if (!visitado[v] && dist[u] != DBL_MAX
                && dist[u] + atual->peso < dist[v]) {
                dist[v] = dist[u] + atual->peso;
                // Insere na heap (pode ter duplicatas, mas a menor distância será processada primeiro)
                inserirHeap(heap, v, dist[v]);
            }
            atual = atual->proxima;
        }
    }

    liberarHeap(heap);
    free(dist);
    free(visitado);
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

int main() {

    FILE *entrada = fopen("instancia_1kk.txt", "r");
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

    // Criar lista de adjacência (muito mais eficiente em memória)
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
        // Arestas direcionadas: u -> v com peso w
        if (u >= 0 && u < n && v >= 0 && v < n) {
            adicionarAresta(&grafo[u], v, w);
        }
    }

    fclose(entrada);

    comparacoes = 0;

    clock_t inicio = clock();

    dijkstra(grafo, n, 0);

    double tempo = (double) (clock() - inicio)/CLOCKS_PER_SEC ;


    // Printa na tela
    printf("Vertices: %d | Comparacoes: %lld\n | Tempo: %.2lf segundos\n", n, comparacoes, tempo);

    // Libera memória
    liberarGrafo(grafo, n);

    return 0;
}