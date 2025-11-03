#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>

long long int comparacoes = 0;

// Função auxiliar: retorna o vértice com menor distância ainda não visitado
int menorDistancia(double dist[], bool visitado[], int n) {
    double min = DBL_MAX;
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
void dijkstra(double **grafo, int n, int origem) {
    double *dist = malloc(n * sizeof(double));
    bool *visitado = malloc(n * sizeof(bool));

    for (int i = 0; i < n; i++) {
        dist[i] = DBL_MAX;
        visitado[i] = false;
    }
    dist[origem] = 0;

    for (int cont = 0; cont < n - 1; cont++) {
        int u = menorDistancia(dist, visitado, n);
        if (u == -1) break;
        visitado[u] = true;

        for (int v = 0; v < n; v++) {
            comparacoes++;
            if (!visitado[v] && grafo[u][v] > 0.0 && dist[u] != DBL_MAX
                && dist[u] + grafo[u][v] < dist[v]) {
                dist[v] = dist[u] + grafo[u][v];
            }
        }
    }

    free(dist);
    free(visitado);
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

    // ler arquivo de instancia e gerar grafo (matriz de adjacencia)
    double **grafo = malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        grafo[i] = calloc(n, sizeof(double));
        // diagonal principal permanece 0 (distancia de um vertice para ele mesmo)
    }

    for (int e = 0; e < m; e++) {
        int u, v;
        double w;
        if (fscanf(entrada, "%d %d %lf", &u, &v, &w) != 3) {
            printf("Erro ao ler aresta %d.\n", e + 1);
            // finaliza mais cedo em caso de formato invalido
            break;
        }
        // Arestas direcionadas: u -> v com peso w
        if (u >= 0 && u < n && v >= 0 && v < n) {
            grafo[u][v] = w;
        }
    }

    comparacoes = 0;

    dijkstra(grafo, n, 0);

    // Printa na tela
    printf("Vertices: %d | Comparacoes: %lld\n", n, comparacoes);

    // Libera memória
    for (int i = 0; i < n; i++)
        free(grafo[i]);
    free(grafo);

    fclose(entrada);

    return 0;
}