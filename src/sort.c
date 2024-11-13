// sort.c
#include <stdio.h>
#include <stdlib.h>
#include "../include/sort.h"

void mergeSort(int *vetor, int posicaoInicio, int posicaoFim) { // Usada para organizar os valores de cada arquivo usando as threads e depois organizar o vetor final
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
            vetorTemp[k++] = vetor[j++];
        } else if (j == posicaoFim + 1 || vetor[i] < vetor[j]) {
            vetorTemp[k++] = vetor[i++];
        } else {
            vetorTemp[k++] = vetor[j++];
        }
    }

    for (i = posicaoInicio; i <= posicaoFim; i++) {
        vetor[i] = vetorTemp[i - posicaoInicio];
    }
    free(vetorTemp);
}
