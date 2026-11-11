
#ifndef ARESTA_H
#define ARESTA_H

typedef struct Aresta {
    int destino;//Indica o vértice de destino da aresta
    int peso;//Indica o peso da aresta
    struct Aresta *proxima;//Indica a próxima aresta na lista de adjacência
} Aresta;

#endif
