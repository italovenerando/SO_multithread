// threads.c
#include "../include/threads.h"
#include "../include/sort.h"

pthread_mutex_t mutexIndice = PTHREAD_MUTEX_INITIALIZER; // Usada para proteger o acesso de cada arquivo pelas threads
pthread_mutex_t mutexValores = PTHREAD_MUTEX_INITIALIZER; // Usada para proteger o acesso da variavel de valor pelas threads

int* todosValores = NULL; // vetor para armazenar todos os valores lidos dos arquivos
int totalValores = 0; // quantidade total de valores lidos
int indiceArquivoAtual = 0; // controla o índice dos arquivos a serem acessados

void* mergeSortThreads(void* arg) {
    /* Ira contabilizar o tempo de processamento de cada thread */
    struct timespec inicioOrdenacao, fimOrdenacao;
    double tempoOrdenacao = 0;
    clock_gettime(CLOCK_MONOTONIC, &inicioOrdenacao);

    ThreadData* data = (ThreadData*)arg;
    char linha[MaxLinha];

    while (1) {
        /* Controle para cada thread receber seu arquivo de forma orgaizada */
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
            linhas++; // Conta quantas linhas ha no arquivo atual
        }

        int* valoresLocais = (int*)malloc(linhas * sizeof(int)); // Aloca valoresLocais conforme tamanho do arquivo
        if (valoresLocais == NULL) {
            printf("Erro ao alocar memoria para os valores da thread %d.\n", data->id);
            fclose(arquivo);
            continue;
        }

        rewind(arquivo); // Volta para o inicio do arquivo para le-lo novamente

        int index = 0;
        while (fgets(linha, MaxLinha, arquivo) != NULL) {
            valoresLocais[index++] = atoi(linha); // Le cada linha, tranforma o valor para inteiro e guarda no vetor
        }

        mergeSort(valoresLocais, 0, linhas - 1); // Executa o algoritmo sort no vetor

        /* Controle para cada thread ler o vetor valoresLocais e concatenar no vetor final principal */
        pthread_mutex_lock(&mutexValores);
        int posInicial = totalValores;
        totalValores += linhas;
        
        /* Realoca 'todosValores' afim de aumentar o espaco com o novo tamanho do
           vetor que esta sendo concatenado */
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
    tempoOrdenacao = (fimOrdenacao.tv_sec - inicioOrdenacao.tv_sec) + (fimOrdenacao.tv_nsec - inicioOrdenacao.tv_nsec) / 1e9; // Conta o total do tempo para a thread atual e imprime
    printf("Tempo de processamento da thread %d: %f segundos\n", data->id, tempoOrdenacao);

    pthread_exit(NULL);
}
