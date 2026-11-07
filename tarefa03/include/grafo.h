
#ifndef GRAFO_H
#define GRAFO_H

#include "vertice.h"

//estrutura que representa um grafo
typedef struct {
    int numVertices;//quantidade de vertices do grafo
    int numArestas;//quantidade de arestas do grafo
    Vertice *vertices;//vetor de vertices do grafo
} Grafo;

//Cria o grafo com a quantidade de vertices especificada
//parametros: numVertices - quantidade de vertices do grafo
Grafo *criarGrafo(int numVertices);

//Insere uma aresta no grafo, com origem, destino e peso especificados
int inserirAresta(Grafo *grafo, int origem, int destino, int peso);

//Lê o grafo da entrada padrão, no formato especificado
Grafo *lerGrafo(void);

//Destrói o grafo, liberando a memória alocada
void destruirGrafo(Grafo *grafo);

#endif
