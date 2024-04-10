#include <stdio.h>
#include <stdlib.h>

float *matrizC;

void * produtoMatrizes() {
    return NULL;
}

float * produtoMatrizesSequencial(float * matrizA, float * matrizB, int colunasA, int linhasB) {

    if (colunasA != linhasB) {
        printf("Erro: número de colunas da matriz A não é igual ao número de linhas da matriz B.");
        return NULL;
    }

    matrizC = (float *) malloc(sizeof(float) * colunasA * linhasB);
    for (int i = 0; i < colunasA; i++) {
        for (int j = 0; j < linhasB; j++) {
            matrizC[i*linhasB + j] = 0;
            for (int k = 0; k < colunasA; k++) {
                matrizC[i*linhasB + j] += matrizA[k*colunasA + j]*matrizB[i*linhasB + k];
            }
            printf("%f ", matrizC[i*linhasB + j]);
        }
        printf("\n");
    }
    return matrizC;
}

