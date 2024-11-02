#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINHA 100

// Estrutura para passar múltiplos argumentos para a thread
typedef struct {
    int id;
    FILE *arquivo;
} ThreadData;

// Função que será executada por cada thread
void* thread_function(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    char linha[MAX_LINHA];

    printf("Thread %d lendo o arquivo...\n", data->id);
    while (fgets(linha, MAX_LINHA, data->arquivo) != NULL) {
        printf("Thread %d leu: %s\n", data->id, linha);
    }
    printf("Thread %d terminou de ler o arquivo.\n", data->id);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: %s <num_threads> <arquivo1> <arquivo2> ... <arquivoN>\n", argv[0]);
        return 1;
    }

    int n_threads = atoi(argv[1]);
    int n_arquivos = argc - 2;

    if (n_threads > n_arquivos) {
        printf("Aviso: há mais threads do que arquivos. Algumas threads ficarão ociosas.\n");
        n_threads = n_arquivos; // Ajusta para o número de arquivos
    }

    // Aloca memória para os ponteiros de arquivos
    FILE** arquivos = (FILE**)malloc(n_arquivos * sizeof(FILE*));
    if (arquivos == NULL) {
        printf("Erro ao alocar memória para arquivos.\n");
        return 1;
    }

    // Abre cada arquivo fornecido como argumento
    for (int i = 0; i < n_arquivos; i++) {
        arquivos[i] = fopen(argv[i + 2], "r");  // Ajusta o índice de argv
        if (arquivos[i] == NULL) {
            printf("Erro ao abrir o arquivo %s.\n", argv[i + 2]);
            for (int j = 0; j < i; j++) fclose(arquivos[j]);  // Fecha arquivos já abertos
            free(arquivos);
            return 1;
        }
    }

    // Declara as threads e a estrutura de dados de cada uma
    pthread_t threads[n_threads];
    ThreadData thread_data[n_threads];

    // Cria as threads, cada uma com um arquivo associado
    for (int i = 0; i < n_threads; i++) {
        thread_data[i].id = i;
        thread_data[i].arquivo = arquivos[i];
        int result = pthread_create(&threads[i], NULL, thread_function, &thread_data[i]);
        if (result) {
            printf("Erro ao criar thread %d\n", i);
            for (int j = 0; j <= i; j++) fclose(arquivos[j]); // Fecha arquivos abertos
            free(arquivos);
            return 1;
        }
    }

    // Aguarda todas as threads terminarem
    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Todas as threads terminaram.\n");

    // Fecha os arquivos e libera a memória
    for (int i = 0; i < n_arquivos; i++) {
        fclose(arquivos[i]);
    }
    free(arquivos);

    return 0;
}
