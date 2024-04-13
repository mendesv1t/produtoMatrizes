#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// ------------------------- DECLARAÇOES AUXILIARES (métodos & variáveis globais) -------------------------

// matriz resultante global do programa:
float * matrizC;

// estrutura auxiliar para montar resultados para um csv:
typedef struct {
    double aceleracao;
    double eficiencia;
    double tempoExecucao;
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

// estrutura auxiliar para enviar experimento as threads:
typedef struct {
    Matriz matrizA;
    Matriz matrizB;
    int lote;
    int M;
    int id;
} tArgs;

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
                                                     experimentos[i]->tempoExecucao, experimentos[i]->linhasMatriz, experimentos[i]->colunasMatriz, experimentos[i]->qtdThreads);
    }
    fclose(resultados);
}

// método que realiza o produto de forma sequencial de matrizes de NxM dimensões:
float * produtoMatrizes(Matriz * matrizA, Matriz * matrizB) {

    if (matrizA->colunas != matrizB->linhas) {
        printf("Erro: número de colunas da matriz A não é igual ao número de linhas da matriz B.");
        return NULL;
    }

    for (int i = 0; i < matrizA->linhas; i++) {
        for (int j = 0; j < matrizB->colunas; j++) {
            matrizC[i * matrizB->colunas + j] = 0;
            for (int k = 0; k < matrizA->colunas; k++) {
                matrizC[i * matrizB->colunas + j] += matrizA->matriz[i * matrizA->colunas + k] * matrizB->matriz[k * matrizB->colunas + j];
            }
        }
    }
    return matrizC;
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
    }

    //carrega a matriz do arquivo, na matriz de retorno alocada:
    dimensao = fread(matrizRetorno, sizeof(float), tamanho, file);

    if(dimensao < tamanho) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
    }

    matriz.matriz = matrizRetorno;
    matriz.linhas = linhas;
    matriz.colunas = colunas;

    return matriz;
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

    if (args->matrizA.colunas != args->matrizB.linhas) {
        printf("Erro: número de colunas da matriz A não é igual ao número de linhas da matriz B.");
        return NULL;
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

    for (int i = 0; i <= M; i++) {
        pthread_join(tid_sistema[i],NULL);
    }

}

// ------------------------- PROGRAMA PRINCIPAL -------------------------

int main(int argc, char*argv[]) {

    //declara estruturas para as matrizes de entrada:
    Matriz * matrizA = malloc(sizeof(Matriz));
    Matriz * matrizB = malloc(sizeof(Matriz));

    // arquivos de entrada das matrizes:
    FILE * arquivoMatrizA;
    FILE * arquivoMatrizB;

    //recebe os argumentos de entrada
    if(argc < 2) {
        fprintf(stderr, "Digite: ./main <ArquivoMatrizA> <ArquivoMatrizB>\n");
        return 1;
    }

    //abre o arquivo para leitura binaria
    arquivoMatrizA = fopen(argv[1], "rb");
    arquivoMatrizB = fopen(argv[2], "rb");

    // retorna ponteiros que apontam para as matrizes lidas nos arquivos de entrada
    * matrizA = leMatrizArquivo(arquivoMatrizA);
    * matrizB = leMatrizArquivo(arquivoMatrizB);

    if (matrizA == NULL || matrizB == NULL) {
        fprintf(stderr,"Erro na leitura dos arquivos de matrizes fornecidos.");
    }

    matrizC = (float *) malloc(sizeof(float) * matrizA->linhas * matrizB->colunas);
    Matriz * matrizAuxConcorrente = malloc(sizeof (Matriz));
    Matriz * matrizAuxSequencial = malloc(sizeof (Matriz));


    if (matrizC == NULL) {
        fprintf(stderr,"Erro ao alocar memória para a matriz C.");
    }

    int M = 0;
    printf("Entre com o número de threads desejado: ");
    scanf("%d", & M);

    criarThreads(* matrizA, * matrizB, M);

    matrizAuxConcorrente->matriz = matrizC;
    matrizAuxConcorrente->linhas = matrizA->linhas;
    matrizAuxConcorrente->colunas = matrizB->colunas;
    escreveMatrizArquivo(matrizAuxConcorrente,"matrizResultanteConcorrente");

    float * matrizResultanteSequencial = malloc(sizeof (float) * matrizA->linhas * matrizB->colunas);
    matrizResultanteSequencial = produtoMatrizes(matrizA, matrizB);
    matrizAuxSequencial->matriz = matrizResultanteSequencial;
    matrizAuxSequencial->linhas = matrizA->linhas;
    matrizAuxSequencial->colunas = matrizB->colunas;
    escreveMatrizArquivo(matrizAuxSequencial,"matrizResultanteSequencial");

    //DadosCsv experimento;
    //extrairCsv(experimento);

    printf("Matriz Concorrente\n");
    for (int i = 0; i< matrizA->linhas; i++) {
        for (int j=0; j<matrizB->colunas; j++) {
            printf("%f ", matrizC[i*matrizB->colunas+j]);
        }
        printf("\n");
    }

    printf("Matriz sequencial\n");
     for (int i = 0; i< matrizA->linhas; i++) {
         for (int j=0; j<matrizB->colunas; j++) {
             printf("%f ", matrizC[i*matrizB->colunas+j]);
         }
         printf("\n");
     }

    //finaliza o uso das variaveis
    fclose(arquivoMatrizA);
    fclose(arquivoMatrizB);
    free(matrizB);
    free(matrizA);
    free(matrizAuxConcorrente);
    free(matrizAuxSequencial);
    free(matrizResultanteSequencial);

    if (matrizC != NULL) {
        free(matrizC);
    }

    return 0;
}