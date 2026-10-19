#include <stdio.h>
#include "matriz.h"

//garantir o padrao linux
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

int main(void)
{

    #ifdef _WIN32
    if (_setmode(_fileno(stdout), _O_BINARY) == -1)
        return 1;
    #endif

    int n;

    //le a ordem das matrizes
    if (scanf("%d", &n) != 1 || n <= 0) {
        return 1;
    }

    //cria as matrizes
    Matriz *A = createMatriz(n);
    Matriz *B = createMatriz(n);

    //se uma delas for nula apaga as matrizes e termina o codigo
    if (A == NULL || B == NULL) {
        deleteMatriz(A);
        deleteMatriz(B);
        return 1;
    }

    //le os elementos das matrizes
    if (!readMatriz(A) || !readMatriz(B)) {
        deleteMatriz(A);
        deleteMatriz(B);
        return 1;
    }

    //imprime as duas matrizes
    //printf("Matriz A\n");
    //printMatriz(A);
    //printf("Matriz B\n");
    //printMatriz(B);

    Matriz *C = multiplicaMatriz(A, B);

    if (C == NULL) {
        deleteMatriz(A);
        deleteMatriz(B);
        return 1;
    }
    //printf("Soma das matrizes A e B\n");
    printMatriz(C);

    deleteMatriz(C);


    //libera a memoria
    deleteMatriz(A);
    deleteMatriz(B);

    return 0;
}














