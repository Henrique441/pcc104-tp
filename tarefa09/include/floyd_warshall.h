
#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

#include <stdint.h>
#include "../include/grafo.h"

typedef struct {
    int numVertices;

    // Menores distancias entre todos os pares
    int64_t **distancia;

    // Proximo vertice para reconstruir o caminho
    int **proximo;
} ResultadoFloydWarshall;

// Executa o algoritmo de Floyd-Warshall
ResultadoFloydWarshall *floydWarshall(const Grafo *grafo);

// Retorna a menor distancia entre dois vertices
int64_t consultarDistancia(
    const ResultadoFloydWarshall *resultado,
    int origem,
    int destino
);

// Imprime o caminho minimo entre dois vertices
void imprimirCaminhoFloyd(
    const ResultadoFloydWarshall *resultado,
    int origem,
    int destino
);

// Imprime a matriz completa de distancias
void imprimirMatrizDistancias(
    const ResultadoFloydWarshall *resultado
);

// Libera a memoria utilizada
void destruirResultadoFloyd(
    ResultadoFloydWarshall *resultado
);

#endif
