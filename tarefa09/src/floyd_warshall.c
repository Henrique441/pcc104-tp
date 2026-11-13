
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>

#include "floyd_warshall.h"

// Aloca uma matriz de distancias
static int64_t **criarMatrizDistancias(int n) {
    int64_t **matriz = calloc(n + 1, sizeof(int64_t *));

    if (matriz == NULL)
        return NULL;

    for (int i = 0; i <= n; i++) {
        matriz[i] = malloc((n + 1) * sizeof(int64_t));

        if (matriz[i] == NULL) {
            for (int j = 0; j < i; j++)
                free(matriz[j]);

            free(matriz);
            return NULL;
        }
    }

    return matriz;
}

// Aloca a matriz utilizada na reconstrucao
static int **criarMatrizProximo(int n) {
    int **matriz = calloc(n + 1, sizeof(int *));

    if (matriz == NULL)
        return NULL;

    for (int i = 0; i <= n; i++) {
        matriz[i] = malloc((n + 1) * sizeof(int));

        if (matriz[i] == NULL) {
            for (int j = 0; j < i; j++)
                free(matriz[j]);

            free(matriz);
            return NULL;
        }
    }

    return matriz;
}

// Libera a memoria alocada para o resultado
void destruirResultadoFloyd(ResultadoFloydWarshall *resultado) {
    if (resultado == NULL)
        return;

    int n = resultado->numVertices;

    if (resultado->distancia != NULL) {
        for (int i = 0; i <= n; i++)
            free(resultado->distancia[i]);

        free(resultado->distancia);
    }

    if (resultado->proximo != NULL) {
        for (int i = 0; i <= n; i++)
            free(resultado->proximo[i]);

        free(resultado->proximo);
    }

    free(resultado);
}

// Executa Floyd-Warshall
ResultadoFloydWarshall *floydWarshall(const Grafo *grafo) {
    if (grafo == NULL || grafo->numVertices <= 0)
        return NULL;

    int n = grafo->numVertices;

    // Aloca a estrutura de resultados
    ResultadoFloydWarshall *resultado =
        calloc(1, sizeof(ResultadoFloydWarshall));

    if (resultado == NULL)
        return NULL;

    resultado->numVertices = n;

    // Aloca as matrizes
    resultado->distancia = criarMatrizDistancias(n);
    resultado->proximo = criarMatrizProximo(n);

    if (resultado->distancia == NULL ||
        resultado->proximo == NULL) {

        destruirResultadoFloyd(resultado);
        return NULL;
    }

    // Inicializa as matrizes
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {

            if (i == j) {
                resultado->distancia[i][j] = 0;
                resultado->proximo[i][j] = i;
            } else {
                resultado->distancia[i][j] = INT64_MAX;
                resultado->proximo[i][j] = -1;
            }
        }
    }

    // Inicializa as distancias com os pesos das arestas
    for (int i = 1; i <= n; i++) {

        Aresta *aresta = grafo->vertices[i].primeiraAresta;

        while (aresta != NULL) {

            int destino = aresta->destino;
            int64_t peso = aresta->peso;

            // Se houver arestas paralelas, mantem a menor
            if (peso < resultado->distancia[i][destino]) {

                resultado->distancia[i][destino] = peso;

                resultado->proximo[i][destino] = destino;
            }

            aresta = aresta->proxima;
        }
    }

    // Algoritmo de Floyd-Warshall
    for (int k = 1; k <= n; k++) {
        for (int i = 1; i <= n; i++) {

            // Nao existe caminho de i ate k
            if (resultado->distancia[i][k] == INT64_MAX)
                continue;

            for (int j = 1; j <= n; j++) {

                // Nao existe caminho de k ate j
                if (resultado->distancia[k][j] == INT64_MAX)
                    continue;

                int64_t dik = resultado->distancia[i][k];
                int64_t dkj = resultado->distancia[k][j];

                // Evita overflow na soma
                if (dik > INT64_MAX - dkj)
                    continue;

                int64_t novoCusto = dik + dkj;

                // Atualiza se encontrou um caminho melhor
                if (novoCusto < resultado->distancia[i][j]) {

                    resultado->distancia[i][j] = novoCusto;

                    resultado->proximo[i][j] =
                        resultado->proximo[i][k];
                }
            }
        }
    }

    return resultado;
}

// Consulta a menor distancia entre origem e destino
int64_t consultarDistancia(
    const ResultadoFloydWarshall *resultado,
    int origem,
    int destino
) {
    if (resultado == NULL)
        return INT64_MAX;

    if (origem < 1 || origem > resultado->numVertices ||
        destino < 1 || destino > resultado->numVertices)
        return INT64_MAX;

    return resultado->distancia[origem][destino];
}

// Imprime o caminho minimo entre dois vertices
void imprimirCaminhoFloyd(
    const ResultadoFloydWarshall *resultado,
    int origem,
    int destino
) {
    if (resultado == NULL ||
        origem < 1 || origem > resultado->numVertices ||
        destino < 1 || destino > resultado->numVertices) {

        printf("Vertices invalidos.\n");
        return;
    }

    if (resultado->proximo[origem][destino] == -1) {
        printf("Nao existe caminho entre %d e %d.\n",
               origem, destino);
        return;
    }

    printf("Caminho: %d", origem);

    int atual = origem;
    int passos = 0;

    while (atual != destino &&
           passos < resultado->numVertices) {

        atual = resultado->proximo[atual][destino];

        if (atual == -1) {
            printf("\nErro na reconstrucao do caminho.\n");
            return;
        }

        printf(" -> %d", atual);

        passos++;
    }

    if (atual != destino) {
        printf("\nErro: caminho excedeu o limite de vertices.\n");
        return;
    }

    printf("\n");
}

// Imprime a matriz completa de distancias
void imprimirMatrizDistancias(
    const ResultadoFloydWarshall *resultado
) {
    if (resultado == NULL)
        return;

    int n = resultado->numVertices;

    printf("\nMatriz de distancias:\n\n");

    printf("%8s", "");

    for (int j = 1; j <= n; j++)
        printf("%8d", j);

    printf("\n");

    for (int i = 1; i <= n; i++) {

        printf("%8d", i);

        for (int j = 1; j <= n; j++) {

            if (resultado->distancia[i][j] == INT64_MAX) {
                printf("%8s", "INF");
            } else {
                printf("%8" PRId64,
                       resultado->distancia[i][j]);
            }
        }

        printf("\n");
    }
}
