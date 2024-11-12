#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MaxLinha 100

// Estrutura para armazenar informações pertinentes de cada thread
typedef struct {
    int id;
    FILE** arquivos;   // Vetor de ponteiros para os arquivos a serem lidos
    int* valores;      // Vetor para armazenar valores lidos dos arquivos
    int numValores;    // Quantidade de valores lidos
    int numArquivos;   // Número total de arquivos
} ThreadData;

void mergeSort(int *vetor, int posicaoInicio, int posicaoFim) {
    int i, j, k, metadeTamanho, *vetorTemp;
    if (posicaoInicio == posicaoFim) return;
    metadeTamanho = (posicaoInicio + posicaoFim) / 2;

    mergeSort(vetor, posicaoInicio, metadeTamanho);
    mergeSort(vetor, metadeTamanho + 1, posicaoFim);

    i = posicaoInicio;
    j = metadeTamanho + 1;
    k = 0;
    vetorTemp = (int *)malloc(sizeof(int) * (posicaoFim - posicaoInicio + 1));

    if (vetorTemp == NULL) {
        printf("Erro ao alocar o vetor temporário!\n");
    }

    while (i < metadeTamanho + 1 || j < posicaoFim + 1) {
        if (i == metadeTamanho + 1) { 
            vetorTemp[k] = vetor[j];
            j++;
            k++;
        } else {
            if (j == posicaoFim + 1) {
                vetorTemp[k] = vetor[i];
                i++;
                k++;
            } else {
                if (vetor[i] < vetor[j]) {
                    vetorTemp[k] = vetor[i];
                    i++;
                    k++;
                } else {
                    vetorTemp[k] = vetor[j];
                    j++;
                    k++;
                }
            }
        }
    }
    for (i = posicaoInicio; i <= posicaoFim; i++) {
        vetor[i] = vetorTemp[i - posicaoInicio];
    }
    free(vetorTemp);
}

pthread_mutex_t mutexIndice = PTHREAD_MUTEX_INITIALIZER;
int indiceArquivoAtual = 0;  // Índice compartilhado para rastrear o próximo arquivo

// Função que será executada por cada thread
void* threadFunction(void* arg) {
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

        data->valores = (int*)malloc(linhas * sizeof(int));
        if (data->valores == NULL) {
            printf("Erro ao alocar memória para os valores da thread %d.\n", data->id);
            continue;
        }
        data->numValores = linhas;

        rewind(arquivo);

        int index = 0;
        while (fgets(linha, MaxLinha, arquivo) != NULL) {
            data->valores[index++] = atoi(linha);
        }

        mergeSort(data->valores, 0, data->numValores - 1);

        printf("\n\nThread %d leu %d valores do arquivo %d.", data->id, data->numValores, arquivoAtual);
        printf("\n\n");
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Entrada inválida! Siga este padrão: %s <quantidade_threads> <arquivo1> <arquivo2>...\n", argv[0]);
        return 1;
    }

    int numThreads = atoi(argv[1]);
    int numArquivos = argc - 2;

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

    // Declaracao das threads
    for (int i = 0; i < numThreads; i++) {
        threadData[i].id = i;
        threadData[i].arquivos = arquivos;
        threadData[i].numArquivos = numArquivos;
        threadData[i].valores = NULL; 
        threadData[i].numValores = 0;

        int result = pthread_create(&threads[i], NULL, threadFunction, &threadData[i]);
        if (result) { // Se result retornar 1, entao ocorreu um erro ao criar as threads
            printf("Erro ao criar thread %d\n", i);
            for (int j = 0; j <= i; j++) fclose(arquivos[j]); // Fecha arquivos abertos
            free(arquivos);
            return 1;
        }
    }

    int* todosValores = NULL;
    int totalValores = 0;

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
            
        int novosValores = threadData[i].numValores;
        int posInicial = totalValores;
        totalValores += novosValores;

        /* Realoca 'todosValores' afim de aumentar o espaco com o novo tamanho do
         vetor que esta sendo concatenado */
        todosValores = (int*)realloc(todosValores, totalValores * sizeof(int)); 

        if (todosValores == NULL) {
            printf("Erro ao realocar memória para concatenar vetores\n");
            return 1;
        }


        for (int j = 0; j < novosValores; j++) {
            /* Acessa o ultimo ponto do vetor ('pos_inicial' atual + iteracao atual ('j'))
             e armazena o proximo numero lido da thread */
            todosValores[posInicial + j] = threadData[i].valores[j]; 
        }

        free(threadData[i].valores);
        threadData[i].valores = NULL;
    }


    mergeSort(todosValores, 0, totalValores - 1); // Roda novamente o algoritmo de organizacao, agora com todos os valores juntos

    printf("Vetor final ordenado:\n");
    for (int i = 0; i < totalValores; i++) {
        printf("%d ", todosValores[i]);
    }
    printf("\n");

    free(todosValores);

    for (int i = 0; i < numArquivos; i++) {
        fclose(arquivos[i]);
    }
    free(arquivos);

    return 0;
}
