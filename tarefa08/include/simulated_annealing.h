
#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include <stdint.h>
#include "grafo.h"
#include "caminho.h"


typedef struct {
    Caminho melhorCaminho;

    int encontrouDestino;
    int iteracoes;
    int solucoesAceitas;
    int solucoesMelhores;

    double tempoExecucao;
} ResultadoSA;

typedef struct {
    double temperaturaInicial;
    double temperaturaMinima;
    double taxaResfriamento;

    int iteracoesPorTemperatura;
    double tempoMaximo;

    unsigned int semente;
} ConfigSA;

ResultadoSA *executarSA(
    const Grafo *grafo,
    int origem,
    int destino,
    ConfigSA config
);

void imprimirResultadoSA(
    const ResultadoSA *resultado
);

void destruirResultadoSA(
    ResultadoSA *resultado
);

#endif
