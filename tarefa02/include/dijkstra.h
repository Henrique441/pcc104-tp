
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdint.h>
#include "grafo.h"

//Estrutura para armazenar o resultado do algoritmo de Dijkstra
typedef struct {
    int numVertices;//Quantidade de verttices do grafo
    int origem;//Vertice de origem
    int64_t *distancia;//vetor de distancias do vertice de origem para cada vertice do grafo
    int *visitado;//vetor de visitados, 1 se o vertice foi visitado, 0 caso contrario
    int *predecessor;//vetor de predecessores
} ResultadoDijkstra;

//funcao que executat o Dijkstra
//Parametros: grafo e vertice de origem
ResultadoDijkstra *dijkstra(const Grafo *grafo, int origem);

//funcao que libera a memoria alocada para o resultado do Dijkstra
//Parametros: resultado do Dijkstra
void destruirResultadoDijkstra(ResultadoDijkstra *resultado);

//Imprimi o caminho do vertice de origem até um vertice de destinho escolhido
//Parametros: resultado do Dijkstra e vertice de destino
void imprimirCaminho(const ResultadoDijkstra *resultado, int destino);

//Imprimir o caminho do vertice de origem ate todos os outros vertices
//Parametros: resultado do Dijkstra
void imprimirTodosCaminhos(const ResultadoDijkstra *resultado);

#endif
