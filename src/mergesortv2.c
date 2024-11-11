#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINHA 100

// Estrutura para passar múltiplos argumentos para a thread
typedef struct {
    int id;
    FILE *arquivo;
    int* valores;   // Vetor para armazenar os valores lidos
    int n_valores;  // Número de valores lidos
} ThreadData;

void mergeSort(int *vetor, int posicaoInicio, int posicaoFim) {
    int i, j, k, metadeTamanho, *vetorTemp;
    if(posicaoInicio == posicaoFim) return;
    metadeTamanho = (posicaoInicio + posicaoFim ) / 2;

    mergeSort(vetor, posicaoInicio, metadeTamanho);
    mergeSort(vetor, metadeTamanho + 1, posicaoFim);

    i = posicaoInicio;
    j = metadeTamanho + 1;
    k = 0;
    vetorTemp = (int *) malloc(sizeof(int) * (posicaoFim - posicaoInicio + 1));

    while(i < metadeTamanho + 1 || j  < posicaoFim + 1) {
        if (i == metadeTamanho + 1 ) { 
            vetorTemp[k] = vetor[j];
            j++;
            k++;
        }
        else {
            if (j == posicaoFim + 1) {
                vetorTemp[k] = vetor[i];
                i++;
                k++;
            }
            else {
                if (vetor[i] < vetor[j]) {
                    vetorTemp[k] = vetor[i];
                    i++;
                    k++;
                }
                else {
                    vetorTemp[k] = vetor[j];
                    j++;
                    k++;
                }
            }
        }

    }
    for(i = posicaoInicio; i <= posicaoFim; i++) {
        vetor[i] = vetorTemp[i - posicaoInicio];
    }
    free(vetorTemp);
}


// Função que será executada por cada thread
void* thread_function(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    char linha[MAX_LINHA];
    
    // Primeira passagem: conta o número de linhas no arquivo
    data->n_valores = 0;
    while (fgets(linha, MAX_LINHA, data->arquivo) != NULL) {
        data->n_valores++;
    }

    // Aloca memória com base no número total de linhas
    data->valores = (int*)malloc(data->n_valores * sizeof(int));
    if (data->valores == NULL) {
        printf("Erro ao alocar memória para os valores da thread %d.\n", data->id);
        pthread_exit(NULL);
    }

    // Reinicia o arquivo para ler novamente a partir do início
    rewind(data->arquivo);

    // Segunda passagem: lê os valores e armazena no vetor
    int index = 0;
    while (fgets(linha, MAX_LINHA, data->arquivo) != NULL) {
        data->valores[index] = atoi(linha);
        index++;
    }
    mergeSort(data->valores, 0, data->n_valores - 1);

    printf("Thread %d leu %d valores.\n", data->id, data->n_valores);
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
        thread_data[i].valores = NULL;  // Inicializa com NULL
        thread_data[i].n_valores = 0;   // Inicializa o contador

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

    // Exemplo de uso dos valores lidos por cada thread
    for (int i = 0; i < n_threads; i++) {
        printf("Valores lidos pela thread %d:\n", thread_data[i].id);
        for (int j = 0; j < thread_data[i].n_valores; j++) {
            printf("%d ", thread_data[i].valores[j]);
        }
        printf("\n");

        // Libera a memória alocada para os valores
        free(thread_data[i].valores);
    }

    // Fecha os arquivos e libera a memória
    for (int i = 0; i < n_arquivos; i++) {
        fclose(arquivos[i]);
    }
    free(arquivos);

    return 0;
}