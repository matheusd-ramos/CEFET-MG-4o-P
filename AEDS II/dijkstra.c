#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

#define TEMPO_LIMITE 3600 // 30 minutos em segundos

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
    clock_t inicioGeral = clock();

    FILE *saida = fopen("resultados.csv", "w");
    if (!saida) {
        printf("Erro ao criar arquivo de resultados.\n");
        return 1;
    }

    fprintf(saida, "Vertices,Comparacoes,Tempo(seg)\n");

    for (int n = 4; n <= 1000000; n++) {
        double tempoDecorrido = (double) (clock() - inicioGeral)/CLOCKS_PER_SEC;
        if (tempoDecorrido > TEMPO_LIMITE) {
            printf("\nTempo total de 30 minutos atingido. Encerrando testes.\n");
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
        clock_t inicio = clock();

        dijkstra(grafo, n, 0);

        double tempo = (double) (clock() - inicio)/CLOCKS_PER_SEC;


        // Printa na tela
        printf("Vertices: %d | Comparacoes: %lld | Tempo de pesquisa: %.2f s | Tempo Total: %.2f s\n", n, comparacoes, tempo, tempoDecorrido);

        // Salva num arquivo csv
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