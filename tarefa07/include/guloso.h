
#ifndef GULOSO_H
#define GULOSO_H

#include <stdint.h>
#include "grafo.h"

typedef struct {
    int origem;
    int destino;

    int *caminho;
    int tamanhoCaminho;

    int64_t custo;
    int verticesVisitados;

    int encontrouDestino;
    double tempoExecucao;
} ResultadoGuloso;

ResultadoGuloso *executarGuloso(
    const Grafo *grafo,
    int origem,
    int destino,
    double tempoMaximo
);

void imprimirResultadoGuloso(
    const ResultadoGuloso *resultado
);

void destruirResultadoGuloso(
    ResultadoGuloso *resultado
);

#endif
