#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

#define MAX_VERTICES 2000

// Estrutura para armazenar o grafo (matriz de adjacência)
int grafo[MAX_VERTICES][MAX_VERTICES];
int num_vertices;

// Função para ler arquivo com formato UPPER_DIAG_ROW (diagonal superior)
int ler_arquivo_upper_diag(const char *nome_arquivo, int dimensao) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir arquivo %s\n", nome_arquivo);
        return 0;
    }
    
    char linha[10000];
    
    // Pula o cabeçalho até encontrar EDGE_WEIGHT_SECTION
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strstr(linha, "EDGE_WEIGHT_SECTION")) {
            break;
        }
    }
    
    num_vertices = dimensao;
    
    // Inicializa a matriz com zeros
    for (int i = 0; i < dimensao; i++) {
        for (int j = 0; j < dimensao; j++) {
            grafo[i][j] = 0;
        }
    }
    
    // Lê os valores da diagonal superior
    int linha_matriz = 0;
    int coluna_matriz = 0;
    int total_valores = (dimensao * (dimensao + 1)) / 2;
    int valores_lidos = 0;
    
    while (fgets(linha, sizeof(linha), arquivo) && valores_lidos < total_valores) {
        if (strlen(linha) < 2) continue;
        
        int primeiro_char = 0;
        while (linha[primeiro_char] == ' ' || linha[primeiro_char] == '\t') primeiro_char++;
        if (linha[primeiro_char] < '0' || linha[primeiro_char] > '9') {
            if (valores_lidos >= total_valores - 100) break;
            continue;
        }
        
        char *token = strtok(linha, " \t\n");
        
        while (token != NULL && valores_lidos < total_valores) {
            int valor = atoi(token);
            
            if (valores_lidos == 0) {
                linha_matriz = 0;
                coluna_matriz = 0;
            } else {
                coluna_matriz++;
                if (coluna_matriz >= dimensao) {
                    linha_matriz++;
                    coluna_matriz = linha_matriz;
                }
            }
            
            if (linha_matriz < dimensao && coluna_matriz < dimensao) {
                grafo[linha_matriz][coluna_matriz] = valor;
                grafo[coluna_matriz][linha_matriz] = valor;
            }
            
            valores_lidos++;
            token = strtok(NULL, " \t\n");
        }
    }
    
    fclose(arquivo);
    return 1;
}

// Função para ler arquivo com formato LOWER_DIAG_ROW (diagonal inferior)
int ler_arquivo_lower_diag(const char *nome_arquivo, int dimensao) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir arquivo %s\n", nome_arquivo);
        return 0;
    }
    
    char linha[10000];
    
    // Pula o cabeçalho até encontrar EDGE_WEIGHT_SECTION
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strstr(linha, "EDGE_WEIGHT_SECTION")) {
            break;
        }
    }
    
    num_vertices = dimensao;
    
    // Inicializa a matriz com zeros
    for (int i = 0; i < dimensao; i++) {
        for (int j = 0; j < dimensao; j++) {
            grafo[i][j] = 0;
        }
    }
    
    // Lê os valores da diagonal inferior
    int linha_atual = 0;
    int valores_lidos = 0;
    int total_valores = (dimensao * (dimensao + 1)) / 2;
    
    while (fgets(linha, sizeof(linha), arquivo) && valores_lidos < total_valores) {
        if (strlen(linha) < 2) continue;
        
        int primeiro_char = 0;
        while (linha[primeiro_char] == ' ' || linha[primeiro_char] == '\t') primeiro_char++;
        if (linha[primeiro_char] < '0' || linha[primeiro_char] > '9') {
            if (valores_lidos >= total_valores - dimensao) break;
            continue;
        }
        
        char *token = strtok(linha, " \t\n");
        int coluna_atual = 0;
        
        while (token != NULL && coluna_atual <= linha_atual && linha_atual < dimensao) {
            int valor = atoi(token);
            
            grafo[linha_atual][coluna_atual] = valor;
            grafo[coluna_atual][linha_atual] = valor;
            
            coluna_atual++;
            valores_lidos++;
            token = strtok(NULL, " \t\n");
        }
        
        linha_atual++;
    }
    
    fclose(arquivo);
    return 1;
}

// Função para calcular o custo de um caminho
int calcular_custo(int caminho[], int n) {
    int custo = 0;
    for (int i = 0; i < n - 1; i++) {
        custo += grafo[caminho[i]][caminho[i + 1]];
    }
    custo += grafo[caminho[n - 1]][caminho[0]];
    return custo;
}

// Estrutura para representar a MST
typedef struct {
    int pai[MAX_VERTICES];
    bool na_mst[MAX_VERTICES];
} MST;

// Algoritmo de Prim para construir a MST
void construir_mst(MST *mst) {
    int chave[MAX_VERTICES]; // Menor peso da aresta que conecta ao vértice
    
    // Inicializa
    for (int i = 0; i < num_vertices; i++) {
        chave[i] = INT_MAX;
        mst->na_mst[i] = false;
        mst->pai[i] = -1;
    }
    
    // Começa do vértice 0
    chave[0] = 0;
    mst->pai[0] = -1;
    
    // Constrói a MST
    for (int count = 0; count < num_vertices - 1; count++) {
        // Encontra o vértice com menor chave que ainda não está na MST
        int u = -1;
        int menor_chave = INT_MAX;
        
        for (int v = 0; v < num_vertices; v++) {
            if (!mst->na_mst[v] && chave[v] < menor_chave) {
                menor_chave = chave[v];
                u = v;
            }
        }
        
        // Se não encontrou vértice, pode ser que o grafo não seja conexo
        if (u == -1) {
            break;
        }
        
        mst->na_mst[u] = true;
        
        // Atualiza as chaves dos vértices adjacentes
        for (int v = 0; v < num_vertices; v++) {
            if (u != v && grafo[u][v] > 0 && !mst->na_mst[v] && grafo[u][v] < chave[v]) {
                mst->pai[v] = u;
                chave[v] = grafo[u][v];
            }
        }
    }
}

// Lista de adjacência da MST para o passeio pré-ordem
typedef struct {
    int lista[MAX_VERTICES][MAX_VERTICES];
    int tamanho[MAX_VERTICES];
} ListaAdjacencia;

// Constrói lista de adjacência da MST
void construir_lista_adjacencia(MST *mst, ListaAdjacencia *lista) {
    for (int i = 0; i < num_vertices; i++) {
        lista->tamanho[i] = 0;
    }
    
    // Adiciona arestas da MST (exceto a raiz)
    for (int i = 1; i < num_vertices; i++) {
        int pai = mst->pai[i];
        if (pai != -1) {
            lista->lista[pai][lista->tamanho[pai]++] = i;
            lista->lista[i][lista->tamanho[i]++] = pai;
        }
    }
}

// Passeio pré-ordem (DFS) da MST
void preorder_traversal(int vertice, ListaAdjacencia *lista, bool visitado[], 
                        int caminho_pre[], int *indice) {
    visitado[vertice] = true;
    caminho_pre[(*indice)++] = vertice;
    
    // Visita filhos
    for (int i = 0; i < lista->tamanho[vertice]; i++) {
        int filho = lista->lista[vertice][i];
        if (!visitado[filho]) {
            preorder_traversal(filho, lista, visitado, caminho_pre, indice);
        }
    }
}

// Variáveis estáticas para evitar stack overflow
static MST mst_static;
static ListaAdjacencia lista_static;
static bool visitado_static[MAX_VERTICES];
static int caminho_pre_static[MAX_VERTICES];
static bool usado_static[MAX_VERTICES];

// Algoritmo aproximado baseado em MST
int tsp_mst(int caminho[]) {
    // Passo 1: Construir MST
    construir_mst(&mst_static);
    
    // Passo 2: Construir lista de adjacência da MST
    construir_lista_adjacencia(&mst_static, &lista_static);
    
    // Passo 3: Fazer passeio pré-ordem começando do vértice 0
    for (int i = 0; i < num_vertices; i++) {
        visitado_static[i] = false;
    }
    int indice = 0;
    
    preorder_traversal(0, &lista_static, visitado_static, caminho_pre_static, &indice);
    
    // Passo 4: Remover duplicatas (já não há duplicatas no passeio pré-ordem, mas garantimos)
    for (int i = 0; i < num_vertices; i++) {
        usado_static[i] = false;
    }
    int pos = 0;
    
    for (int i = 0; i < indice; i++) {
        if (!usado_static[caminho_pre_static[i]]) {
            caminho[pos++] = caminho_pre_static[i];
            usado_static[caminho_pre_static[i]] = true;
        }
    }
    
    // Garante que todos os vértices estão no caminho
    for (int i = 0; i < num_vertices; i++) {
        if (!usado_static[i]) {
            caminho[pos++] = i;
        }
    }
    
    return calcular_custo(caminho, num_vertices);
}

int main() {
    printf("Iniciando programa...\n");
    fflush(stdout);
    
    char *arquivos[] = {"si535.txt", "pa561.txt", "si1032.txt"};
    int dimensoes[] = {535, 561, 1032};
    int num_arquivos = 3;
    
    printf("=== Problema do Caixeiro Viajante - Algoritmo Aproximado (MST) ===\n\n");
    printf("Usando: MST + Passeio Pre-ordem\n\n");
    printf("%-15s %-15s %-20s %-15s\n", "Arquivo", "Vertices", "Custo", "Tempo (s)");
    printf("------------------------------------------------------------\n");
    fflush(stdout);
    
    for (int a = 0; a < num_arquivos; a++) {
        char *nome_arquivo = arquivos[a];
        int dimensao = dimensoes[a];
        
        // Determina o formato do arquivo
        int sucesso = 0;
        if (nome_arquivo[0] == 's' && nome_arquivo[1] == 'i') {
            sucesso = ler_arquivo_upper_diag(nome_arquivo, dimensao);
        } else if (nome_arquivo[0] == 'p' && nome_arquivo[1] == 'a') {
            sucesso = ler_arquivo_lower_diag(nome_arquivo, dimensao);
        } else {
            printf("Erro: formato de arquivo desconhecido para %s\n", nome_arquivo);
            continue;
        }
        
        if (!sucesso) {
            printf("Erro ao ler arquivo %s\n", nome_arquivo);
            continue;
        }
        
        int caminho[MAX_VERTICES];
        
        // Mede o tempo de execução
        clock_t inicio = clock();
        int custo = tsp_mst(caminho);
        clock_t fim = clock();
        
        double tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        
        printf("%-15s %-15d %-20d %-15.6f\n", nome_arquivo, num_vertices, custo, tempo_execucao);
    }
    
    printf("\n------------------------------------------------------------\n");
    printf("Processamento concluido!\n");
    
    return 0;
}

