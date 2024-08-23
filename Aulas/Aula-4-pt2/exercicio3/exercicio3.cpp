#include <iostream>   // Inclui a biblioteca padrão de entrada e saída, usada para operações como std::cout.
#include <chrono>     // Inclui a biblioteca para medição de tempo, usada para medir a performance do código.

int main() {
    int N = 1000;  // Define o tamanho da matriz como N x N, onde N é 1000.

    // Alocação dinâmica de memória para uma matriz N x N.
    // Primeiro, aloca um array de ponteiros, onde cada ponteiro irá apontar para uma linha da matriz.
    int** matriz = new int*[N];

    // Para cada linha da matriz, aloca um array de inteiros de tamanho N.
    for (int i = 0; i < N; ++i) {
        matriz[i] = new int[N];
    }
    // Inicializar o timer do chrono.
    auto start = std::chrono::high_resolution_clock::now();

    // Aqui você pode inicializar e somar os elementos da matriz.
    // Por exemplo, você pode preencher a matriz com valores e calcular a soma total.
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            matriz[i][j] = i + j;
        }
    }

    // Finalizar o timer do chrono.
    auto end = std::chrono::high_resolution_clock::now();

    // Calcular a diferença de tempo e imprimir o resultado.
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Tempo de execução: " << elapsed_seconds.count() << "s\n";
    
    // Liberação da memória alocada dinamicamente.
    // Primeiro, libera a memória alocada para cada linha (os arrays de inteiros).
    for (int i = 0; i < N; ++i) {
        delete[] matriz[i];
    }

    // Finalmente, libera a memória alocada para o array de ponteiros.
    delete[] matriz;

    return 0;  // Retorna 0, indicando que o programa terminou com sucesso.
}