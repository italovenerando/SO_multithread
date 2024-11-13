// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/threads.h"
#include "../include/sort.h"

int main(int argc, char* argv[]) {
    if (argc < 5 || strcmp(argv[argc - 2], "-o") != 0) {
        printf("Entrada invalida! Siga este padrão: %s <quantidade_threads> <arquivo1> <arquivo2> ... -o <arquivo_saida>\n", argv[0]); // Tratamento para a passagem de paramtreos
        return 1;
    }

    int numThreads = atoi(argv[1]);
    int numArquivos = argc - 4;
    char* arquivoSaida = argv[argc - 1];

    FILE** arquivos = (FILE**)malloc(numArquivos * sizeof(FILE*)); // Aloca um vetor de arquivos com tamanho dinamico
    if (arquivos == NULL) {
        printf("Erro ao alocar memória para os arquivos.\n");
        return 1;
    }

    for (int i = 0; i < numArquivos; i++) {
        arquivos[i] = fopen(argv[i + 2], "r"); // Abre os arquivos no modo de leitura
        if (arquivos[i] == NULL) {
            printf("Erro ao abrir arquivo %s.\n", argv[i + 2]);
            for (int j = 0; j < i; j++) fclose(arquivos[j]);
            free(arquivos);
            return 1;
        }
    }

    // Aloca dinamicamente o vetor de threads do tipo pthread_t
    pthread_t* threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));
    if (threads == NULL) {
        printf("Erro ao alocar memória para as threads.\n");
        for (int i = 0; i < numArquivos; i++) fclose(arquivos[i]);
        free(arquivos);
        return 1;
    }

    // Aloca dinamicamente o vetor de structs ThreadData
    ThreadData* threadData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));
    if (threadData == NULL) {
        printf("Erro ao alocar memória para os dados das threads.\n");
        free(threads);
        for (int i = 0; i < numArquivos; i++) fclose(arquivos[i]);
        free(arquivos);
        return 1;
    }

    /* Ira contabilizar o tempo total da criacao e execucao das threads ate a sua finalizacao */
    struct timespec inicio, fim;
    double tempo = 0;
    clock_gettime(CLOCK_MONOTONIC, &inicio); // Inicia a contagem da execucao total das threads
    

    // Declaracao das threads
    for (int i = 0; i < numThreads; i++) {
        threadData[i].id = i;
        threadData[i].arquivos = arquivos;
        threadData[i].numArquivos = numArquivos;
        int result = pthread_create(&threads[i], NULL, mergeSortThreads, &threadData[i]);

        if (result) { // Se result retornar um valor não zero, ocorreu um erro
            printf("Erro ao criar thread %d\n", i);
            for (int j = 0; j <= i; j++) fclose(arquivos[j]); // Fecha arquivos abertos
            free(arquivos);
            free(threads);
            free(threadData);
            return 1; 
        }
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL); // Finaliza o conjunto das threads 
    }

    clock_gettime(CLOCK_MONOTONIC, &fim); // Finaliza a contagem da execucao total das threads
    tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9; // Soma os segundos e nano segundos totais
    printf("\nTempo total de execução: %f segundos.\n", tempo);

    mergeSort(todosValores, 0, totalValores - 1);

    FILE* saida = fopen(arquivoSaida, "w");

    if (saida == NULL) {
        printf("Erro ao abrir o arquivo de saída %s.\n", arquivoSaida);
        free(todosValores);
        for (int i = 0; i < numArquivos; i++) {
            fclose(arquivos[i]);
        }
        free(arquivos);
        free(threads);
        free(threadData);
        return 1;
    }

    for (int i = 0; i < totalValores; i++) {
        fprintf(saida, "%d\n", todosValores[i]); // Escreve os valores armazenados em todosValores no arquivo de saida
    }

    free(todosValores);

    for (int i = 0; i < numArquivos; i++) {
        fclose(arquivos[i]);
    }
    free(arquivos);
    free(threads);
    free(threadData); // Libera a memória alocada para o vetor de dados das threads

    fclose(saida);
    
    return 0;
}