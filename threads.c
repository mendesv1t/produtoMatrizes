#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include "metodos_vetor.c"

void criarThreads(int M, int N) {

    // recuperando o id das threads no sistema:
    pthread_t tid_sistema[M];
    int threads[M];

    // criando M threads:
    for (int i = 1; i <= M; i++) {

        threads[i] = i;

        if (pthread_create(&tid_sistema[i], NULL, produtoMatrizes, NULL)) {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    for (int i = 1; i <= M; i++) {
        pthread_join(tid_sistema[i],NULL);
    }

}
