
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#include "grafo.h"
#include "floyd_warshall.h"

// Converte um argumento para inteiro positivo
static int lerVertice(const char *texto, int *valor) {
    char *fim;
    errno = 0;

    long numero = strtol(texto, &fim, 10);

    if (errno != 0 || texto == fim ||
        *fim != '\0' || numero < 1 ||
        numero > INT_MAX) {
        return 0;
    }

    *valor = (int)numero;
    return 1;
}

int main(int argc, char *argv[]) {

    // Sem argumentos: imprime todas as distancias.
    // Com dois argumentos: consulta origem e destino.
    if (argc != 1 && argc != 3) {
        fprintf(stderr,
            "Uso: %s [origem destino] < grafo.gr\n",
            argv[0]);
        return EXIT_FAILURE;
    }

    int origem = 0;
    int destino = 0;

    if (argc == 3) {
        if (!lerVertice(argv[1], &origem) ||
            !lerVertice(argv[2], &destino)) {
            fprintf(stderr, "Vertices invalidos.\n");
            return EXIT_FAILURE;
        }
    }

    // Le o grafo da entrada padrao
    Grafo *grafo = lerGrafo();

    if (grafo == NULL) {
        fprintf(stderr, "Erro ao ler o grafo.\n");
        return EXIT_FAILURE;
    }

    if (argc == 3 &&
        (origem > grafo->numVertices ||
         destino > grafo->numVertices)) {
        fprintf(stderr,
                "Origem ou destino fora do grafo.\n");
        destruirGrafo(grafo);
        return EXIT_FAILURE;
    }

    // Mede somente a execucao do Floyd-Warshall
    clock_t inicio = clock();

    ResultadoFloydWarshall *resultado =
        floydWarshall(grafo);

    clock_t fim = clock();

    if (resultado == NULL) {
        fprintf(stderr,
                "Erro ao executar Floyd-Warshall.\n");
        destruirGrafo(grafo);
        return EXIT_FAILURE;
    }

    double tempo = (double)(fim - inicio) /
                   CLOCKS_PER_SEC;

    if (argc == 3) {

        int64_t distancia =
            consultarDistancia(resultado, origem, destino);

        printf("Origem: %d\n", origem);
        printf("Destino: %d\n", destino);

        if (distancia == INT64_MAX) {
            printf("Distancia: INF\n");
            printf("Nao existe caminho entre os vertices.\n");
        } else {
            printf("Distancia: %" PRId64 "\n", distancia);

            imprimirCaminhoFloyd(
                resultado, origem, destino);
        }

    } else {
        imprimirMatrizDistancias(resultado);
    }

    fprintf(stderr,
            "Tempo de execucao: %.6f segundos\n",
            tempo);

    destruirResultadoFloyd(resultado);
    destruirGrafo(grafo);

    return EXIT_SUCCESS;
}
