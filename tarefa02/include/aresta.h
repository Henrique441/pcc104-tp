
#ifndef ARESTA_H
#define ARESTA_H

//Estrutura da aresta
typedef struct {
    int destino;//indice do vertice de destino
    int peso;//peso da aresta
    int proxima;//aponta o indice da proxima aresta do vertice
} Aresta;

#endif
