#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

// ------------------------- DECLARAÇOES AUXILIARES (métodos & variáveis globais) -------------------------

// matriz resultante global do programa:
float * matrizC = NULL;

// estrutura auxiliar para montar resultados para um csv:
typedef struct {
    double aceleracao;
    double eficiencia;
    double tempoExecucao;
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

// estrutura auxiliar para enviar experimento as threads:
typedef struct {
    Matriz matrizA;
    Matriz matrizB;
    int lote;
    int M;
    int id;
} tArgs;

//método para extrair resultados para análise em um csv, dada uma lista de experimentos
void extrairCsv(Experimento * experimento, char * nomeArquivo) {

    FILE * resultados;
    resultados = fopen(nomeArquivo, "r");

    if (!resultados) {
        resultados = fopen(nomeArquivo, "a");
        fprintf(resultados, "aceleracao;eficiencia;tempoExecucao;linhasMatriz;colunasMatriz;qtdThreads\n");
        //escreve os experimento resultantes do experimento em uma linha:
        fprintf(resultados,"%f;%f;%f;%d;%d;%d\n", experimento->aceleracao, experimento->eficiencia,
                    experimento->tempoExecucao, experimento->linhasMatriz, experimento->colunasMatriz, experimento->qtdThreads);
    } else {
        resultados = fopen(nomeArquivo, "a");
        //escreve os experimento resultantes do experimento em uma linha:
        fprintf(resultados,"%f;%f;%f;%d;%d;%d\n", experimento->aceleracao, experimento->eficiencia,
                    experimento->tempoExecucao, experimento->linhasMatriz, experimento->colunasMatriz, experimento->qtdThreads);
        }

    if (!resultados) {
        fprintf(stderr, "Erro ao criar arquivo csv\n");
        return;
    }
    // fixa header do csv:

    fclose(resultados);
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

// método que lê um arquivo e devolve uma estrutura que contém uma matriz e experimento relevantes sobre ela:
Matriz leMatrizArquivo(FILE * file) {

    int linhas;
    int colunas;
    int dimensao;
    float * matrizRetorno;
    int tamanho;
    Matriz matriz;

    if(!file) {
        fprintf(stderr, "Erro na abertura do arquivo\n");
        return matriz;
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
        return matriz;
    }

    //carrega a matriz do arquivo, na matriz de retorno alocada:
    dimensao = fread(matrizRetorno, sizeof(float), tamanho, file);

    if(dimensao < tamanho) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
        return matriz;
    }

    matriz.matriz = matrizRetorno;
    matriz.linhas = linhas;
    matriz.colunas = colunas;

    return matriz;
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

double produtoMatrizesSequencial(Matriz * matrizA, Matriz * matrizB, char * nomeArquivoSaida) {

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

    free(matrizSequencial);

    return tempoTotal;
}

// ------------------------- THREADS (criação de threads & tarefa das threads) -------------------------

// tarefa designada as threads:
void * tarefa(void * arg) {

    tArgs * args = (tArgs *) arg;

    int id = args->id;
    int lote = args->lote;
    int M = args->M;
    int inicio = id*lote;
    int fim = (id + 1)*lote;

    // para garantir casos ímpares chegarem ao fim de todas as linhas:
    if ((id + 1) == M) {
        fim = args->matrizA.linhas;
    }

    for (int i = inicio; i < fim; i++) {
        for (int j = 0; j < args->matrizB.colunas; j++) {
            matrizC[i * args->matrizB.colunas + j] = 0;
            for (int k = 0; k < args->matrizA.colunas; k++) {
                matrizC[i * args->matrizB.colunas + j] += args->matrizA.matriz[i * args->matrizA.colunas + k] * args->matrizB.matriz[k * args->matrizB.colunas + j];
            }
        }
    }

    free(arg);
    pthread_exit(NULL);
}

// criação de threads:
void criarThreads(Matriz matrizA, Matriz matrizB, int M) {

    // recuperando o id das threads no sistema:
    pthread_t tid_sistema[M];
    int threads[M];


    if (matrizA.colunas != matrizB.linhas) {
        printf("Erro: número de colunas da matriz A não é igual ao número de linhas da matriz B.\n");
        return;
    }

    // criando M threads:
    for (int i = 0; i < M; i++) {
        tArgs * args = malloc(sizeof (tArgs));
        threads[i] = i;
        args->id = i;
        args->matrizA = matrizA;
        args->matrizB = matrizB;
        args->M = M;
        args->lote = matrizA.linhas/M;
      if (pthread_create(&tid_sistema[i], NULL, tarefa, args)) {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
       }

    }

    for (int i = 0; i < M; i++) {
        pthread_join(tid_sistema[i],NULL);
    }

}

int geraResultados(Matriz * matrizA, Matriz * matrizB, int M) {

    double inicio, fim;
    double tempoSequencial = produtoMatrizesSequencial(matrizA, matrizB, "matrizSeq");
    double tempoMedioConcorrente = 0;
    int numNucleos = 16;

    for (int i = 0; i<3; i++) {

        GET_TIME(inicio);
        criarThreads(* matrizA, * matrizB, M);
        GET_TIME(fim);

        //extrai tempo médio de processamento
        tempoMedioConcorrente += (fim - inicio)/3;
    }
    printf("Tempo médio decorrido (Concorrente): %f segundos\nProcessamento de uma matriz de %d linhas e %d colunas\n", tempoMedioConcorrente, matrizA->linhas, matrizB->colunas);

    Experimento * exp = malloc(sizeof (Experimento));
    exp->tempoExecucao = tempoMedioConcorrente;
    exp->aceleracao = tempoSequencial/exp->tempoExecucao;
    exp->eficiencia = exp->aceleracao/numNucleos;
    exp->linhasMatriz = matrizA->linhas;
    exp->colunasMatriz = matrizB->colunas;
    exp->qtdThreads = M;
    extrairCsv(exp, "tempoConcorrente.csv");
    free(exp);
    return 0;
}

// ------------------------- PROGRAMA PRINCIPAL -------------------------

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

    matrizC = (float *) malloc(sizeof(float) * matrizA->linhas * matrizB->colunas);
    Matriz * matrizAuxConcorrente = malloc(sizeof (Matriz));


    if (matrizC == NULL) {
        fprintf(stderr,"Erro ao alocar memória para a matriz C.");
        free(matrizC);
        return 4;
    }

    int M;
    printf("Entre com o número de threads desejado: ");
    scanf("%d", & M);

    if (!M) {
        fprintf(stderr, "Erro ao ler número de threads desejado.\n");
        return 5;
    }

    double inicio, fim, tempoTotal;

    GET_TIME(inicio);
    criarThreads(* matrizA, * matrizB, M);

    if (matrizC[0] != 0) {
        GET_TIME(fim);
        tempoTotal = fim - inicio;
        printf("Tempo decorrido (Concorrente): %f segundos\nProcessamento de uma matriz de %d linhas e %d colunas\n", tempoTotal, matrizA->linhas, matrizB->colunas);
    }

    matrizAuxConcorrente->matriz = matrizC;
    matrizAuxConcorrente->linhas = matrizA->linhas;
    matrizAuxConcorrente->colunas = matrizB->colunas;
    escreveMatrizArquivo(matrizAuxConcorrente,nomeArquivoSaida);

    //finaliza o uso das variaveis
    fclose(arquivoMatrizA);
    fclose(arquivoMatrizB);
    free(matrizA);
    free(matrizB);
    free(matrizC);
    free(matrizAuxConcorrente);

    return 0;
}