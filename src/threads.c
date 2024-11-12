// threads.c
#include "../include/threads.h"
#include "../include/sort.h"

pthread_mutex_t mutexIndice = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexValores = PTHREAD_MUTEX_INITIALIZER;

int* todosValores = NULL;
int totalValores = 0;
int indiceArquivoAtual = 0;

void* mergeSortThreads(void* arg) {
    struct timespec inicioOrdenacao, fimOrdenacao;
    double tempoOrdenacao = 0;
    clock_gettime(CLOCK_MONOTONIC, &inicioOrdenacao);

    ThreadData* data = (ThreadData*)arg;
    char linha[MaxLinha];

    while (1) {
        pthread_mutex_lock(&mutexIndice);
        int arquivoAtual = indiceArquivoAtual;
        indiceArquivoAtual++;
        pthread_mutex_unlock(&mutexIndice);

        if (arquivoAtual >= data->numArquivos) {
            break;
        }

        FILE* arquivo = data->arquivos[arquivoAtual];
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo %d.\n", arquivoAtual);
            continue;
        }

        int linhas = 0;
        while (fgets(linha, MaxLinha, arquivo) != NULL) {
            linhas++;
        }

        int* valoresLocais = (int*)malloc(linhas * sizeof(int));
        if (valoresLocais == NULL) {
            printf("Erro ao alocar memoria para os valores da thread %d.\n", data->id);
            fclose(arquivo);
            continue;
        }

        rewind(arquivo);

        int index = 0;
        while (fgets(linha, MaxLinha, arquivo) != NULL) {
            valoresLocais[index++] = atoi(linha);
        }

        

        mergeSort(valoresLocais, 0, linhas - 1);

        

        pthread_mutex_lock(&mutexValores);
        int posInicial = totalValores;
        totalValores += linhas;

        int* temp = (int*)realloc(todosValores, totalValores * sizeof(int));
        if (temp == NULL) {
            printf("Erro ao realocar memoria para concatenar vetores\n");
            pthread_mutex_unlock(&mutexValores);
            free(valoresLocais);
            fclose(arquivo);
            return NULL;
        }
        todosValores = temp;

        for (int i = 0; i < linhas; i++) {
            todosValores[posInicial + i] = valoresLocais[i];
        }
        pthread_mutex_unlock(&mutexValores);

        free(valoresLocais);
    }

    clock_gettime(CLOCK_MONOTONIC, &fimOrdenacao);
    tempoOrdenacao = (fimOrdenacao.tv_sec - inicioOrdenacao.tv_sec) + (fimOrdenacao.tv_nsec - inicioOrdenacao.tv_nsec) / 1e9;
    printf("Tempo de processamento da thread %d: %f segundos\n", data->id, tempoOrdenacao);

    pthread_exit(NULL);
}