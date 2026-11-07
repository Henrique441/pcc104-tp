
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "dijkstra.h"

//executa o algoritmo de Dijkstra
ResultadoDijkstra *dijkstra(const Grafo *grafo, int origem) {
    if (grafo == NULL || origem < 1 || origem > grafo->numVertices)
        return NULL;

    ResultadoDijkstra *resultado = malloc(sizeof(ResultadoDijkstra));//aloca espaço para o resultado

    if (resultado == NULL)
        return NULL;

    resultado->numVertices = grafo->numVertices;
    resultado->origem = origem;
    resultado->distancia = NULL;
    resultado->visitado = NULL;
    resultado->predecessor = NULL;

    //aloca os vetores de distancia, visitado e predecessor
    resultado->distancia = malloc((grafo->numVertices + 1) * sizeof(int64_t));

    resultado->visitado = malloc((grafo->numVertices + 1) * sizeof(int));

    resultado->predecessor = malloc((grafo->numVertices + 1) * sizeof(int));

    if (resultado->distancia == NULL || resultado->visitado == NULL || resultado->predecessor == NULL) {
        destruirResultadoDijkstra(resultado);
        return NULL;
    }

    //inicializa os vetores de distancia com infinito, visitado com 0 e predecessor com -1
    for (int i = 1; i <= grafo->numVertices; i++) {
        resultado->distancia[i] = INT64_MAX;
        resultado->visitado[i] = 0;
        resultado->predecessor[i] = -1;
    }

    resultado->distancia[origem] = 0;//A distancia do vertice de origem para ele mesmo é 0

    for (int i = 0; i < grafo->numVertices; i++) {
        int u = -1;//vertice com a menor distancia ainda nao visitado
        int64_t menor = INT64_MAX;//menor distancia encontrada

        //Faz uma busca sequencial para encontrar o vertice com a menor distancia ainda nao visitado
        for (int v = 1; v <= grafo->numVertices; v++) {
            if (!resultado->visitado[v] &&
                resultado->distancia[v] < menor) {
                menor = resultado->distancia[v];
                u = v;
            }
        }

        if (u == -1){
            break;//se nao encontrou nenhum vertice com a menor distancia ainda nao visitado, significa que todos os vertices foram visitados ou que os vertices restantes sao inalcancaveis
        }

        resultado->visitado[u] = 1;

        Aresta *aresta = grafo->vertices[u].primeiraAresta;//ponteiro para a primeira aresta do vertice u

        //passa pelas arestas do vertice u
        while (aresta != NULL) {
            int v = aresta->destino;
            int peso = aresta->peso;

            //se o vertice v nao foi visitado e a distancia do vertice u + peso da aresta for menor que a distancia do vertice v, atualiza a distancia do vertice v e o predecessor dele
            if (!resultado->visitado[v] && resultado->distancia[u] <= INT64_MAX - peso && resultado->distancia[u] + peso < resultado->distancia[v]) {

                resultado->distancia[v] = resultado->distancia[u] + peso;//Atualiza o novo peso da distancia todal

                resultado->predecessor[v] = u;//atualiza o novo predecessor do vertice v para o vertice u
            }

            aresta = aresta->proxima;//Vai para a proxima aresta do vertice que esta sendo analisado
        }
    }

    return resultado;
}

//funcao para imprimir um caminho entre o vertice de origem e um vertice de destino escolhido
void imprimirCaminho(const ResultadoDijkstra *resultado, int destino) {
    if (resultado == NULL || destino < 1 || destino > resultado->numVertices)
        return;

    int origem = resultado->origem;

    if (resultado->distancia[destino] == INT64_MAX) {
        printf("[%d,%d](INF)\n", origem, destino);
        return;
    }

    int *caminho = malloc(
        (resultado->numVertices + 1) * sizeof(int)
    );

    if (caminho == NULL)
        return;

    int tamanho = 0;
    int atual = destino;

    while (atual != -1 && tamanho <= resultado->numVertices) {
        caminho[tamanho++] = atual;
        atual = resultado->predecessor[atual];
    }

    if (tamanho == 0 || caminho[tamanho - 1] != origem) {
        free(caminho);
        return;
    }

    printf("[%d,%d](%" PRId64 ")", origem, destino, resultado->distancia[destino]);

    //printa o caminho
    for (int i = tamanho - 1; i >= 0; i--) {
        printf("%d", caminho[i]);
        printf(" ");

    }
    printf("\n");

    free(caminho);
}

void imprimirTodosCaminhos(
    const ResultadoDijkstra *resultado) {
    if (resultado == NULL)
        return;

    for (int v = 1; v <= resultado->numVertices; v++) {
        if (v != resultado->origem)
            imprimirCaminho(resultado, v);
    }
}

//Destruir o resultado do algoritmo de Dijkstra
void destruirResultadoDijkstra(
    ResultadoDijkstra *resultado) {
    if (resultado == NULL)
        return;

    free(resultado->distancia);
    free(resultado->visitado);
    free(resultado->predecessor);
    free(resultado);
}
