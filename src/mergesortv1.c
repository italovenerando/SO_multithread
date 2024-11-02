#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>



void* thread_function(void* arg) {
    int thread_id = *(int*)arg;
    printf("Thread %d: executando...\n", thread_id);

    // Código da thread aqui...
    pthread_exit(NULL);
}

int main(int n_parametros, char* argv[]) {
    
    int n_threads = atoi(argv[1]);

    pthread_t threads[n_threads]; //declara um array de threads do tipo pthreads_t
    int thread_ids[n_threads]; //id para identificar cada thread

    for (int i = 0; i < n_threads; i++) {
        thread_ids[i] = i;
        int result = pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
        if (result) {
            printf("Erro ao criar thread %d\n", i); //se result retornar 1, entao ocorreu um erro ao criar as threads
            exit(-1);
        }
    }

    // Esperar que todas as threads terminem
    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Todas as threads terminaram.\n");
    return 0;
}
