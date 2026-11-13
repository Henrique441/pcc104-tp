
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

#include "grafo.h"
#include "guloso.h"

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(
            stderr,
            "Uso: %s <origem> <destino> [tempo_maximo]\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }

    char *fim;

    errno = 0;
    long origem = strtol(argv[1], &fim, 10);

    if (errno == ERANGE || fim == argv[1] ||
        *fim != '\0' || origem < 1 ||
        origem > INT_MAX) {

        fprintf(stderr, "Origem invalida.\n");
        return EXIT_FAILURE;
    }

    errno = 0;
    long destino = strtol(argv[2], &fim, 10);

    if (errno == ERANGE || fim == argv[2] ||
        *fim != '\0' || destino < 1 ||
        destino > INT_MAX) {

        fprintf(stderr, "Destino invalido.\n");
        return EXIT_FAILURE;
    }

    double tempoMaximo = 60.0;

    if (argc == 4) {
        errno = 0;
        tempoMaximo = strtod(argv[3], &fim);

        if (errno == ERANGE || fim == argv[3] ||
            *fim != '\0' ||
            !isfinite(tempoMaximo) ||
            tempoMaximo < 0) {

            fprintf(stderr, "Tempo invalido.\n");
            return EXIT_FAILURE;
        }
    }

    Grafo *grafo = lerGrafo();

    if (grafo == NULL) {
        fprintf(stderr, "Erro ao ler o grafo.\n");
        return EXIT_FAILURE;
    }

    ResultadoGuloso *resultado = executarGuloso(
        grafo,
        (int)origem,
        (int)destino,
        tempoMaximo
    );

    if (resultado == NULL) {
        fprintf(stderr, "Erro na busca gulosa.\n");
        destruirGrafo(grafo);
        return EXIT_FAILURE;
    }

    imprimirResultadoGuloso(resultado);

    destruirResultadoGuloso(resultado);
    destruirGrafo(grafo);

    return EXIT_SUCCESS;
}
