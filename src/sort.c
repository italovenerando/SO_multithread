// sort.c
#include <stdio.h>
#include <stdlib.h>
#include "../include/sort.h"

void quick(int *vet, int esq, int dir) { // Usada para organizar o vetor final
    int pivo = esq, i, ch, j;
    for (i = esq + 1; i <= dir; i++) {
        j = i;
        if (vet[j] < vet[pivo]) {
            ch = vet[j];
            while (j > pivo) {
                vet[j] = vet[j - 1];
                j--;
            }
            vet[j] = ch;
            pivo++;
        }
    }
    if (pivo - 1 >= esq) {
        quick(vet, esq, pivo - 1);
    }
    if (pivo + 1 <= dir) {
       quick(vet, pivo + 1, dir);
    }
}

void mergeSort(int *vetor, int posicaoInicio, int posicaoFim) { // Usada para organizar os valores de cada arquivo usando as threads
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
