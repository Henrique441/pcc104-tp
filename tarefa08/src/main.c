
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "grafo.h"
#include "simulated_annealing.h"

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 5) {
        fprintf(stderr,
            "Uso: %s <origem> <destino> "
            "[tempo_maximo] [semente]\n",
            argv[0]);
        return EXIT_FAILURE;
    }

    int origem = atoi(argv[1]);
    int destino = atoi(argv[2]);

    ConfigSA config = {
        .temperaturaInicial = 100.0,
        .temperaturaMinima = 0.01,
        .taxaResfriamento = 0.995,
        .iteracoesPorTemperatura = 20,
        .tempoMaximo = 60.0,
        .semente = (unsigned int)time(NULL)
    };

    if (argc >= 4)
        config.tempoMaximo = atof(argv[3]);

    if (argc == 5)
        config.semente = (unsigned int)strtoul(
            argv[4], NULL, 10
        );

    Grafo *grafo = lerGrafo();

    if (grafo == NULL) {
        fprintf(stderr, "Erro ao ler grafo.\n");
        return EXIT_FAILURE;
    }

    ResultadoSA *resultado = executarSA(
        grafo, origem, destino, config
    );

    if (resultado == NULL) {
        fprintf(stderr, "Erro no Simulated Annealing.\n");
        destruirGrafo(grafo);
        return EXIT_FAILURE;
    }

    imprimirResultadoSA(resultado);

    destruirResultadoSA(resultado);
    destruirGrafo(grafo);

    return EXIT_SUCCESS;
}
