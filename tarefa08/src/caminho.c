
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include "caminho.h"
#include "aresta.h"

//Gera um número pseudoaleatório usando o algoritmo Xorshift.
static unsigned int aleatorio(unsigned int *estado) {
    unsigned int x = *estado;

    //Evita utilizar zero como estado inicial.
    if (x == 0)
        x = 2463534242u;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    *estado = x;
    return x;
}

//Sorteia um número inteiro entre 0 e limite - 1.
static int sortear(unsigned int *estado, int limite) {
    return (int)(aleatorio(estado) % (unsigned int)limite);
}

//Verifica se o tempo máximo de execução foi atingido.
static int tempoEsgotado(clock_t inicio, double limite) {
    return (double)(clock() - inicio) / CLOCKS_PER_SEC
           >= limite;
}

//Aloca memória para armazenar os vértices e pesos de um caminho.
Caminho criarCaminho(int capacidade) {
    Caminho c = {0};

    if (capacidade <= 0)
        return c;

    c.vertices = malloc(
        (size_t)capacidade * sizeof(int)
    );

    c.pesos = malloc(
        (size_t)capacidade * sizeof(int64_t)
    );

    //Libera a memória caso alguma alocação falhe.
    if (c.vertices == NULL || c.pesos == NULL) {
        liberarCaminho(&c);
    }

    return c;
}

//Libera a memória utilizada pelo caminho e reinicia seus campos.
void liberarCaminho(Caminho *c) {
    if (c == NULL)
        return;

    free(c->vertices);
    free(c->pesos);

    c->vertices = NULL;
    c->pesos = NULL;
    c->tamanho = 0;
    c->custo = 0;
}

// Copia os vértices, pesos e informações de um caminho para outro.
int copiarCaminho(Caminho *destino, const Caminho *origem) {
    if (destino == NULL || origem == NULL ||
        destino->vertices == NULL ||
        destino->pesos == NULL ||
        origem->vertices == NULL ||
        origem->pesos == NULL)
        return 0;

    //Copia os vetores de vértices e pesos.
    memcpy(
        destino->vertices,
        origem->vertices,
        (size_t)origem->tamanho * sizeof(int)
    );

    memcpy(
        destino->pesos,
        origem->pesos,
        (size_t)origem->tamanho * sizeof(int64_t)
    );

    destino->tamanho = origem->tamanho;
    destino->custo = origem->custo;

    return 1;
}

//Realiza uma busca em profundidade com ordem aleatória das arestas
static EstadoBusca dfsAleatoria(
    const Grafo *grafo,
    int atual,
    int destino,
    Caminho *caminho,
    int *visitado,
    unsigned int *semente,
    int *chamadasRestantes,
    clock_t inicio,
    double tempoMaximo
) {
    //verifica os limites de tempo e de chamadas recursivas
    if (tempoEsgotado(inicio, tempoMaximo))
        return BUSCA_TEMPO;

    if (*chamadasRestantes <= 0)
        return BUSCA_LIMITE;

    (*chamadasRestantes)--;

    //Encerra a busca quando o destino é encontrado
    if (atual == destino)
        return BUSCA_ENCONTROU;

    int grau = 0;

    const Aresta *a =
        grafo->vertices[atual].primeiraAresta;

    // Conta as arestas válidas para vértices ainda não visitados.
    while (a != NULL) {
        if (!visitado[a->destino] && a->peso >= 0)
            grau++;

        a = a->proxima;
    }

    if (grau == 0)
        return BUSCA_NAO_ENCONTROU;

    //Aloca um vetor para armazenar as possíveis próximas arestas.
    const Aresta **opcoes = malloc(
        (size_t)grau * sizeof(Aresta *)
    );

    if (opcoes == NULL)
        return BUSCA_ERRO;

    int quantidade = 0;

    a = grafo->vertices[atual].primeiraAresta;

    //Armazena as arestas que podem ser exploradas
    while (a != NULL) {
        if (!visitado[a->destino] && a->peso >= 0)
            opcoes[quantidade++] = a;

        a = a->proxima;
    }

    //Embaralha as arestas para variar a ordem da busca
    for (int i = quantidade - 1; i > 0; i--) {
        int j = sortear(semente, i + 1);

        const Aresta *aux = opcoes[i];
        opcoes[i] = opcoes[j];
        opcoes[j] = aux;
    }

    EstadoBusca estado = BUSCA_NAO_ENCONTROU;

    // Tenta encontrar um caminho explorando cada opção
    for (int i = 0; i < quantidade; i++) {
        if (tempoEsgotado(inicio, tempoMaximo)) {
            estado = BUSCA_TEMPO;
            break;
        }

        int proximo = opcoes[i]->destino;
        int64_t peso = opcoes[i]->peso;

        // Evita estouro no cálculo do custo acumulado
        if (caminho->custo > INT64_MAX - peso)
            continue;

        visitado[proximo] = 1;

        // Adiciona o próximo vértice ao caminho atual
        int posicao = caminho->tamanho;

        caminho->vertices[posicao] = proximo;
        caminho->pesos[posicao] = peso;
        caminho->tamanho++;
        caminho->custo += peso;

        // Continua a busca recursivamente a partir do próximo vértice
        estado = dfsAleatoria(
            grafo,
            proximo,
            destino,
            caminho,
            visitado,
            semente,
            chamadasRestantes,
            inicio,
            tempoMaximo
        );

        if (estado == BUSCA_ENCONTROU) {
            free(opcoes);
            return BUSCA_ENCONTROU;
        }

        // Desfaz a escolha para tentar outra possibilidade (backtracking)
        caminho->custo -= peso;
        caminho->tamanho--;
        visitado[proximo] = 0;

        if (estado != BUSCA_NAO_ENCONTROU)
            break;
    }

    free(opcoes);
    return estado;
}

// Inicializa e executa a busca por um caminho aleatório válido
EstadoBusca construirCaminhoAleatorio(
    const Grafo *grafo,
    int origem,
    int destino,
    Caminho *caminho,
    unsigned int *semente,
    int limiteChamadas,
    clock_t inicio,
    double tempoMaximo
) {
    // Verifica se os parâmetros recebidos são válidos
    if (grafo == NULL || caminho == NULL ||
        caminho->vertices == NULL ||
        caminho->pesos == NULL ||
        semente == NULL || limiteChamadas <= 0 ||
        tempoMaximo <= 0.0)
        return BUSCA_ERRO;

    int n = grafo->numVertices;

    if (origem < 1 || origem > n ||
        destino < 1 || destino > n)
        return BUSCA_ERRO;

    // Cria um vetor para controlar os vértices visitados
    int *visitado = calloc(
        (size_t)n + 1, sizeof(int)
    );

    if (visitado == NULL)
        return BUSCA_ERRO;

    // Inicializa o caminho com o vértice de origem
    caminho->tamanho = 1;
    caminho->custo = 0;
    caminho->vertices[0] = origem;
    caminho->pesos[0] = 0;
    visitado[origem] = 1;

    int chamadas = limiteChamadas;

    // Inicia a busca em profundidade aleatória
    EstadoBusca estado = dfsAleatoria(
        grafo, origem, destino,
        caminho, visitado, semente,
        &chamadas, inicio, tempoMaximo
    );

    free(visitado);

    // Limpa o caminho caso a busca não encontre o destino
    if (estado != BUSCA_ENCONTROU) {
        caminho->tamanho = 0;
        caminho->custo = 0;
    }

    return estado;
}

// Gera um caminho vizinho modificando parte do caminho atual
EstadoBusca gerarVizinho(
    const Grafo *grafo,
    const Caminho *atual,
    int destino,
    Caminho *vizinho,
    unsigned int *semente,
    int limiteChamadas,
    clock_t inicio,
    double tempoMaximo
) {
    // Verifica se os parâmetros e caminhos são válidos
    if (grafo == NULL || atual == NULL ||
        vizinho == NULL || semente == NULL ||
        atual->vertices == NULL ||
        atual->pesos == NULL ||
        vizinho->vertices == NULL ||
        vizinho->pesos == NULL ||
        atual->tamanho < 2 ||
        limiteChamadas <= 0)
        return BUSCA_ERRO;

    int n = grafo->numVertices;

    // Sorteia uma posição para cortar o caminho atual
    int corte = sortear(
        semente, atual->tamanho - 1
    );

    int *visitado = calloc(
        (size_t)n + 1, sizeof(int)
    );

    if (visitado == NULL)
        return BUSCA_ERRO;

    vizinho->tamanho = corte + 1;
    vizinho->custo = 0;

    // Copia a parte inicial do caminho até a posição de corte
    memcpy(
        vizinho->vertices,
        atual->vertices,
        (size_t)(corte + 1) * sizeof(int)
    );

    memcpy(
        vizinho->pesos,
        atual->pesos,
        (size_t)(corte + 1) * sizeof(int64_t)
    );

    // Marca os vértices copiados e calcula o custo parcial
    for (int i = 0; i <= corte; i++) {
        int v = vizinho->vertices[i];

        visitado[v] = 1;

        if (vizinho->custo >
            INT64_MAX - vizinho->pesos[i]) {
            free(visitado);
            return BUSCA_ERRO;
        }

        vizinho->custo += vizinho->pesos[i];
    }

    int chamadas = limiteChamadas;

    // Reconstrói o restante do caminho por meio de uma busca aleatória
    EstadoBusca estado = dfsAleatoria(
        grafo,
        vizinho->vertices[corte],
        destino,
        vizinho,
        visitado,
        semente,
        &chamadas,
        inicio,
        tempoMaximo
    );

    free(visitado);

    // Descarta o vizinho caso não seja possível chegar ao destino
    if (estado != BUSCA_ENCONTROU) {
        vizinho->tamanho = 0;
        vizinho->custo = 0;
    }

    return estado;
}
