
#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"


//Função para criar um grafo
Grafo *criarGrafo(int numVertices, int capacidadeArestas) {
    //Confere se pode criar o grafo
    if (numVertices <= 0 || capacidadeArestas < 0) {
        return NULL;
    }

    Grafo *grafo = malloc(sizeof(Grafo));//alloca o grafo

    //confere se a alocação deu certo
    if (grafo == NULL) {
        return NULL;
    }

    //aloca e atribui os elementos do grafo
    grafo->numVertices = numVertices;
    grafo->numArestas = 0;
    grafo->capacidadeArestas = capacidadeArestas;

    grafo->vertices = malloc((numVertices + 1) * sizeof(Vertice));//adiciona +1 para que o indice do vertice seja igual ao id do vertice

    grafo->arestas = NULL;

    if (capacidadeArestas > 0) {
        grafo->arestas = malloc(capacidadeArestas * sizeof(Aresta));
    }

    
    if (grafo->vertices == NULL || (capacidadeArestas > 0 && grafo->arestas == NULL)){
        free(grafo->vertices);
        free(grafo->arestas);
        free(grafo);
        return NULL;
    }

    //adiciona os ids e inicializa a primeira aresta de cada vertice como -1
    for (int i = 1; i <= numVertices; i++) {
        grafo->vertices[i].id = i;
        grafo->vertices[i].primeiraAresta = -1;
    }

    return grafo;
}



//Função que insere uma aresta no grafo
//Parametros: grafo, vertice de origem, vertice de destino e peso da aresta
int inserirAresta(Grafo *grafo, int origem, int destino, int peso) {

    //confere se pode inserir a aresta
    if (grafo == NULL || origem < 1 || origem > grafo->numVertices || destino < 1 || destino > grafo->numVertices || peso < 0 || grafo->numArestas >= grafo->capacidadeArestas) {
        return 0;
    }

    int indice = grafo->numArestas;//indice da nova aresta

    grafo->arestas[indice].destino = destino;//vertice de destino da aresta
    grafo->arestas[indice].peso = peso;//peso da aresta
    grafo->arestas[indice].proxima = grafo->vertices[origem].primeiraAresta;//proxima aresta do vertice de origem

    grafo->vertices[origem].primeiraAresta = indice;//atualiza a primeira aresta do vertice para a nova aresta

    grafo->numArestas++;//aumenta a quantidade de arestas do grafo

    return 1;
}

//funcao que destroi o grafo
void destruirGrafo(Grafo *grafo) {
    if (grafo == NULL) {
        return;
    }

    free(grafo->vertices);
    free(grafo->arestas);
    free(grafo);
}




//função para ler um grafo da entrada padrão
Grafo *lerGrafo(void) {
    char linha[256];
    Grafo *grafo = NULL;
    int numVertices, numArestas;
    int origem, destino, peso;

    while (fgets(linha, sizeof(linha), stdin) != NULL) {
        if (linha[0] == 'c' || linha[0] == '\n') {
            continue;
        }

        //Cria o grafo quando le a linha com a quantidade de vertices e arestas
        if (linha[0] == 'p') {
            if (grafo != NULL || sscanf(linha, "p sp %d %d", &numVertices, &numArestas) != 2) {//le a quantidade de vertices e linhas
                destruirGrafo(grafo);
                return NULL;
            }

            grafo = criarGrafo(numVertices, numArestas);//Cria o grafo

            if (grafo == NULL) {
                return NULL;
            }
        } else if (linha[0] == 'a') {//Se a linha for do tipo a insere a aresta no grafo
            if (grafo == NULL || sscanf(linha, "a %d %d %d", &origem, &destino, &peso) != 3) {
                destruirGrafo(grafo);
                return NULL;
            }

            if (!inserirAresta(grafo, origem, destino, peso)) {
                destruirGrafo(grafo);
                return NULL;
            }
        } else {
            destruirGrafo(grafo);
            return NULL;
        }
    }

    //confere se o grafo foi criado e se a quantidade de arestas lidas é igual a quantidade de arestas lidas
    if (grafo == NULL || grafo->numArestas != grafo->capacidadeArestas) {
        destruirGrafo(grafo);
        return NULL;
    }

    return grafo;
}


