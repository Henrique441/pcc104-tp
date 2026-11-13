
#ifndef CAMINHO_H
#define CAMINHO_H

#include <stdint.h>
#include <time.h>
#include "grafo.h"

typedef struct {
    int *vertices;
    int64_t *pesos;
    int tamanho;
    int64_t custo;
} Caminho;

typedef enum {
    BUSCA_ERRO = -1,
    BUSCA_NAO_ENCONTROU = 0,
    BUSCA_ENCONTROU = 1,
    BUSCA_LIMITE = 2,
    BUSCA_TEMPO = 3
} EstadoBusca;

Caminho criarCaminho(int capacidade);

void liberarCaminho(Caminho *caminho);

int copiarCaminho(
    Caminho *destino,
    const Caminho *origem
);

EstadoBusca construirCaminhoAleatorio(
    const Grafo *grafo,
    int origem,
    int destino,
    Caminho *caminho,
    unsigned int *semente,
    int limiteChamadas,
    clock_t inicio,
    double tempoMaximo
);

EstadoBusca gerarVizinho(
    const Grafo *grafo,
    const Caminho *atual,
    int destino,
    Caminho *vizinho,
    unsigned int *semente,
    int limiteChamadas,
    clock_t inicio,
    double tempoMaximo
);

#endif
