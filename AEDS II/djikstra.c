#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

#define TEMPO_LIMITE 1800 // 30 minutos em segundos

long long int comparacoes = 0;

// Função auxiliar: retorna o vértice com menor distância ainda não visitado
int menorDistancia(int dist[], bool visitado[], int n) {
    int min = INT_MAX;
    int indiceMin = -1;

    for (int v = 0; v < n; v++) {
        comparacoes++; // comparação do if
        if (!visitado[v] && dist[v] <= min) {
            min = dist[v];
            indiceMin = v;
        }
    }
    return indiceMin;
}

// Dijkstra clássico com matriz de adjacência
void dijkstra(int **grafo, int n, int origem) {
    int *dist = malloc(n * sizeof(int));
    bool *visitado = malloc(n * sizeof(bool));

    for (int i = 0; i < n; i++) {
        dist[i] = INT_MAX;
        visitado[i] = false;
    }
    dist[origem] = 0;

    for (int cont = 0; cont < n - 1; cont++) {
        int u = menorDistancia(dist, visitado, n);
        if (u == -1) break;
        visitado[u] = true;

        for (int v = 0; v < n; v++) {
            comparacoes++;
            if (!visitado[v] && grafo[u][v] > 0 && dist[u] != INT_MAX
                && dist[u] + grafo[u][v] < dist[v]) {
                dist[v] = dist[u] + grafo[u][v];
            }
        }
    }

    free(dist);
    free(visitado);
}

int main() {
    srand(time(NULL));
    time_t inicioGeral = time(NULL);

    FILE *saida = fopen("resultados.csv", "w");
    if (!saida) {
        printf("Erro ao criar arquivo de resultados.\n");
        return 1;
    }

    fprintf(saida, "Vertices,Comparacoes,Tempo(seg)\n");

    for (int n = 4; n <= 1000000; n++) {
        double tempoDecorrido = difftime(time(NULL), inicioGeral);
        if (tempoDecorrido > TEMPO_LIMITE) {
            printf("\nTempo total de 30 minutos atingido. Encerrando testes.\n");
            break;
        }

        // Verifica se ainda cabe em memória
        if ((long long)n * n > 200000000) {
            printf("Grafo com %d vertices excede memoria (~200MB). Encerrando testes.\n", n);
            break;
        }

        // Aloca e gera grafo completo com pesos aleatórios > 0
        int **grafo = malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++) {
            grafo[i] = malloc(n * sizeof(int));
            for (int j = 0; j < n; j++) {
                grafo[i][j] = (i == j) ? 0 : (rand() % 100 + 1);
            }
        }

        comparacoes = 0;
        time_t inicio = time(NULL);

        dijkstra(grafo, n, 0);

        double tempo = difftime(time(NULL), inicio);
        printf("Vertices: %d | Comparacoes: %lld | Tempo: %.2f s\n", n, comparacoes, tempo);
        fprintf(saida, "%d;%lld;%.2f\n", n, comparacoes, tempo);

        // Libera memória
        for (int i = 0; i < n; i++)
            free(grafo[i]);
        free(grafo);
    }

    fclose(saida);
    printf("\nResultados salvos em 'resultados.csv'.\n");

    return 0;
}