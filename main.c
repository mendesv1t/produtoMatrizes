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



//método para extrair resultados para análise em um csv
// ideia inicial: um array de DadosCsv com vários experimentos a fim de calcular no app externo de planilhas
// nisso, pensei em receber um array, iterar sobre ele e cada linha ir adicionando os dados.
void extrairCsv(DadosCsv dados) {
    FILE *resultados;
    resultados = fopen("resultados.csv", "w+");

    if (!resultados) {
        fprintf(stderr, "Erro ao criar arquivo csv\n");
        return;
    }

    // fixa header do csv:
    fprintf(resultados, "aceleracao;eficiencia;tempoExecucao;linhasMatriz;colunasMatriz;qtdThreads\n");

    //escreve os dados resultantes do experimento em uma linha:
    fprintf(resultados,"%f;%f;%f;%d;%d;%d\n", dados.aceleracao, dados.eficiencia, dados.tempoExecucao, dados.linhasMatriz, dados.colunasMatriz, dados.qtdThreads);
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
void escreveMatrizArquivo( float * matriz,char * nome, int linhas, int colunas) {

    size_t ret;
    long long int tam = linhas*colunas;

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

// método que lê um arquivo e devolve uma estrutura que contém uma matriz e dados relevantes sobre ela:
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

    if (args->matrizA.colunas != args->matrizB.linhas) {
        printf("Erro: número de colunas da matriz A não é igual ao número de linhas da matriz B.");
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

// ------------------------- PROGRAMA PRINCIPAL -------------------------

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

    //declara estruturas para as matrizes de entrada:
    Matriz * matrizA = malloc(sizeof(Matriz));
    Matriz * matrizB = malloc(sizeof(Matriz));


    // arquivos de entrada das matrizes:
    FILE * arquivoMatrizA;
    FILE * arquivoMatrizB;


    //recebe os argumentos de entrada
    if(argc < 3) {
        fprintf(stderr, "Digite: ./main <ArquivoMatrizA> <ArquivoMatrizB>\n");
        return 1;
    }

    //abre o arquivo para leitura binaria
    arquivoMatrizA = fopen(argv[1], "rb");
    arquivoMatrizB = fopen(argv[2], "rb");


    // retorna ponteiros que apontam para os dados lidos das matrizes de entrada
    * matrizA = leMatrizArquivo(arquivoMatrizA);
    * matrizB = leMatrizArquivo(arquivoMatrizB);

    if (matrizA == NULL || matrizB == NULL) {
        fprintf(stderr,"Erro na leitura dos arquivos de matrizes fornecidos.");
    }


    int M = 0;

    printf("Entre com o número de threads desejado: ");
    scanf("%d", & M);

    //criarThreads(* matrizAux_B, * matrizAux_B, M);

    matrizC = (float *) malloc(sizeof(float) * matrizA->linhas * matrizB->colunas);


    if (matrizC == NULL) {
        fprintf(stderr,"Erro ao alocar memória para a matriz C.");
    }

    matrizC = produtoMatrizes(matrizA, matrizB);
    escreveMatrizArquivo(matrizC,"matrizResultanteSeq", matrizA->linhas, matrizB->colunas);

    //DadosCsv dados;
    //extrairCsv(dados);


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

    if (matrizC != NULL) {
        free(matrizC);
    }


    return 0;

}