
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#include "../include/grafo.h"
#include "../include/dijkstra.h"

static int lerInteiro(const char *texto, int *valor) {
    char *fim;
    errno = 0;

    long numero = strtol(texto, &fim, 10);

    if (errno != 0 || texto == fim ||
        *fim != '\0' || numero < INT_MIN ||
        numero > INT_MAX) {
        return 0;
    }

    *valor = (int)numero;
    return 1;
}

int main(int argc, char *argv[]) {
    int origem = 1;
    int destino = -1;

    if (argc > 3) {
        fprintf(stderr,
                "Uso: %s [origem] [destino]\n", argv[0]);
        return 1;
    }

    if (argc >= 2 && !lerInteiro(argv[1], &origem)) {
        fprintf(stderr, "Origem invalida.\n");
        return 1;
    }

    if (argc == 3 && !lerInteiro(argv[2], &destino)) {
        fprintf(stderr, "Destino invalido.\n");
        return 1;
    }

    Grafo *grafo = lerGrafo();

    if (grafo == NULL) {
        fprintf(stderr, "Erro ao ler o grafo.\n");
        return 1;
    }

    if (origem < 1 || origem > grafo->numVertices ||
        (destino != -1 &&
         (destino < 1 || destino > grafo->numVertices))) {
        fprintf(stderr, "Vertice invalido.\n");
        destruirGrafo(grafo);
        return 1;
    }

    clock_t inicio = clock();

    ResultadoDijkstra *resultado = dijkstra(grafo, origem);

    clock_t fim = clock();

    if (resultado == NULL) {
        fprintf(stderr, "Erro ao executar Dijkstra.\n");
        destruirGrafo(grafo);
        return 1;
    }

    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    fprintf(stderr, "Tempo de execucao: %.6f segundos\n", tempo);

    fprintf(stderr, "Insercoes na fila: %" PRId64 "\n",
            resultado->numInsercoes);

    fprintf(stderr, "Remocoes da fila: %" PRId64 "\n",
            resultado->numRemocoes);

    fprintf(stderr, "Atualizacoes de distancia: %" PRId64 "\n",
            resultado->numAtualizacoes);

    fprintf(stderr, "Vertices processados: %" PRId64 "\n",
            resultado->numVerticesProcessados);

    if (destino != -1) {
        imprimirCaminho(resultado, destino);
    } else {
        imprimirTodosCaminhos(resultado);
    }

    destruirResultadoDijkstra(resultado);
    destruirGrafo(grafo);

    return 0;
}
