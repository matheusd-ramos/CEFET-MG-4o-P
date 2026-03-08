#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_VERTICES 20
#define MAX_EXECUTION_TIME 1800  // 30 minutos em segundos

// Estrutura para armazenar o grafo (matriz de adjacência)
int grafo[MAX_VERTICES][MAX_VERTICES];
int num_vertices;

// Função para gerar um grafo completo aleatório
void gerar_grafo_aleatorio(int n) {
    num_vertices = n;
    srand(time(NULL) + n); // Seed diferente para cada instância
    
    // Gera pesos aleatórios entre 1 e 100
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                grafo[i][j] = 0; // Distância de um vértice para ele mesmo é 0
            } else {
                grafo[i][j] = (rand() % 100) + 1; // Peso entre 1 e 100
            }
        }
    }
}

// Função para calcular o custo de um caminho
int calcular_custo(int caminho[], int n) {
    int custo = 0;
    // Soma o custo de ir de cada cidade para a próxima
    for (int i = 0; i < n - 1; i++) {
        custo += grafo[caminho[i]][caminho[i + 1]];
    }
    // Adiciona o custo de voltar ao vértice inicial (fechar o ciclo)
    custo += grafo[caminho[n - 1]][caminho[0]];
    return custo;
}

// Função para trocar dois elementos de um array
void trocar(int *a, int *b) {
    int temp = *a;  // Guarda o valor de 'a' temporariamente
    *a = *b;        // Coloca o valor de 'b' em 'a'
    *b = temp;      // Coloca o valor antigo de 'a' em 'b'
}

// Função para inverter um array entre dois índices
void inverter(int arr[], int inicio, int fim) {
    while (inicio < fim) {
        trocar(&arr[inicio], &arr[fim]);  // Troca os elementos das pontas
        inicio++;  // Avança do início
        fim--;     // Recua do fim
    }
}

// Função para gerar a próxima permutação (algoritmo de Narayana)
// Gera todas as permutações em ordem lexicográfica
bool proxima_permutacao(int arr[], int n) {
    
    int i = n - 2;
    while (i >= 0 && arr[i] >= arr[i + 1]) {
        i--;
    }
    
    // Se não encontrou, significa que já está na última permutação (ordem decrescente)
    if (i < 0) {
        return false;  // Não há mais permutações
    }
    
    int j = n - 1;
    while (arr[j] <= arr[i]) {
        j--;
    }
    
    trocar(&arr[i], &arr[j]);
    
    // Isso garante que estamos gerando em ordem lexicográfica
    inverter(arr, i + 1, n - 1);
    
    return true;  // Gerou uma nova permutação
}

// Função para resolver o TSP usando força bruta
int tsp_forca_bruta(int n, long long *num_permutacoes, time_t tempo_inicio_total) {
    // Cria array inicial com todos os vértices (exceto o 0, que será fixo como início)
    int caminho[MAX_VERTICES];
    int melhor_caminho[MAX_VERTICES];
    int melhor_custo = INT_MAX;
    *num_permutacoes = 0;
    
    // Inicializa o caminho: [0, 1, 2, ..., n-1]
    for (int i = 0; i < n; i++) {
        caminho[i] = i;
    }
    
    // Fixa o vértice 0 como início (reduz o número de permutações)
    // Permuta apenas os vértices de 1 a n-1
    do {
        // Verifica o tempo a cada 100000 permutações para não impactar muito a performance
        if (*num_permutacoes % 100000 == 0 && *num_permutacoes > 0) {
            time_t tempo_atual = time(NULL);
            double tempo_decorrido = difftime(tempo_atual, tempo_inicio_total);
            if (tempo_decorrido >= MAX_EXECUTION_TIME) {
                return -1; // Interrompido por tempo
            }
        }
        
        (*num_permutacoes)++;
        int custo = calcular_custo(caminho, n);
        
        if (custo < melhor_custo) {
            melhor_custo = custo;
            for (int i = 0; i < n; i++) {
                melhor_caminho[i] = caminho[i];
            }
        }
        
        // Gera próxima permutação dos vértices de 1 a n-1
        // (mantém o 0 fixo na primeira posição)
    } while (proxima_permutacao(caminho + 1, n - 1));
    
    return melhor_custo;
}

// Função para imprimir o grafo (útil para debug)
void imprimir_grafo(int n) {
    printf("\nMatriz de adjacência do grafo (%dx%d):\n", n, n);
    printf("    ");
    for (int i = 0; i < n; i++) {
        printf("%4d ", i);
    }
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("%2d: ", i);
        for (int j = 0; j < n; j++) {
            printf("%4d ", grafo[i][j]);
        }
        printf("\n");
    }
}

int main() {
    printf("=== Problema do Caixeiro Viajante - Forca Bruta ===\n\n");
    printf("Gerando instancias e calculando solucoes...\n");
    printf("Tempo maximo de execucao: %d segundos (30 minutos)\n\n", MAX_EXECUTION_TIME);
    printf("%-10s %-15s %-20s %-20s %-15s\n", "Vertices", "Tempo (s)", "Custo Minimo", "Permutacoes", "Status");
    printf("------------------------------------------------------------------------------\n");
    
    time_t inicio_total = time(NULL);
    time_t tempo_atual;
    double tempo_decorrido = 0;
    
    int n = 2;
    bool continuar = true;
    
    while (continuar && n <= MAX_VERTICES) {
        // Verifica se já passou 30 minutos
        tempo_atual = time(NULL);
        tempo_decorrido = difftime(tempo_atual, inicio_total);
        
        if (tempo_decorrido >= MAX_EXECUTION_TIME) {
            printf("\nTempo maximo de execucao atingido (30 minutos).\n");
            continuar = false;
            break;
        }
        
        // Verifica se ainda há tempo antes de começar uma nova instância
        tempo_atual = time(NULL);
        tempo_decorrido = difftime(tempo_atual, inicio_total);
        if (tempo_decorrido >= MAX_EXECUTION_TIME) {
            printf("\nTempo maximo de execucao atingido antes de iniciar instancia de %d vertices.\n", n);
            continuar = false;
            break;
        }
        
        // Gera grafo aleatório para n vértices
        gerar_grafo_aleatorio(n);
        
        // Mede o tempo de execução
        long long num_permutacoes = 0;
        clock_t inicio = clock();
        int melhor_custo = tsp_forca_bruta(n, &num_permutacoes, inicio_total);
        clock_t fim = clock();
        
        double tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
        
        // Verifica se foi interrompido por tempo ou se ainda está dentro do tempo limite
        tempo_atual = time(NULL);
        tempo_decorrido = difftime(tempo_atual, inicio_total);
        
        if (melhor_custo == -1 || tempo_decorrido >= MAX_EXECUTION_TIME) {
            printf("%-10d %-15.6f %-20s %-20lld %-15s\n", n, tempo_execucao, "INTERROMPIDO", num_permutacoes, "INTERROMPIDO");
            if (melhor_custo == -1) {
                printf("\nInstancia de %d vertices interrompida por tempo maximo de execucao.\n", n);
            } else {
                printf("\nTempo maximo de execucao atingido apos processar %d vertices.\n", n);
            }
            continuar = false;
        } else {
            printf("%-10d %-15.6f %-20d %-20lld %-15s\n", n, tempo_execucao, melhor_custo, num_permutacoes, "CONCLUIDO");
        }
        
        n++;
    }
    
    tempo_atual = time(NULL);
    tempo_decorrido = difftime(tempo_atual, inicio_total);
    
    printf("\n------------------------------------------------------------\n");
    printf("Tempo total de execucao: %.2f segundos\n", tempo_decorrido);
    printf("Numero de instancias processadas: %d\n", n - 2);
    
    return 0;
}

