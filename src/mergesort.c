// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/threads.h"
#include "../include/sort.h"

int main(int argc, char* argv[]) {
    if (argc < 5 || strcmp(argv[argc - 2], "-o") != 0) {
        printf("Entrada invalida! Siga este padrão: %s <quantidade_threads> <arquivo1> <arquivo2> ... -o <arquivo_saida>\n", argv[0]);
        return 1;
    }

    int numThreads = atoi(argv[1]);
    int numArquivos = argc - 4;
    char* arquivoSaida = argv[argc - 1];

    FILE** arquivos = (FILE**)malloc(numArquivos * sizeof(FILE*));
    if (arquivos == NULL) {
        printf("Erro ao alocar memória para os arquivos.\n");
        return 1;
    }

    for (int i = 0; i < numArquivos; i++) {
        arquivos[i] = fopen(argv[i + 2], "r");
        if (arquivos[i] == NULL) {
            printf("Erro ao abrir arquivo %s.\n", argv[i + 2]);
            for (int j = 0; j < i; j++) fclose(arquivos[j]);
            free(arquivos);
            return 1;
        }
    }

    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    struct timespec inicio, fim;
    double tempo = 0;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < numThreads; i++) {
        threadData[i].id = i;
        threadData[i].arquivos = arquivos;
        threadData[i].numArquivos = numArquivos;
        int result = pthread_create(&threads[i], NULL, mergeSortThreads, &threadData[i]);

        if (result) {
            printf("Erro ao criar thread %d\n", i);
            for (int j = 0; j <= i; j++) fclose(arquivos[j]);
            free(arquivos);
            return 1;
        }
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL); // Finaliza o conjunto das threads 
    }

    /* Ira contabilizar o tempo total da criacao e execucao das threads ate a sua finalizacao */
    clock_gettime(CLOCK_MONOTONIC, &fim);
    tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9; 
    printf("\nTempo total de execução: %f segundos.\n", tempo);

    quick(todosValores, 0, totalValores - 1);

    FILE* saida = fopen(arquivoSaida, "w");

    if (saida == NULL) {
        printf("Erro ao abrir o arquivo de saída %s.\n", arquivoSaida);
        free(todosValores);
        for (int i = 0; i < numArquivos; i++) {
            fclose(arquivos[i]);
        }
        free(arquivos);
        return 1;
    }

    for (int i = 0; i < totalValores; i++) {
        fprintf(saida, "%d\n", todosValores[i]);
    }

    free(todosValores);

    for (int i = 0; i < numArquivos; i++) {
        fclose(arquivos[i]);
    }
    free(arquivos);

    fclose(saida);
    
    return 0;
}
