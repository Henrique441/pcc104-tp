#ifndef MATRIZ_H
#define MATRIZ_H 


//Struct da matriz
typedef struct {
    int n; // tamanho da matriz
    int **dados; // valores da matriz
} Matriz;

//Cria uma matriz e aloca espaco para a matriz na memoria
//Parametro nome do arquivo a ser inserido
Matriz *createMatriz(int n);

//Deleta uma matriz
//Paramtro ponteiro de uma matriz
void deleteMatriz(Matriz *m);

//printa a matriz na tela
//parametro ponteiro de uma matriz m
void printMatriz(const Matriz *m);

//faz a leitura da matriz e retorna por ponteiro
//parametro um ponteiro de matriz m
int readMatriz(Matriz *m);



//Faz a multipicacao das matrizes, const é utilizado para nao alterar as matrizes originais
//parametros matriz a, matriz b
Matriz *multiplicaMatriz(const Matriz *a, const Matriz *b);









#endif


