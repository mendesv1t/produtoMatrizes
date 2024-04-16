#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"


// estrutura auxiliar para leitura e montagem de matrizes:
typedef struct {
    float * matriz;
    int colunas;
    int linhas;
} Matriz;


// método que lê um arquivo e devolve uma estrutura que contém uma matriz e experimento relevantes sobre ela:
Matriz * leMatrizArquivo(FILE * file) {

    int linhas;
    int colunas;
    int dimensao;
    float * matrizRetorno;
    int tamanho;
    Matriz * matriz = malloc(sizeof (Matriz));

    if(!file) {
        fprintf(stderr, "Erro na abertura do arquivo\n");
        return NULL;
    }

    dimensao = fread(&linhas, sizeof(int), 1, file);
    dimensao = fread(&colunas, sizeof(int), 1, file);
    tamanho = linhas*colunas;

    if(!dimensao) {
        fprintf(stderr, "Erro de leitura das dimensoes das matrizes no arquivo \n");
    }


    //aloca memoria para a matriz de retorno
    matrizRetorno = (float*) malloc(sizeof(float) * tamanho);

    if(!matrizRetorno) {
        fprintf(stderr, "Erro de alocao da memoria da matriz de retorno\n");
        free(matrizRetorno);
        return NULL;
    }

    //carrega a matriz do arquivo, na matriz de retorno alocada:
    dimensao = fread(matrizRetorno, sizeof(float), tamanho, file);

    if(dimensao < tamanho) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
        return NULL;
    }

    if (!matriz) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
        return NULL;
    }

    matriz->matriz = matrizRetorno;
    matriz->linhas = linhas;
    matriz->colunas = colunas;

    return matriz;
}

// com base nos códigos fornecidos, extraí a logica de escrever matriz em um arquivo para um método:
void escreveMatrizArquivo( Matriz * matriz, char * nome) {

    size_t ret;
    long long int tam = matriz->linhas*matriz->colunas;

    //abre o arquivo para escrita binaria
    FILE * matrizArquivo = fopen(nome, "wb");
    if(!matrizArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return;
    }
    //escreve numero de linhas e de colunas
    ret = fwrite(&matriz->linhas, sizeof(int), 1, matrizArquivo);
    ret = fwrite(&matriz->colunas, sizeof(int), 1, matrizArquivo);

    //escreve os elementos da matriz
    ret = fwrite(matriz->matriz, sizeof(float), tam, matrizArquivo);

    if(ret < tam) {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return;
    }

    //finaliza o uso das variaveis
    fclose(matrizArquivo);
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
    matrizA = leMatrizArquivo(arquivoMatrizA);
    matrizB = leMatrizArquivo(arquivoMatrizB);

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
