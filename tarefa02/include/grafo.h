
#ifndef GRAFO_H
#define GRAFO_H

#include "vertice.h"
#include "aresta.h"

//Estrutura do grafo
typedef struct {
    int numVertices;//Quantidade de vertices do grafo
    int numArestas;//Quantidade de arestas do grafo
    int capacidadeArestas;// Indica a capacidade maxima de arestas que cabem no vetor de arestas alocado

    Vertice *vertices;//Vetor de vertices do grafo
    Aresta *arestas;//Vetor de arestas do grafo
} Grafo;

//Cria um grafo
//Parametros: quantidade de vertices e capacidade maxima de arestas
Grafo *criarGrafo(int numVertices, int capacidadeArestas);

//Insere um arco direcionado no grafo
//Parametros: grafo, vertice de origem, vertice de destino e peso da aresta
int inserirAresta(Grafo *grafo, int origem, int destino, int peso);

//Libera toda a memoria alocada
// Parametros: grafo
void destruirGrafo(Grafo *grafo);

//Le um grafo DIMACS da entrada padrao
//Retorna um ponteiro para o grafo lido
Grafo *lerGrafo(void);

#endif
