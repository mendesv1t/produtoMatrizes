/* Programa que le um arquivo binario com dois valores inteiros (indicando as dimensoes de uma matriz) 
 * e uma sequencia com os valores da matriz (em float)
 * Entrada: nome do arquivo de entrada
 * Saida: valores da matriz escritos no formato texto (com 6 casas decimais) na saida padrao 
 * */

#include<stdio.h>
#include<stdlib.h>

int main(int argc, char*argv[]) {

   float *matrizA; //matriz que será carregada do arquivo
   float *matrizB; //matriz que será carregada do arquivo

   int linhasA, colunasA; //dimensoes da matriz

   int linhasB, colunasB; //dimensoes da matriz

   long long int tamA; //qtde de elementos na matriz
   long long int tamB; //qtde de elementos na matriz

   FILE * arquivoMatrizA; //descritor do arquivo de entrada

   FILE * arquivoMatrizB; //descritor do arquivo de entrada

   size_t dimensaoA; //retorno da funcao de leitura no arquivo de entrada
    size_t dimensaoB;
   //recebe os argumentos de entrada
   if(argc < 3) {
      fprintf(stderr, "Digite: ./leMatrizBinario <matrizA> <matrizB>\n");
      return 1;
   }
   
   //abre o arquivo para leitura binaria
   arquivoMatrizA = fopen(argv[1], "rb");
    arquivoMatrizB = fopen(argv[2], "rb");
   if(!arquivoMatrizA || !arquivoMatrizB) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
   }

   //le as dimensoes da matriz
   dimensaoA = fread(&linhasA, sizeof(int), 1, arquivoMatrizA);
   //le as dimensoes da matriz
   dimensaoB = fread(&linhasB, sizeof(int), 1, arquivoMatrizB);
   if(!dimensaoA || !dimensaoB) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   dimensaoA = fread(&colunasA, sizeof(int), 1, arquivoMatrizA);
   dimensaoB = fread(&colunasB, sizeof(int), 1, arquivoMatrizB);

   if(!dimensaoA || !dimensaoB) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   tamA = linhasA * colunasA; //calcula a qtde de elementos da matriz
   tamB = linhasB * colunasB; //calcula a qtde de elementos da matriz

   //aloca memoria para a matriz
   matrizA = (float*) malloc(sizeof(float) * tamA);
   matrizB = (float*) malloc(sizeof(float) * tamB);

   if(!matrizA || !matrizB) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return 3;
   }

   //carrega a matriz de elementos do tipo float do arquivo
   dimensaoA = fread(matrizA, sizeof(float), tamA, arquivoMatrizA);
   //carrega a matriz de elementos do tipo float do arquivo
   dimensaoB = fread(matrizB, sizeof(float), tamB, arquivoMatrizB);
   if(dimensaoA < tamA || dimensaoB < tamB) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      return 4;
   }

   //imprime a matriz na saida padrao
    printf("------------------- Matriz A -------------------\n");
   for(int i=0; i<linhasA; i++) {
      for(int j=0; j<colunasA; j++)
        fprintf(stdout, "%.6f ", matrizA[i*colunasA+j]);
      fprintf(stdout, "\n");
   }

   //imprime a matriz na saida padrao
    printf("------------------- Matriz B -------------------\n");
   for(int i=0; i<linhasB; i++) {
      for(int j=0; j<colunasB; j++)
        fprintf(stdout, "%.6f ", matrizB[i*colunasB+j]);
      fprintf(stdout, "\n");
   }

   //finaliza o uso das variaveis
   fclose(arquivoMatrizA);
   free(matrizA);
   //finaliza o uso das variaveis
   fclose(arquivoMatrizB);
   free(matrizB);
   return 0;
}

