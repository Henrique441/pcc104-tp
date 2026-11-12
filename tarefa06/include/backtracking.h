
#ifndef BACKTRACKING_H
#define BACKTRACKING_H

#include <stdint.h>
#include "grafo.h"

typedef struct {
    int origem;
    int destino;
    int numVertices;

    int64_t melhorCusto;
    int *melhorCaminho;
    int tamanhoMelhorCaminho;

    uint64_t numCaminhosExplorados;
    uint64_t numChamadasRecursivas;
    uint64_t numPodasRealizadas;
    uint64_t podasCusto;
    uint64_t podasLimiteInferior;

    double tempoExecucao;
    int buscaConcluida;
} ResultadoBacktracking;

// modo 1: poda por custo
// modo 2: poda por custo + limite inferior
ResultadoBacktracking *executarBacktrackingModo(
    const Grafo *grafo,
    int origem,
    int destino,
    double tempoMaximo,
    int modo
);

// Mantem compatibilidade com a primeira versao
ResultadoBacktracking *executarBacktracking(
    const Grafo *grafo,
    int origem,
    int destino,
    double tempoMaximo
);

void imprimirResultadoBacktracking(
    const ResultadoBacktracking *resultado
);

void destruirResultadoBacktracking(
    ResultadoBacktracking *resultado
);

#endif
