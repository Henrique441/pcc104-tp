
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

//criar grafo
Grafo *criarGrafo(int numVertices) {

    //confere se pode criar o grafo
    if (numVertices <= 0)
        return NULL;

    Grafo *grafo = malloc(sizeof(Grafo));//aloca o grafo

    if (grafo == NULL)
        return NULL;

    //aloca e atribui os elementos do grafo
    grafo->numVertices = numVertices;
    grafo->numArestas = 0;

    grafo->vertices = malloc((numVertices + 1) * sizeof(Vertice));//adiciona +1 para que o indice do vertice seja igual ao id do vertice

    if (grafo->vertices == NULL) {
        free(grafo);
        return NULL;
    }

    //adiciona os ids e inicializa a primeira aresta de cada vertice como NULL
    for (int i = 0; i <= numVertices; i++) {
        grafo->vertices[i].id = i;
        grafo->vertices[i].primeiraAresta = NULL;
    }

    return grafo;
}


int inserirAresta(Grafo *grafo, int origem, int destino, int peso) {
    if (grafo == NULL)
        return 0;

    //confere se pode inserir a aresta
    if (origem < 1 || origem > grafo->numVertices || destino < 1 || destino > grafo->numVertices || peso < 0)
        return 0;

    Aresta *novaAresta = malloc(sizeof(Aresta));//aloca a nova aresta

    if (novaAresta == NULL)
        return 0;

    novaAresta->destino = destino;//vertice de destino
    novaAresta->peso = peso;//peso da aresta

    novaAresta->proxima = grafo->vertices[origem].primeiraAresta;//proxima aresta do vertice de origem

    grafo->vertices[origem].primeiraAresta = novaAresta;//atualiza a primeira aresta do vertice para a nova aresta

    grafo->numArestas++;//aumenta a quantidade de arestas do grafo

    return 1;
}

//função para ler um grafo da entrada padrão
Grafo *lerGrafo(void) {
    char linha[256];
    Grafo *grafo = NULL;
    int quantidadeArestas = -1;

    while (fgets(linha, sizeof(linha), stdin) != NULL) {
        if (linha[0] == 'c' || linha[0] == '\n')
            continue;

        if (linha[0] == 'p') {
            int numVertices;
            int numArestas;

            if (grafo != NULL ||sscanf(linha, "p sp %d %d", &numVertices, &numArestas) != 2 || numVertices <= 0 || numArestas < 0) {
                destruirGrafo(grafo);
                return NULL;
            }

            grafo = criarGrafo(numVertices);//cria o grafo com a quantidade de vertices lida

            if (grafo == NULL)
                return NULL;

            quantidadeArestas = numArestas;
        }
        else if (linha[0] == 'a') {//se for uma linha de aresta, le os valores e insere a aresta no grafo
            int origem, destino, peso;

            if (grafo == NULL ||
                sscanf(linha, "a %d %d %d",
                       &origem, &destino, &peso) != 3 ||
                grafo->numArestas >= quantidadeArestas) {
                destruirGrafo(grafo);
                return NULL;
            }

            if (!inserirAresta(grafo, origem,
                              destino, peso)) {
                destruirGrafo(grafo);
                return NULL;
            }
        }
    }

    //confere se houve algum erro ao criar o grafo
    if (ferror(stdin) || grafo == NULL || grafo->numArestas != quantidadeArestas) {
        destruirGrafo(grafo);
        return NULL;
    }

    return grafo;
}

//funcao que destroi o grafo
void destruirGrafo(Grafo *grafo) {
    if (grafo == NULL)
        return;

    if (grafo->vertices != NULL) {
        for (int i = 1; i <= grafo->numVertices; i++) {//percorre todos os vertices do grafo
            Aresta *atual =
                grafo->vertices[i].primeiraAresta;

            while (atual != NULL) {//percorre todas as arestas do vertice e libera a memoria alocada
                Aresta *proxima = atual->proxima;
                free(atual);
                atual = proxima;
            }
        }
    }

    free(grafo->vertices);
    free(grafo);
}
