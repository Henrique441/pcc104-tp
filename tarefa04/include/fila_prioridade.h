
#ifndef FILA_PRIORIDADE_H
#define FILA_PRIORIDADE_H

#include <stdint.h>

//estrutura para representar um elemento do heap
typedef struct {
    int vertice;
    int64_t prioridade;
} ElementoHeap;

//estrutura para representar a fila de prioridade
typedef struct {
    ElementoHeap *elementos;
    int *posicao;
    int tamanho;
    int capacidade;
} FilaPrioridade;

//função para criar uma fila de prioridade com capacidade especificada
//parametros: capacidade - capacidade máxima da fila de prioridade
FilaPrioridade *criarFilaPrioridade(int capacidade);

//função para inserir um elemento na fila de prioridade
//parametros: fila, vertice a ser inserido, prioridade do vertice
int inserir(FilaPrioridade *fila, int vertice, int64_t prioridade);

//função para extrair o elemento com a menor prioridade da fila de prioridade
//parametros: fila
int extrairMinimo(FilaPrioridade *fila);

//função para diminuir a prioridade de um elemento na fila de prioridade
//parametros: fila, vertice a ter a prioridade diminuida, nova prioridade do vertice
int diminuirPrioridade(FilaPrioridade *fila, int vertice, int64_t novaPrioridade);

//função para verificar se a fila de prioridade está vazia
int filaVazia(const FilaPrioridade *fila);

//função para destruir a fila de prioridade e liberar a memória alocada
void destruirFilaPrioridade(FilaPrioridade *fila);

#endif
