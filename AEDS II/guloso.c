#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

#define TEMPO_LIMITE 60*30 // segundos
long long int comparacoes = 0;

// Função auxiliar: encontra o vértice com menor chave que ainda não foi incluído na MST
int menorChave(int chave[], bool incluido[], int n) {
    int min = INT_MAX;
    int indiceMin = -1;

    for (int v = 0; v < n; v++) {
        comparacoes++;
        if (!incluido[v] && chave[v] < min) {
            min = chave[v];
            indiceMin = v;
        }
    }
    return indiceMin;
}

// Algoritmo de Prim (versão gulosa)
void primMST(int **grafo, int n) {
    int *chave = malloc(n * sizeof(int));    // Menor peso de aresta para conectar o vértice
    bool *incluido = malloc(n * sizeof(bool)); // Se o vértice já está na MST
    int *pai = malloc(n * sizeof(int));       // Armazena a MST resultante

    for (int i = 0; i < n; i++) {
        chave[i] = INT_MAX;
        incluido[i] = false;
        pai[i] = -1;
    }

    chave[0] = 0; // Começa pelo vértice 0

    for (int cont = 0; cont < n - 1; cont++) {
        int u = menorChave(chave, incluido, n);
        if (u == -1) break;
        incluido[u] = true;

        // Atualiza valores de chave dos vértices adjacentes
        for (int v = 0; v < n; v++) {
            comparacoes++;
            if (grafo[u][v] && !incluido[v] && grafo[u][v] < chave[v]) {
                pai[v] = u;
                chave[v] = grafo[u][v];
            }
        }
    }

    // Soma o custo total da MST
    int custoTotal = 0;
    for (int i = 1; i < n; i++)
        if (pai[i] != -1)
            custoTotal += grafo[i][pai[i]];

    printf("Custo total da MST: %d\n", custoTotal);

    free(chave);
    free(incluido);
    free(pai);
}

int main() {
    srand(time(NULL));
    clock_t inicioGeral = clock();

    FILE *saida = fopen("resultados.csv", "w");
    if (!saida) {
        printf("Erro ao criar arquivo de resultados.\n");
        return 1;
    }

    fprintf(saida, "Vertices,Comparacoes,Tempo(seg)\n");

    for (int n = 4; n <= 1000000; n++) {
        double tempoDecorrido = (double)(clock() - inicioGeral) / CLOCKS_PER_SEC;
        if (tempoDecorrido > TEMPO_LIMITE) {
            printf("\nTempo total de %.2f minutos atingido. Encerrando testes.\n", TEMPO_LIMITE / 60.0);
            break;
        }

        // Aloca e gera grafo completo com pesos aleatórios > 0
        int **grafo = malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++) {
            grafo[i] = malloc(n * sizeof(int));
            for (int j = 0; j < n; j++) {
                if (i == j) grafo[i][j] = 0;
                else grafo[i][j] = rand() % 100 + 1; // pesos de 1 a 100
            }
        }

        comparacoes = 0;
        clock_t inicio = clock();
        primMST(grafo, n);
        double tempo = (double)(clock() - inicio) / CLOCKS_PER_SEC;

        fprintf(saida, "%d;%lld;%.6f\n", n, comparacoes, tempo);
        printf("N=%d, Comparacoes=%lld, Tempo=%.6f s\n", n, comparacoes, tempo);

        for (int i = 0; i < n; i++)
            free(grafo[i]);
        free(grafo);
    }

    fclose(saida);
    printf("\nResultados salvos em resultados.csv\n");
    return 0;
}