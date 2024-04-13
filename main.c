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
    int linhasMatriz;
    int colunasMatriz;
    int qtdThreads;
} DadosCsv;

// estrutura auxiliar para leitura e montagem de matrizes:
typedef struct {
    float * matriz;
    int colunas;
    int linhas;
} Matriz;

// estrutura auxiliar para enviar dados as threads:
typedef struct {
    Matriz matrizA;
    Matriz matrizB;
    int id;
} tArgs;



// método que realiza o produto de forma sequencial de matrizes de NxM dimensões:
float * produtoMatrizes(float * matrizA, float * matrizB, int colunasA, int linhasA, int colunasB, int linhasB) {

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
        }
    }
    return matrizC;
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

    //carrega as matrizes:
    dimensao = fread(matrizRetorno, sizeof(float), tamanho, file);

    if(dimensao < tamanho) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
    }

    matriz.matriz = matrizRetorno;
    matriz.linhas = linhas;
    matriz.colunas = colunas;

    return matriz;
}


//método para extrair resultados em um csv
void extrairCsv(DadosCsv dados) {
    FILE *resultados;
    resultados = fopen("resultados.csv", "w+");

    if (!resultados) {
        fprintf(stderr, "Erro ao criar arquivo\n");
        return;
    }

    // fixa header do csv:
    fprintf(resultados, "aceleracao;eficiencia;tempoExecucao;linhasMatriz;colunasMatriz;qtdThreads\n");

    //escreve os dados resultantes do experimento em uma linha:
    fprintf(resultados,"%f;%f;%f;%d;%d;%d\n", dados.aceleracao, dados.eficiencia, dados.tempoExecucao, dados.linhasMatriz, dados.colunasMatriz, dados.qtdThreads);
    fclose(resultados);
}

// ------------------------- THREADS (criação de threads & tarefa das threads) -------------------------


// tarefa designada as threads:
void * tarefa(void * arg) {

    tArgs * args = (tArgs *) arg;

    if (args->matrizA.colunas != args->matrizB.linhas) {
        printf("Erro: número de colunas da matriz A não é igual ao número de linhas da matriz B.");
        return NULL;
    }

    matrizC = (float *) malloc(sizeof(float) * args->matrizA.linhas * args->matrizB.colunas);
    if (matrizC == NULL) {
        printf("Erro ao alocar memória para a matriz C.");
        return NULL;
    }

    int linhasA = args->matrizA.linhas;
    int colunasA = args->matrizA.colunas;
    int colunasB = args->matrizB.colunas;

    for (int i = 0; i < linhasA; i++) {
        // linhas para threads
        for (int j = 0; j < colunasB; j++) {
            matrizC[i * colunasB + j] = 0;
            for (int k = 0; k < colunasA; k++) {
                matrizC[i * colunasB + j] += args->matrizA.matriz[i * colunasA + k] * args->matrizB.matriz[k * colunasB + j];
            }
        }
    }
    return matrizC;
}

// criação de threads:
void criarThreads(Matriz matrizA, Matriz matrizB, int M) {

    // recuperando o id das threads no sistema:
    pthread_t tid_sistema[M];
    int threads[M];

    tArgs * args = malloc(sizeof (tArgs));
    args->matrizA = matrizA;
    args->matrizB = matrizB;

    // criando M threads:
    for (int i = 0; i <= M; i++) {
        threads[i] = i;
        args->id = i;
      if (pthread_create(&tid_sistema[i], NULL, tarefa, args)) {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
       }
    }

    for (int i = 1; i <= M; i++) {
        pthread_join(tid_sistema[i],NULL);
    }

}


int main(int argc, char*argv[]) {

/*toDo:
     * - criar método sequencial de produto de matrizes (feito)
     * - ler matrizes A e B de um arquivo (feito)
     * - escrever a matriz C em um arquivo (feito)
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
    int linhasA;
    int colunasA;

    float *matrizB;
    int linhasB;
    int colunasB;

    // arquivos de entrada das matrizes:
    FILE * arquivoMatrizA;
    FILE * arquivoMatrizB;


    //recebe os argumentos de entrada
    if(argc < 3) {
        fprintf(stderr, "Digite: ./leMatrizBinario <ArquivoMatrizA> <ArquivoMatrizB>\n");
        return 1;
    }

    //abre o arquivo para leitura binaria
    arquivoMatrizA = fopen(argv[1], "rb");
    arquivoMatrizB = fopen(argv[2], "rb");

    Matriz *matrizAux_A = malloc(sizeof(Matriz));
    Matriz *matrizAux_B = malloc(sizeof(Matriz));

    if (matrizAux_A == NULL || matrizAux_B == NULL) {
        // Tratar erro de alocação de memória
        exit(EXIT_FAILURE);
    }

    * matrizAux_A = leMatrizArquivo(arquivoMatrizA);
    * matrizAux_B = leMatrizArquivo(arquivoMatrizB);


    matrizA = matrizAux_A->matriz;
    linhasA = matrizAux_A->linhas;
    colunasA = matrizAux_A->colunas;

    matrizB = matrizAux_B->matriz;
    linhasB = matrizAux_B->linhas;
    colunasB = matrizAux_B->colunas;

    int M = 0;

    printf("Entre com o número de threads desejado: ");
    scanf("%d", & M);

    //criarThreads(* matrizAux_B, * matrizAux_B, M);

    matrizC = produtoMatrizes(matrizA, matrizB, colunasA, linhasA, colunasB, linhasB);
    escreveMatrizArquivo(matrizC,"matrizResultanteSeq", linhasA, colunasB);
    escreveMatrizArquivo(matrizC,"matrizResultanteConc", linhasA, colunasB);

    DadosCsv dados;
    dados.qtdThreads = 2;
    dados.colunasMatriz = 4;
    dados.linhasMatriz = 4;
    dados.tempoExecucao = 2.9;
    dados.eficiencia = 3.85;
    dados.aceleracao = 29.98;
    extrairCsv(dados);


    for (int i = 0; i< linhasA; i++) {
        for (int j=0; j<colunasB; j++) {
            printf("%f ", matrizC[i*colunasB+j]);
        }
        printf("\n");
    }
    //finaliza o uso das variaveis
    fclose(arquivoMatrizA);
    fclose(arquivoMatrizB);
    free(matrizB);
    free(matrizA);
    free(matrizAux_A);
    free(matrizAux_B);

    if (matrizC != NULL) {
        free(matrizC);
    }


    return 0;

}