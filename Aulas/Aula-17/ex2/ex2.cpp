// square_matrix_mpi.cpp
#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib> // Para rand() e srand()
#include <ctime>   // Para time()

// Função para inicializar a matriz com valores aleatórios
void initialize_matrix(std::vector<int>& matrix, int size) {
    for(int i = 0; i < size * size; ++i) {
        matrix[i] = rand() % 100; // Valores entre 0 e 99
    }
}

int main(int argc, char* argv[]) {
    int rank, size;
    int matriz_size;
    std::vector<int> matriz; // Apenas o processo raiz irá armazenar a matriz completa
    std::vector<int> original_matrix; // Cópia da matriz original
    std::vector<int> sub_matriz;
    int elementos_por_processo, resto;
    std::vector<int> recvcounts;
    std::vector<int> displs;

    // Inicializar o ambiente MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Verificar argumentos
    if(argc != 2) {
        if(rank == 0) {
            std::cerr << "Uso: " << argv[0] << " <tamanho_da_matriz>" << std::endl;
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    matriz_size = std::atoi(argv[1]);
    int total_elementos = matriz_size * matriz_size;

    // Processo 0 inicializa a matriz
    if(rank == 0) {
        matriz.resize(total_elementos);
        srand(static_cast<unsigned int>(time(NULL)));
        initialize_matrix(matriz, matriz_size);
    }

    // Broadcast do tamanho da matriz para todos os processos
    MPI_Bcast(&matriz_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    total_elementos = matriz_size * matriz_size;

    // Calcular recvcounts e displs no processo raiz
    if(rank == 0) {
        recvcounts.resize(size, 0);
        displs.resize(size, 0);
        elementos_por_processo = total_elementos / size;
        resto = total_elementos % size;

        for(int i = 0; i < size; ++i) {
            recvcounts[i] = elementos_por_processo + (i < resto ? 1 : 0);
            displs[i] = (i == 0) ? 0 : displs[i-1] + recvcounts[i-1];
        }
    }

    // Broadcast de recvcounts para todos os processos
    if(rank != 0) {
        recvcounts.resize(size);
    }
    MPI_Bcast(recvcounts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular displs no processo atual
    if(rank != 0) {
        displs.resize(size);
    }
    MPI_Bcast(displs.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada processo sabe quantos elementos receberá
    int my_count = recvcounts[rank];

    // Alocar espaço para a sub-matriz
    sub_matriz.resize(my_count);

    // Sincronizar todos os processos antes de iniciar a medição de tempo
    MPI_Barrier(MPI_COMM_WORLD);

    // Iniciar a medição de tempo
    double start_time = MPI_Wtime();

    // Utilizar MPI_Scatterv para distribuir os dados
    MPI_Scatterv(
        matriz.data(),           // Enviar buffer (apenas o root)
        recvcounts.data(),       // Quantidade de elementos a serem recebidos por cada processo
        displs.data(),           // Deslocamentos para cada processo
        MPI_INT,                 // Tipo de dados
        sub_matriz.data(),       // Buffer de recepção
        my_count,                // Número de elementos a receber
        MPI_INT,                 // Tipo de dados
        0,                       // Processo raiz
        MPI_COMM_WORLD           // Comunicador
    );

    // Cada processo calcula o quadrado dos elementos recebidos
    for(auto &element : sub_matriz) {
        element = element * element;
    }

    // Utilizar MPI_Gatherv para reunir os resultados
    MPI_Gatherv(
        sub_matriz.data(),       // Enviar buffer
        my_count,                // Número de elementos a enviar
        MPI_INT,                 // Tipo de dados
        matriz.data(),           // Receber buffer (apenas o root)
        recvcounts.data(),       // Quantidade de elementos que cada processo enviará
        displs.data(),           // Deslocamentos para cada processo
        MPI_INT,                 // Tipo de dados
        0,                       // Processo raiz
        MPI_COMM_WORLD           // Comunicador
    );

    // Sincronizar todos os processos antes de parar a medição de tempo
    MPI_Barrier(MPI_COMM_WORLD);

    // Parar a medição de tempo
    double end_time = MPI_Wtime();

    // Calcular o tempo total de execução
    double elapsed_time = end_time - start_time;

    // Processo raiz imprime o tempo de execução e as matrizes
    if(rank == 0) {
        std::cout << "Tempo de execução para calcular o quadrado da matriz: " 
                  << elapsed_time << " segundos" << std::endl;

    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
