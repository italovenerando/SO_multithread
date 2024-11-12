// threads.h
#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#define MaxLinha 100

typedef struct {
    int id;
    FILE** arquivos;
    int numArquivos;
} ThreadData;

extern pthread_mutex_t mutexIndice;
extern pthread_mutex_t mutexValores;
extern int* todosValores;
extern int totalValores;
extern int indiceArquivoAtual;

void* mergeSortThreads(void* arg);

#endif
