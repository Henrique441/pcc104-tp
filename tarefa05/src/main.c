
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include "grafo.h"
#include "busca_completa.h"

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(
            stderr,
            "Uso: %s <origem> <destino> [tempo_maximo]\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }

    char *fimOrigem;
    char *fimDestino;

    errno = 0;
    long origem = strtol(argv[1], &fimOrigem, 10);
    int origemInvalida =
        errno == ERANGE || fimOrigem == argv[1] ||
        *fimOrigem != '\0' || origem < 1 || origem > INT_MAX;

    errno = 0;
    long destino = strtol(argv[2], &fimDestino, 10);
    int destinoInvalido =
        errno == ERANGE || fimDestino == argv[2] ||
        *fimDestino != '\0' || destino < 1 || destino > INT_MAX;

    if (origemInvalida || destinoInvalido) {
        fprintf(stderr, "Vertices invalidos.\n");
        return EXIT_FAILURE;
    }

    double tempoMaximo = 0;

    if (argc == 4) {
        char *fimTempo;

        errno = 0;
        tempoMaximo = strtod(argv[3], &fimTempo);

        if (errno == ERANGE || fimTempo == argv[3] ||
            *fimTempo != '\0' || !isfinite(tempoMaximo) ||
            tempoMaximo < 0) {
            fprintf(stderr, "Tempo maximo invalido.\n");
            return EXIT_FAILURE;
        }
    }

    Grafo *grafo = lerGrafo();

    if (grafo == NULL) {
        fprintf(stderr, "Erro ao carregar o grafo.\n");
        return EXIT_FAILURE;
    }

    if (origem > grafo->numVertices ||
        destino > grafo->numVertices) {
        fprintf(stderr, "Origem ou destino invalido.\n");
        destruirGrafo(grafo);
        return EXIT_FAILURE;
    }

    clock_t inicio = clock();

    ResultadoBuscaCompleta *resultado =
        buscaCompleta(
            grafo,
            (int)origem,
            (int)destino,
            tempoMaximo
        );

    clock_t fim = clock();

    if (resultado == NULL) {
        fprintf(stderr, "Erro na busca completa.\n");
        destruirGrafo(grafo);
        return EXIT_FAILURE;
    }

    double tempo =
        (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("Tempo de execucao: %.6f segundos\n", tempo);

    imprimirResultadoBuscaCompleta(resultado);

    destruirResultadoBuscaCompleta(resultado);
    destruirGrafo(grafo);

    return EXIT_SUCCESS;
}
