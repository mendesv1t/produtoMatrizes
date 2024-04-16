#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"

// estrutura auxiliar para montar resultados para um csv:
typedef struct {
    float aceleracao;
    float eficiencia;
    float mediaTempoExecucao;
    int sequencial; // se for 0 é sequencial, se for 1 é concorrente.
    int linhasMatriz;
    int colunasMatriz;
    int qtdThreads;
} Experimento;

// estrutura auxiliar para leitura e montagem de matrizes:
typedef struct {
    float * matriz;
    int colunas;
    int linhas;
} Matriz;

//método para extrair resultados para análise em um csv, dada uma lista de experimentos
void extrairCsv(Experimento * experimentos[], int qtd) {

    FILE * resultados;
    resultados = fopen("resultados.csv", "w+");

    if (!resultados) {
        fprintf(stderr, "Erro ao criar arquivo csv\n");
        return;
    }
    // fixa header do csv:
    fprintf(resultados, "modo;aceleracao;eficiencia;tempoExecucao;linhasMatriz;colunasMatriz;qtdThreads\n");
    for (int i = 0; i<qtd; i++) {
        //escreve os experimento resultantes do experimento em uma linha:
        fprintf(resultados,"%s;%f;%f;%f;%d;%d;%d\n", experimentos[i]->sequencial == 0 ? "sequencial" : "concorrente", experimentos[i]->aceleracao, experimentos[i]->eficiencia,
                experimentos[i]->mediaTempoExecucao, experimentos[i]->linhasMatriz, experimentos[i]->colunasMatriz, experimentos[i]->qtdThreads);
    }
    fclose(resultados);
}

// método que realiza o produto de forma sequencial de matrizes de NxM dimensões:
float * produtoMatrizes(Matriz * matrizA, Matriz * matrizB) {
    float * retorno = malloc(sizeof (float)*matrizA->linhas*matrizB->colunas);

    if (matrizA->colunas != matrizB->linhas) {
        printf("Erro: número de colunas da matriz A não é igual ao número de linhas da matriz B.");
        return NULL;
    }

    for (int i = 0; i < matrizA->linhas; i++) {
        for (int j = 0; j < matrizB->colunas; j++) {
            retorno[i * matrizB->colunas + j] = 0;
            for (int k = 0; k < matrizA->colunas; k++) {
                retorno[i * matrizB->colunas + j] += matrizA->matriz[i * matrizA->colunas + k] * matrizB->matriz[k * matrizB->colunas + j];
            }
        }
    }
    return retorno;
}


int main(int argc, char*argv[]) {

    //declara estruturas para as matrizes de entrada:
    Matriz * matrizA = malloc(sizeof(Matriz));
    Matriz * matrizB = malloc(sizeof(Matriz));

    if (!matrizA) {
        fprintf(stderr, "Erro ao alocar memória para a matriz A\n");
        free(matrizA);
        return 1;
    }
    if (!matrizB) {
        fprintf(stderr, "Erro ao alocar memória para a matriz B\n");
        free(matrizA);
        free(matrizB);
        return 2;
    }

    // arquivos de entrada das matrizes:
    FILE * arquivoMatrizA;
    FILE * arquivoMatrizB;

    //recebe os argumentos de entrada
    if(argc < 3) {
        fprintf(stderr, "Digite: ./main <ArquivoMatrizA> <ArquivoMatrizB> <nomeArquivoSaida>\n");
        return 3;
    }

    //abre o arquivo para leitura binaria
    arquivoMatrizA = fopen(argv[1], "rb");
    arquivoMatrizB = fopen(argv[2], "rb");
    char * nomeArquivoSaida = argv[3];

    // retorna ponteiros que apontam para as matrizes lidas nos arquivos de entrada
    * matrizA = leMatrizArquivo(arquivoMatrizA);
    * matrizB = leMatrizArquivo(arquivoMatrizB);

    Matriz * matrizSequencial = malloc(sizeof (Matriz));


    if (matrizSequencial == NULL) {
        fprintf(stderr,"Erro ao alocar memória para a matriz resultante.");
        return 4;
    }

    double inicio, fim, tempoTotal;

    GET_TIME(inicio);
    matrizSequencial->matriz = produtoMatrizes(matrizA, matrizB);
    GET_TIME(fim);
    tempoTotal = fim - inicio;

    if (matrizSequencial->matriz != NULL) {
        printf("Tempo decorrido (Sequencial): %f segundos\nProcessamento de uma matriz de %d linhas e %d colunas\n", tempoTotal, matrizA->linhas, matrizB->colunas);
        matrizSequencial->linhas = matrizA->linhas;
        matrizSequencial->colunas = matrizB->colunas;
        escreveMatrizArquivo(matrizSequencial,nomeArquivoSaida);
    }

    //finaliza o uso das variaveis
    fclose(arquivoMatrizA);
    fclose(arquivoMatrizB);
    free(matrizA);
    free(matrizB);
    free(matrizSequencial);

    return 0;
}
