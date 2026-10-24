
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "dijkstra.h"

//executa o algoritmo de Dijkstra
ResultadoDijkstra *dijkstra(const Grafo *grafo, int origem) {

    //Verifica se pode prosseguir com o algoritmo
    if (grafo == NULL || origem < 1 || origem > grafo->numVertices) {
        return NULL;
    }

    int n = grafo->numVertices;

    ResultadoDijkstra *resultado = malloc(sizeof(ResultadoDijkstra));

    if (resultado == NULL) {
        return NULL;
    }

    resultado->numVertices = n;
    resultado->origem = origem;
    resultado->distancia = NULL;
    resultado->visitado = NULL;
    resultado->predecessor = NULL;

    //aloca os vetores de distancia, visitado e predecessor
    resultado->distancia = malloc((n + 1) * sizeof(int64_t));
    resultado->visitado = calloc(n + 1, sizeof(int));
    resultado->predecessor = malloc((n + 1) * sizeof(int));

    //confere se a alocação deu certo
    if (resultado->distancia == NULL || resultado->visitado == NULL || resultado->predecessor == NULL) {
        destruirResultadoDijkstra(resultado);
        return NULL;
    }

    //inicializa os vetores de distancia com infinito e predecessor com -1
    for (int i = 1; i <= n; i++) {
        resultado->distancia[i] = INT64_MAX;
        resultado->predecessor[i] = -1;
    }

    resultado->distancia[origem] = 0;//A distancia do vertice de origem para ele mesmo é 0

    for (int i = 1; i <= n; i++) {
        int u = -1;//vertice com a menor distancia ainda nao visitado
        int64_t menorDistancia = INT64_MAX;//menor distancia encontrada

        //Faz uma busca sequencial para encontrar o vertice com a menor distancia ainda nao visitado
        for (int v = 1; v <= n; v++) {
            if (!resultado->visitado[v] && resultado->distancia[v] < menorDistancia) {//se o vertice nao foi visitado e a distancia dele é menor que a menor distancia encontrada
                menorDistancia = resultado->distancia[v];//atualiza a menor distancia encontrada
                u = v;//atualiza o vertice com a menor distancia ainda nao visitado
            }
        }

        if (u == -1) {
            break;//se nao encontrou nenhum vertice com a menor distancia ainda nao visitado, significa que todos os vertices foram visitados ou que os vertices restantes sao inalcancaveis
        }

        resultado->visitado[u] = 1;

        int indice = grafo->vertices[u].primeiraAresta;//indice da primeira aresta do vertice u

        //passa pelas arestas do vertice u
        while (indice != -1) {
            Aresta aresta = grafo->arestas[indice];

            int v = aresta.destino;
            int peso = aresta.peso;

            //se o vertice v nao foi visitado e a distancia do vertice u + peso da aresta for menor que a distancia do vertice v, atualiza a distancia do vertice v e o predecessor dele
            if (!resultado->visitado[v] && resultado->distancia[u] <= INT64_MAX - peso) {
                int64_t novaDistancia = resultado->distancia[u] + peso;

                if (novaDistancia < resultado->distancia[v]) {//se a nova distancia for menor que a distancia atual do vertice v, atualiza a distancia e o pai
                    resultado->distancia[v] = novaDistancia;//atualizando a distancia
                    resultado->predecessor[v] = u;//atualizando o pai
                }
            }

            indice = aresta.proxima;//Vai para a proxima aresta do vertice que esta sendo analisado
        }
    }

    return resultado;
}

//funcao que libera a memoria alocada para o resultado do Dijkstra
void destruirResultadoDijkstra(ResultadoDijkstra *resultado) {
    if (resultado == NULL) {
        return;
    }

    free(resultado->distancia);
    free(resultado->visitado);
    free(resultado->predecessor);
    free(resultado);
}

//funcao para imprimir um caminho entre o vertice de origem e um vertice de destino escolhido
void imprimirCaminho(const ResultadoDijkstra *resultado, int destino) {
    //Confere se pode imprimir
    if (resultado == NULL || destino < 1 || destino > resultado->numVertices) {
        return;
    }

    //Confere se existe um caminho entre o vertice de origem e o vertice de destino
    if (resultado->distancia[destino] == INT64_MAX) {
        printf("Nao existe caminho.\n");
        return;
    }

    int *caminho = malloc((resultado->numVertices + 1) * sizeof(int));//aloca espaco para os vertices do caminho

    if (caminho == NULL) {
        fprintf(stderr, "Erro de alocacao.\n");
        return;
    }

    int tamanho = 0;
    int atual = destino;

    //Percorre o caminho dos vertices e de seus pais até a origem
    while (atual != -1 && tamanho < resultado->numVertices) {
        caminho[tamanho++] = atual;
        atual = resultado->predecessor[atual];
    }

    if (atual != -1 || caminho[tamanho - 1] != resultado->origem) {
        printf("Erro ao reconstruir o caminho.\n");
        free(caminho);
        return;
    }

    //printa o caminho
    for (int i = tamanho - 1; i >= 0; i--) {
        printf("%d", caminho[i]);

        if (i > 0) {
            printf(" -> ");
        }
    }

    printf("\n");

    free(caminho);
}

//Imprimir todos os caminhos
void imprimirTodosCaminhos(const ResultadoDijkstra *resultado) {
    if (resultado == NULL) {
        return;
    }

    int n = resultado->numVertices;
    int *caminho = malloc((n + 1) * sizeof(int));

    if (caminho == NULL) {
        fprintf(stderr, "Erro de alocacao.\n");
        return;
    }

    //Passa por todos os vertices e imprimir o caminho do vertice de origem até cada um e o valor total do peso
    for (int destino = 1; destino <= n; destino++) {
        if (destino == resultado->origem) {
            continue;
        }

        //Confere se existe um caminho entre o vertice de origem e o vertice de destino
        if (resultado->distancia[destino] == INT64_MAX) {
            printf("[%d,%d](INF)\n", resultado->origem, destino);
            continue;
        }

        int tamanho = 0;
        int atual = destino;

        //Percorre o caminho dos vertices e de seus pais até a origem
        while (atual != -1 && tamanho < n) {
            caminho[tamanho++] = atual;
            atual = resultado->predecessor[atual];
        }

        if (atual != -1 ||tamanho == 0 || caminho[tamanho - 1] != resultado->origem) {
            fprintf(stderr, "Erro no caminho ate %d.\n", destino);
            free(caminho);
            return;
        }

        printf("[%d,%d](%" PRId64 ")", resultado->origem, destino, resultado->distancia[destino]);

        //printa o caminho
        for (int i = tamanho - 1; i >= 0; i--) {
            printf(" %d", caminho[i]);
        }

        printf("\n");
    }

    free(caminho);
}


