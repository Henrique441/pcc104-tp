
#ifndef VERTICE_H
#define VERTICE_H

#include "aresta.h"

//estrutura que representa um vértice do grafo
typedef struct {
    int id;//Indica o identificador do vértice
    Aresta *primeiraAresta;//Indica a primeira aresta da lista de adjacência do vértice
} Vertice;

#endif
