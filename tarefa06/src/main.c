
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <time.h>

#include "grafo.h"
#include "busca_completa.h"
#include "backtracking.h"

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 5) {
        fprintf(
            stderr,
            "Uso: %s <origem> <destino> "
            "[tempo_maximo] [modo]\n",
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

    if (argc >= 4) {
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

    int modo = 1;

    if (argc == 5) {
        errno = 0;
        long valor = strtol(argv[4], &fim, 10);

        if (errno == ERANGE || fim == argv[4] ||
            *fim != '\0' ||
            valor < 0 || valor > 2) {

            fprintf(stderr,
                "Modo invalido. Use 0, 1 ou 2.\n");
            return EXIT_FAILURE;
        }

        modo = (int)valor;
    }

    Grafo *grafo = lerGrafo();

    if (grafo == NULL) {
        fprintf(stderr, "Erro ao carregar grafo.\n");
        return EXIT_FAILURE;
    }

    if (origem > grafo->numVertices ||
        destino > grafo->numVertices) {

        fprintf(stderr, "Vertices invalidos.\n");
        destruirGrafo(grafo);
        return EXIT_FAILURE;
    }

    printf("Modo: %d\n", modo);

    if (modo == 0) {
        clock_t inicio = clock();

        ResultadoBuscaCompleta *resultado =
            buscaCompleta(
                grafo,
                (int)origem,
                (int)destino,
                tempoMaximo
            );

        double tempo =
            (double)(clock() - inicio) /
            CLOCKS_PER_SEC;

        if (resultado == NULL) {
            fprintf(stderr,
                "Erro na busca completa.\n");
            destruirGrafo(grafo);
            return EXIT_FAILURE;
        }

        printf("Podas realizadas: 0\n");
        printf("Tempo de execucao: %.6f s\n",
            tempo);

        imprimirResultadoBuscaCompleta(resultado);
        destruirResultadoBuscaCompleta(resultado);

    } else {
        ResultadoBacktracking *resultado =
            executarBacktrackingModo(
                grafo,
                (int)origem,
                (int)destino,
                tempoMaximo,
                modo
            );

        if (resultado == NULL) {
            fprintf(stderr,
                "Erro no backtracking.\n");
            destruirGrafo(grafo);
            return EXIT_FAILURE;
        }

        imprimirResultadoBacktracking(resultado);
        destruirResultadoBacktracking(resultado);
    }

    destruirGrafo(grafo);
    return EXIT_SUCCESS;
}
