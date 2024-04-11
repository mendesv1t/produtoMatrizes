#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

float *matrizC = NULL;

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
        for (int j = 0; j < colunasB; j++) {
            matrizC[i * colunasB + j] = 0;
            for (int k = 0; k < colunasA; k++) {
                matrizC[i * colunasB + j] += matrizA[i * colunasA + k] * matrizB[k * colunasB + j];
            }
            //printf("%f ", matrizC[i * colunasB + j]);
        }
        printf("\n");
    }
    return matrizC;
}

void processaLoteResultante(int i,float * matrizA, float * matrizB, int colunasA, int colunasB, int linhasA) {
    for (int j = 0; j < colunasB; j++) {
        matrizC[i * colunasB + j] = 0;
        for (int k = 0; k < colunasA; k++) {
            matrizC[i * colunasB + j] += matrizA[i * linhasA + k] * matrizB[k * colunasB + j];
        }
        printf("%f ", matrizC[i * colunasB + j]);
    }
}


void * tarefa() {
    return NULL;
}


void criarThreads(int M, int N) {

    // recuperando o id das threads no sistema:
    pthread_t tid_sistema[M];
    int threads[M];

    // criando M threads:
    for (int i = 1; i <= M; i++) {

        threads[i] = i;

        if (pthread_create(&tid_sistema[i], NULL, tarefa(), NULL)) {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    for (int i = 1; i <= M; i++) {
        pthread_join(tid_sistema[i],NULL);
    }

}

void escreveMatrizArquivo( float * matriz,char * nome, int linhas, int colunas) {

    size_t ret;
    long long int tam = linhas*colunas;
    //escreve a matriz no arquivo
    //abre o arquivo para escrita binaria
    FILE * matrizArquivo = fopen(nome, "wb");
    if(!matrizArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return;
    }
    //escreve numero de linhas e de colunas
    ret = fwrite(&linhas, sizeof(int), 1, matrizArquivo);
    ret = fwrite(&colunas, sizeof(int), 1, matrizArquivo);

    //escreve os elementos da matriz
    ret = fwrite(matriz, sizeof(float), tam, matrizArquivo);

    if(ret < tam) {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return;
    }

    //finaliza o uso das variaveis
    fclose(matrizArquivo);
}


int main(int argc, char*argv[]) {

/*toDo:
     * - criar método sequencial de produto de matrizes (feito)
     * - ler matrizes A e B de um arquivo
     * - escrever a matriz C em um arquivo
     * - criar tarefa das threads
     * - criar threads
     * - estimar tempo se fosse realizado com um código sequencial para entradas muito grandes e extrair a matriz de saída
     * - estimar tempo de concorrência, extrair a matriz resultado da saída.
     * - realizar diff <matrizSequ> <matrizConc>, e verificar se são iguais.
     * - Para as entradas de dimensoes:  500X500, 1000X1000 e 2000X2000. Repetir 3 vezes cada e realizar a média de tempo.
     * - estimar aceleração e eficiência com 1, 2, 4 e 8 threads
     */


    // declara as matrizes de entrada:
    float *matrizA;
    float *matrizB;

    // declara as linhas e colunas das matrizes:
    int linhasA, colunasA;
    int linhasB, colunasB;

    // quantidade de elementos da matriz
    long long int tamA;
    long long int tamB;

    // arquivos de entrada das matrizes:
    FILE * arquivoMatrizA;
    FILE * arquivoMatrizB;

    size_t dimensaoA; //retorno da funcao de leitura no arquivo de entrada
    size_t dimensaoB;

    //recebe os argumentos de entrada
    if(argc < 3) {
        fprintf(stderr, "Digite: ./leMatrizBinario <ArquivoMatrizA> <ArquivoMatrizB>\n");
        return 1;
    }

    //abre o arquivo para leitura binaria
    arquivoMatrizA = fopen(argv[1], "rb");
    arquivoMatrizB = fopen(argv[2], "rb");

    if(!arquivoMatrizA || !arquivoMatrizB) {
        fprintf(stderr, "Erro na abertura dos arquivos\n");
        return 2;
    }

    //le as dimensoes da matriz
    dimensaoA = fread(&linhasA, sizeof(int), 1, arquivoMatrizA);
    dimensaoB = fread(&linhasB, sizeof(int), 1, arquivoMatrizB);

    if(!dimensaoA || !dimensaoB) {
        fprintf(stderr, "Erro de leitura das dimensoes das matrizes no arquivo \n");
        return 3;
    }
    dimensaoA = fread(&colunasA, sizeof(int), 1, arquivoMatrizA);
    dimensaoB = fread(&colunasB, sizeof(int), 1, arquivoMatrizB);

    if(!dimensaoA || !dimensaoB) {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 3;
    }

    tamA = linhasA * colunasA;
    tamB = linhasB * colunasB;

    //aloca memoria para as matrizes
    matrizA = (float*) malloc(sizeof(float) * tamA);
    matrizB = (float*) malloc(sizeof(float) * tamB);

    if(!matrizA || !matrizB) {
        fprintf(stderr, "Erro de alocao da memoria da matriz\n");
        return 3;
    }

    //carrega as matrizes:
    dimensaoA = fread(matrizA, sizeof(float), tamA, arquivoMatrizA);
    dimensaoB = fread(matrizB, sizeof(float), tamB, arquivoMatrizB);

    if(dimensaoA < tamA || dimensaoB < tamB) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
        return 4;
    }

    float * matrizC = produtoMatrizesSequencial(matrizA, matrizB, colunasA, linhasA, colunasB, linhasB);

    //imprime a matriz na saida padrao
    printf("------------------- Matriz A -------------------\n");
    for(int i=0; i<linhasA; i++) {
        for(int j=0; j<colunasA; j++)
            fprintf(stdout, "%.6f ", matrizA[i*colunasB+j]);
        fprintf(stdout, "\n");
    }
    //imprime a matriz na saida padrao
    printf("------------------- Matriz B -------------------\n");
    for(int i=0; i<linhasB; i++) {
        for(int j=0; j<colunasB; j++)
            fprintf(stdout, "%.6f ", matrizB[i*colunasB+j]);
        fprintf(stdout, "\n");
    }
    //imprime a matriz na saida padrao
    printf("------------------- Matriz C -------------------\n");
    for(int i=0; i<linhasA; i++) {
        for(int j=0; j<colunasB; j++)
            fprintf(stdout, "%.6f ", matrizC[i*colunasB+j]);
        fprintf(stdout, "\n");
    }

    escreveMatrizArquivo(matrizC,"matrizC", linhasA, colunasB);

    //finaliza o uso das variaveis
    fclose(arquivoMatrizA);
    free(matrizA);
    //finaliza o uso das variaveis
    fclose(arquivoMatrizB);
    free(matrizB);


    if (matrizC != NULL) {
        free(matrizC);
    }


    return 0;

}