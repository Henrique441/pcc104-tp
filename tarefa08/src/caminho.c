
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include "caminho.h"
#include "aresta.h"

static unsigned int aleatorio(unsigned int *estado) {
    unsigned int x = *estado;

    if (x == 0)
        x = 2463534242u;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    *estado = x;
    return x;
}

static int sortear(unsigned int *estado, int limite) {
    return (int)(aleatorio(estado) % (unsigned int)limite);
}

static int tempoEsgotado(clock_t inicio, double limite) {
    return (double)(clock() - inicio) / CLOCKS_PER_SEC
           >= limite;
}

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

    if (c.vertices == NULL || c.pesos == NULL) {
        liberarCaminho(&c);
    }

    return c;
}

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

int copiarCaminho(Caminho *destino, const Caminho *origem) {
    if (destino == NULL || origem == NULL ||
        destino->vertices == NULL ||
        destino->pesos == NULL ||
        origem->vertices == NULL ||
        origem->pesos == NULL)
        return 0;

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
    if (tempoEsgotado(inicio, tempoMaximo))
        return BUSCA_TEMPO;

    if (*chamadasRestantes <= 0)
        return BUSCA_LIMITE;

    (*chamadasRestantes)--;

    if (atual == destino)
        return BUSCA_ENCONTROU;

    int grau = 0;

    const Aresta *a =
        grafo->vertices[atual].primeiraAresta;

    while (a != NULL) {
        if (!visitado[a->destino] && a->peso >= 0)
            grau++;

        a = a->proxima;
    }

    if (grau == 0)
        return BUSCA_NAO_ENCONTROU;

    const Aresta **opcoes = malloc(
        (size_t)grau * sizeof(Aresta *)
    );

    if (opcoes == NULL)
        return BUSCA_ERRO;

    int quantidade = 0;

    a = grafo->vertices[atual].primeiraAresta;

    while (a != NULL) {
        if (!visitado[a->destino] && a->peso >= 0)
            opcoes[quantidade++] = a;

        a = a->proxima;
    }

    for (int i = quantidade - 1; i > 0; i--) {
        int j = sortear(semente, i + 1);

        const Aresta *aux = opcoes[i];
        opcoes[i] = opcoes[j];
        opcoes[j] = aux;
    }

    EstadoBusca estado = BUSCA_NAO_ENCONTROU;

    for (int i = 0; i < quantidade; i++) {
        if (tempoEsgotado(inicio, tempoMaximo)) {
            estado = BUSCA_TEMPO;
            break;
        }

        int proximo = opcoes[i]->destino;
        int64_t peso = opcoes[i]->peso;

        if (caminho->custo > INT64_MAX - peso)
            continue;

        visitado[proximo] = 1;

        int posicao = caminho->tamanho;

        caminho->vertices[posicao] = proximo;
        caminho->pesos[posicao] = peso;
        caminho->tamanho++;
        caminho->custo += peso;

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

        caminho->custo -= peso;
        caminho->tamanho--;
        visitado[proximo] = 0;

        if (estado != BUSCA_NAO_ENCONTROU)
            break;
    }

    free(opcoes);
    return estado;
}

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

    int *visitado = calloc(
        (size_t)n + 1, sizeof(int)
    );

    if (visitado == NULL)
        return BUSCA_ERRO;

    caminho->tamanho = 1;
    caminho->custo = 0;
    caminho->vertices[0] = origem;
    caminho->pesos[0] = 0;
    visitado[origem] = 1;

    int chamadas = limiteChamadas;

    EstadoBusca estado = dfsAleatoria(
        grafo, origem, destino,
        caminho, visitado, semente,
        &chamadas, inicio, tempoMaximo
    );

    free(visitado);

    if (estado != BUSCA_ENCONTROU) {
        caminho->tamanho = 0;
        caminho->custo = 0;
    }

    return estado;
}

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

    if (estado != BUSCA_ENCONTROU) {
        vizinho->tamanho = 0;
        vizinho->custo = 0;
    }

    return estado;
}
