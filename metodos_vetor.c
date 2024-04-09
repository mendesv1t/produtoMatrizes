#include <stdio.h>
#include <stdlib.h>
#define N 2
#define M 2
// a matriz resultante tem o número de linhas de um, e numero de colunas do outro!
float *matrizC;

float * produtoMatriz(float * matrizA, float * matrizB) {
    matrizC = (float *) malloc(sizeof(float) * N * M);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            matrizC[i*M + j] = 0;
            for (int k = 0; k < N; k++) {
                matrizC[i*M + j] += matrizA[k*N + j]*matrizB[i*M + k];
            }
            printf("%f ", matrizC[i*M + j]);
        }
        printf("\n");
    }
    free(matrizC);
}

int main(void) {
    float a[4] = {1,2,3,4};
    float b[4] = {1,2,3,4};
    produtoMatriz(&a, &b);

    // [1  2] [1  2]
    // [3  4] [3  4]
    // resultado:
    // [7  10]
    // [15 22]
}