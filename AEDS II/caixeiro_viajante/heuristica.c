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
    // Linha i da matriz: valores para colunas i, i+1, i+2, ..., n-1
    int linha_matriz = 0;
    int coluna_matriz = 0;
    int total_valores = (dimensao * (dimensao + 1)) / 2;
    int valores_lidos = 0;
    
    while (fgets(linha, sizeof(linha), arquivo) && valores_lidos < total_valores) {
        // Ignora linhas vazias
        if (strlen(linha) < 2) continue;
        
        // Verifica se a linha começa com número ou espaço seguido de número
        int primeiro_char = 0;
        while (linha[primeiro_char] == ' ' || linha[primeiro_char] == '\t') primeiro_char++;
        if (linha[primeiro_char] < '0' || linha[primeiro_char] > '9') {
            // Pode ser coordenadas no final
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
                // Se passou do fim da linha atual, vai para próxima linha
                if (coluna_matriz >= dimensao) {
                    linha_matriz++;
                    coluna_matriz = linha_matriz;
                }
            }
            
            if (linha_matriz < dimensao && coluna_matriz < dimensao) {
                grafo[linha_matriz][coluna_matriz] = valor;
                grafo[coluna_matriz][linha_matriz] = valor; // Matriz simétrica
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
    // Formato: linha i contém valores para colunas 0, 1, 2, ..., i
    int linha_atual = 0;
    int valores_lidos = 0;
    int total_valores = (dimensao * (dimensao + 1)) / 2; // Total de valores na matriz triangular
    
    while (fgets(linha, sizeof(linha), arquivo) && valores_lidos < total_valores) {
        // Ignora linhas vazias
        if (strlen(linha) < 2) continue;
        
        // Verifica se a linha começa com número ou espaço seguido de número
        int primeiro_char = 0;
        while (linha[primeiro_char] == ' ' || linha[primeiro_char] == '\t') primeiro_char++;
        if (linha[primeiro_char] < '0' || linha[primeiro_char] > '9') {
            if (valores_lidos >= total_valores - dimensao) break; // Já leu tudo
            continue;
        }
        
        char *token = strtok(linha, " \t\n");
        int coluna_atual = 0;
        
        while (token != NULL && coluna_atual <= linha_atual && linha_atual < dimensao) {
            int valor = atoi(token);
            
            grafo[linha_atual][coluna_atual] = valor;
            grafo[coluna_atual][linha_atual] = valor; // Matriz simétrica
            
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
    // Adiciona o custo de voltar ao vértice inicial
    custo += grafo[caminho[n - 1]][caminho[0]];
    return custo;
}

// Heurística: Vizinho Mais Próximo
int nearest_neighbor(int caminho[]) {
    bool visitado[MAX_VERTICES] = {false};
    caminho[0] = 0; // Começa do vértice 0
    visitado[0] = true;
    
    for (int i = 1; i < num_vertices; i++) {
        int ultimo = caminho[i - 1];
        int mais_proximo = -1;
        int menor_distancia = INT_MAX;
        
        // Encontra o vértice não visitado mais próximo
        for (int j = 0; j < num_vertices; j++) {
            if (!visitado[j] && grafo[ultimo][j] < menor_distancia) {
                menor_distancia = grafo[ultimo][j];
                mais_proximo = j;
            }
        }
        
        caminho[i] = mais_proximo;
        visitado[mais_proximo] = true;
    }
    
    return calcular_custo(caminho, num_vertices);
}

// Heurística 2-opt: Melhora a solução trocando arestas
int dois_opt(int caminho[], int custo_inicial) {
    int melhor_custo = custo_inicial;
    bool melhorou = true;
    int iteracoes = 0;
    int max_iteracoes = 100; // Limita iterações para não demorar muito
    
    while (melhorou && iteracoes < max_iteracoes) {
        melhorou = false;
        iteracoes++;
        
        for (int i = 0; i < num_vertices - 2; i++) {
            for (int j = i + 2; j < num_vertices; j++) {
                // Calcula a diferença de custo ao inverter o segmento entre i e j
                int custo_antigo = grafo[caminho[i]][caminho[i + 1]] + 
                                   grafo[caminho[j]][caminho[(j + 1) % num_vertices]];
                int custo_novo = grafo[caminho[i]][caminho[j]] + 
                                grafo[caminho[i + 1]][caminho[(j + 1) % num_vertices]];
                
                if (custo_novo < custo_antigo) {
                    // Inverte o segmento entre i+1 e j
                    int inicio = i + 1;
                    int fim = j;
                    while (inicio < fim) {
                        int temp = caminho[inicio];
                        caminho[inicio] = caminho[fim];
                        caminho[fim] = temp;
                        inicio++;
                        fim--;
                    }
                    
                    melhor_custo = calcular_custo(caminho, num_vertices);
                    melhorou = true;
                    break; // Reinicia a busca
                }
            }
            if (melhorou) break;
        }
    }
    
    return melhor_custo;
}

// Função principal para resolver TSP com heurística
int resolver_tsp_heuristica(int caminho[]) {
    // Passo 1: Nearest Neighbor
    int custo = nearest_neighbor(caminho);
    
    // Passo 2: Melhora com 2-opt
    custo = dois_opt(caminho, custo);
    
    return custo;
}

int main() {
    char *arquivos[] = {"si535.txt", "pa561.txt", "si1032.txt"};
    int dimensoes[] = {535, 561, 1032};
    int num_arquivos = 3;
    
    printf("=== Problema do Caixeiro Viajante - Heuristica ===\n\n");
    printf("Usando: Nearest Neighbor + 2-opt\n\n");
    printf("%-15s %-15s %-20s %-15s\n", "Arquivo", "Vertices", "Custo", "Tempo (s)");
    printf("------------------------------------------------------------\n");
    
    for (int a = 0; a < num_arquivos; a++) {
        char *nome_arquivo = arquivos[a];
        int dimensao = dimensoes[a];
                
        // Determina o formato do arquivo
        int sucesso = 0;
        if (nome_arquivo[0] == 's' && nome_arquivo[1] == 'i') {
            // Arquivo "si" - formato UPPER_DIAG_ROW
            sucesso = ler_arquivo_upper_diag(nome_arquivo, dimensao);
        } else if (nome_arquivo[0] == 'p' && nome_arquivo[1] == 'a') {
            // Arquivo "pa" - formato LOWER_DIAG_ROW
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
        int custo = resolver_tsp_heuristica(caminho);
        clock_t fim = clock();
        
        double tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        
        printf("%-15s %-15d %-20d %-15.6f\n", nome_arquivo, num_vertices, custo, tempo_execucao);
    }
    
    printf("\n------------------------------------------------------------\n");
    printf("Processamento concluido!\n");
    
    return 0;
}
