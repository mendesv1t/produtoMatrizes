#include <stdio.h>
#include <stdlib.h>

float *matrizC;

void * produtoMatrizes() {
    return NULL;
}

void montaMatrizResultante(int i,float * matrizA, float * matrizB, int colunasA, int colunasB, int linhasA) {
    for (int j = 0; j < colunasB; j++) {
        matrizC[i * colunasB + j] = 0;
        for (int k = 0; k < colunasA; k++) {
            matrizC[i * colunasB + j] += matrizA[i * linhasA + k] * matrizB[k * colunasB + j];
        }
        printf("%f ", matrizC[i * colunasB + j]);
    }
}

float * produtoMatrizesSequencial(float * matrizA, float * matrizB, int colunasA, int linhasA, int colunasB, int linhasB) {
    if (colunasA != linhasB) {
        printf("Erro: número de colunas da matriz A não é igual ao número de linhas da matriz B.");
        return NULL;
    }

    matrizC = (float *) malloc(sizeof(float) * linhasA * colunasB);
    if (matrizC == NULL) {
        printf("Erro ao alocar memória para a matriz C.");
        return NULL;
    }

    for (int i = 0; i < linhasA; i++) {
        montaMatrizResultante(i,matrizA,matrizB,colunasA,colunasB,linhasA);
        printf("\n");
    }
    return matrizC;
}


