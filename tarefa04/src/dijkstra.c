
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "../include/dijkstra.h"
#include "../include/fila_prioridade.h"

//executa o algoritmo de Dijkstra
ResultadoDijkstra *dijkstra(const Grafo *grafo, int origem) {
    if (grafo == NULL || origem < 1 || origem > grafo->numVertices)
        return NULL;

    int n = grafo->numVertices;
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

    //inicializa os contadores de insercoes, remocoes, atualizacoes e vertices processados
    resultado->numInsercoes = 0;//inicializa o contador de insercoes
    resultado->numRemocoes = 0;//inicializa o contador de remocoes
    resultado->numAtualizacoes = 0;//inicializa o contador de atualizacoes
    resultado->numVerticesProcessados = 0;//inicializa o contador de vertices processados


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

    FilaPrioridade *fila = criarFilaPrioridade(n);//cria a fila de prioridade

    if (fila == NULL) {
        destruirResultadoDijkstra(resultado);
        return NULL;
    }

    resultado->distancia[origem] = 0;//A distancia do vertice de origem para ele mesmo é 0

    //insere o vertice de origem na fila de prioridade com prioridade 0
    if (!inserir(fila, origem, 0)) {
        destruirFilaPrioridade(fila);
        destruirResultadoDijkstra(resultado);
        return NULL;
    }
    resultado->numInsercoes++;

    //executa o algoritmo de Dijkstra enquanto a fila de prioridade nao estiver vazia
    while (!filaVazia(fila)) {
        int u = extrairMinimo(fila);//extrai o vertice com a menor distancia da fila de prioridade

        resultado->numRemocoes++;//incrementa o contador de remocoes

        if (resultado->visitado[u]) {
            continue;
        }

        resultado->visitado[u] = 1;//marca o vertice como visitado
        resultado->numVerticesProcessados++;//incrementa o contador de vertices processados

        Aresta *aresta = grafo->vertices[u].primeiraAresta;//ponteiro para a primeira aresta do vertice u

        while (aresta != NULL) {//percorre todas as arestas do vertice u
            int v = aresta->destino;
            int peso = aresta->peso;

            if (!resultado->visitado[v] && resultado->distancia[u] <= INT64_MAX - peso) {

                int64_t novaDistancia = resultado->distancia[u] + peso;//calcula a nova distancia do vertice v

                if (novaDistancia < resultado->distancia[v]) {//
                    resultado->distancia[v] = novaDistancia;
                    resultado->predecessor[v] = u;
                    resultado->numAtualizacoes++;

                    if (fila->posicao[v] == -1) {
                        if (!inserir(fila, v, novaDistancia)) {
                            destruirFilaPrioridade(fila);
                            destruirResultadoDijkstra(resultado);
                            return NULL;
                        }

                        resultado->numInsercoes++;
                    } else {
                        if (!diminuirPrioridade(fila, v, novaDistancia)) {//se nao conseguir diminuir a prioridade, libera a memoria e retorna NULL
                            destruirFilaPrioridade(fila);
                            destruirResultadoDijkstra(resultado);
                            return NULL;
                        }
                    }
                }
            }

            aresta = aresta->proxima;
        }
    }

    destruirFilaPrioridade(fila);

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
