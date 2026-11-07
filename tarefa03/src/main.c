
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include "grafo.h"
#include "dijkstra.h"

int converterVertice(const char *texto, int *valor) {
    char *fim;
    errno = 0;

    long numero = strtol(texto, &fim, 10);

    if (errno != 0 || texto == fim || *fim != '\0' || numero < 1 ||
        numero > INT_MAX)
        return 0;

    *valor = (int)numero;
    return 1;
}

int main(int argc, char *argv[]) {
    int origem = 1;
    int destino = -1;

    if (argc > 3) {
        fprintf(stderr, "Uso: %s [origem] [destino]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc >= 2 && !converterVertice(argv[1], &origem)) {
        fprintf(stderr, "Origem invalida.\n");
        return EXIT_FAILURE;
    }

    if (argc == 3 && !converterVertice(argv[2], &destino)) {
        fprintf(stderr, "Destino invalido.\n");
        return EXIT_FAILURE;
    }

    Grafo *grafo = lerGrafo();

    if (grafo == NULL) {
        fprintf(stderr, "Erro ao ler o grafo.\n");
        return EXIT_FAILURE;
    }

    if (origem > grafo->numVertices ||
        (destino != -1 &&
         destino > grafo->numVertices)) {
        fprintf(stderr, "Vertice fora do intervalo.\n");
        destruirGrafo(grafo);
        return EXIT_FAILURE;
    }

    clock_t inicio = clock();

    ResultadoDijkstra *resultado = dijkstra(grafo, origem);

    clock_t fimTempo = clock();

    if (resultado == NULL) {
        fprintf(stderr, "Erro ao executar Dijkstra.\n");
        destruirGrafo(grafo);
        return EXIT_FAILURE;
    }

    double tempo = (fimTempo - inicio) / (double)CLOCKS_PER_SEC;

    fprintf(stderr,"Tempo de Dijkstra: %.6f segundos\n", tempo);

    if (destino != -1)
        imprimirCaminho(resultado, destino);
    else
        imprimirTodosCaminhos(resultado);

    destruirResultadoDijkstra(resultado);
    destruirGrafo(grafo);

    return EXIT_SUCCESS;
}
