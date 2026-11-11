
#ifndef BUSCA_COMPLETA_H
#define BUSCA_COMPLETA_H

#include <stdint.h>
#include "grafo.h"

//Estrutura para armazenar o resultado da busca completa
typedef struct {
    int origem;
    int destino;
    int numVertices;

    int64_t melhorCusto;
    int *melhorCaminho;
    int tamanhoMelhorCaminho;

    uint64_t numCaminhosExplorados;
    uint64_t numChamadasRecursivas;

    int buscaConcluida;
} ResultadoBuscaCompleta;

//funcao que executa a busca completa
//Parametros: grafo, vertice de origem e vertice de destino
ResultadoBuscaCompleta *buscaCompleta(const Grafo *grafo, int origem, int destino, double tempoMaximo);

//funcao que libera a memoria alocada para o resultado da busca completa
//Parametros: resultado da busca completa
void imprimirResultadoBuscaCompleta(const ResultadoBuscaCompleta *resultado);

//funcao que libera a memoria alocada para o resultado da busca completa
//Parametros: resultado da busca completa
void destruirResultadoBuscaCompleta(ResultadoBuscaCompleta *resultado);

#endif
