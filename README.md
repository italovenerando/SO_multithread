# Projeto: Ordenação Multi-threaded com POSIX Threads

## 📌 Descrição
Este projeto foi desenvolvido para aprofundar os conhecimentos em **lógica de programação** e na **linguagem C**, utilizando **múltiplas threads** com a biblioteca **POSIX Threads**.

O programa implementa um algoritmo de ordenação multi-threaded para ler valores inteiros de **N arquivos de entrada**, organizá-los em **ordem crescente** e armazená-los em um único arquivo de saída. 

## 🔍 Funcionamento
- O programa recebe como entrada **N arquivos** contendo valores inteiros não ordenados (podem estar repetidos ou não).
- Utiliza **T threads** (2, 4 ou 8) para processar e ordenar os números.
- Os dados são lidos, ordenados e armazenados em um único arquivo de saída.
- O tempo de execução de cada thread e o tempo total são exibidos na tela.

## 🚀 Execução
O programa deve ser executado via linha de comando no **Linux**, seguindo o seguinte formato:
```sh
./mergesort <num_threads> <arquivo1> <arquivo2> ... -o <arquivo_saida>
```
### 📌 Exemplo:
```sh
./mergesort 4 arq1.dat arq2.dat arq3.dat -o saida.dat
```
Onde:
- `./mergesort` é o nome do programa;
- `4` define o número de threads;
- `arq1.dat arq2.dat arq3.dat` são os arquivos de entrada;
- `-o saida.dat` especifica o arquivo de saída ordenado.

## 📥 Entradas
- O número de threads a ser utilizado (2, 4 ou 8).
- Os arquivos contendo os números inteiros desordenados.

## 📤 Saídas
- Um **arquivo de saída** com os números ordenados.
- Tempos de execução exibidos na tela, conforme o exemplo:
```sh
Tempo de execução do Thread 0: 0.092 segundos.
Tempo de execução do Thread 1: 0.067 segundos.
Tempo de execução do Thread 2: 0.073 segundos.
Tempo de execução do Thread 3: 0.058 segundos.
Tempo total de execução: 5.922 segundos.
```

## 🔧 Tecnologias Utilizadas
- **Linguagem C (pura)**
- **POSIX Threads (pthread)**
- **Sistema Operacional: Linux**

## 📚 Algoritmo de Ordenação
O algoritmo utilizado para a ordenação dos números é o **Merge Sort**. O **Merge Sort** é um algoritmo de ordenação eficiente que segue o paradigma **dividir para conquistar**. Ele funciona da seguinte maneira:

1. Divide o conjunto de números em duas metades recursivamente até que cada subconjunto contenha apenas um elemento.
2. Em seguida, os subconjuntos são mesclados (merge) de maneira ordenada para formar a sequência final ordenada.
3. No contexto deste projeto, o processo de mesclagem é distribuído entre múltiplas threads para otimizar o tempo de execução.

Este algoritmo é eficiente, com complexidade **O(n log n)**, sendo ideal para grandes volumes de dados.

## 📜 Licença
Este projeto foi desenvolvido para fins desenvolvimento pessoal.

---
📌 Desenvolvido por **Ítalo Carvalhaes Camargo Venerando** e **Augusto Toledo Caires de Oliveira** 🎓
