#include "threads.c"
int main(void) {

/*toDo:
     * - criar método sequencial de produto de matrizes (check)
     * - criar tarefa das threads
     * - criar threads
     * - estimar tempo se fosse realizado com um código sequencial para entradas muito grandes e extrair a matriz de saída
     * - estimar tempo de concorrência, extrair a matriz resultado da saída.
     * - realizar diff <matrizSequ> <matrizConc>, e verificar se são iguais.
     * - Para as entradas de dimensoes:  500X500, 1000X1000 e 2000X2000. Repetir 3 vezes cada e realizar a média de tempo.
     * - estimar aceleração e eficiência com 1, 2, 4 e 8 threads
     */


    int linhasA = 3, colunasA = 3;
    int linhasB = 3, colunasB = 1;

    float matrizA[] = {1, 2, 3,3, 4,5,5,6,6};
    float matrizB[] = {1,2,3};

    /*
     * [1 2 3] [1]
     * [3,4 5] [2]
     * [5,6 6] [3]
     */

    matrizC = produtoMatrizesSequencial(matrizA, matrizB, colunasA, linhasA, colunasB, linhasB);
    if (matrizC != NULL) {
        free(matrizC);
    }

}