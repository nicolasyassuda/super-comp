// mean_mpi_openmp.cpp
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <vector>
#include <cstdlib> // Para rand() e srand()
#include <ctime>   // Para time()
#include <iomanip> // Para std::setprecision

// Função para inicializar o vetor com valores aleatórios
void initialize_vector(std::vector<double>& vec, int size) {
    for(int i = 0; i < size; ++i) {
        vec[i] = static_cast<double>(rand()) / RAND_MAX * 100.0; // Valores entre 0.0 e 100.0
    }
}

int main(int argc, char* argv[]) {
    int rank, size;
    int N; // Tamanho total do vetor
    std::vector<double> vector_total; // Apenas o processo raiz irá armazenar o vetor completo
    std::vector<double> sub_vector;    // Vetor local para cada processo
    int elementos_por_processo, resto;
    std::vector<int> recvcounts;
    std::vector<int> displs;
    double local_sum = 0.0;
    double total_sum = 0.0;
    double start_time, end_time;

    // Inicializar o ambiente MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obter o rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obter o número total de processos

    // Verificar argumentos
    if(argc != 2) {
        if(rank == 0) {
            std::cerr << "Uso: " << argv[0] << " <tamanho_do_vetor_N>" << std::endl;
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    N = std::atoi(argv[1]);
    if(N <= 0) {
        if(rank == 0) {
            std::cerr << "O tamanho do vetor N deve ser positivo." << std::endl;
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Processo raiz inicializa o vetor
    if(rank == 0) {
        vector_total.resize(N);
        srand(static_cast<unsigned int>(time(NULL)));
        initialize_vector(vector_total, N);
    }

    // Broadcast do tamanho do vetor para todos os processos
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcular recvcounts e displs no processo raiz
    if(rank == 0) {
        recvcounts.resize(size, 0);
        displs.resize(size, 0);
        elementos_por_processo = N / size;
        resto = N % size;

        for(int i = 0; i < size; ++i) {
            recvcounts[i] = elementos_por_processo + (i < resto ? 1 : 0);
            displs[i] = (i == 0) ? 0 : displs[i-1] + recvcounts[i-1];
        }
    }

    // Broadcast de recvcounts e displs para todos os processos
    if(rank != 0) {
        recvcounts.resize(size);
        displs.resize(size);
    }
    MPI_Bcast(recvcounts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(displs.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada processo sabe quantos elementos receberá
    int my_count = recvcounts[rank];

    // Alocar espaço para o sub-vetor
    sub_vector.resize(my_count);

    // Sincronizar todos os processos antes de iniciar a medição de tempo
    MPI_Barrier(MPI_COMM_WORLD);

    // Iniciar a medição de tempo
    start_time = MPI_Wtime();

    // Distribuir os dados usando MPI_Scatterv
    MPI_Scatterv(
        vector_total.data(), // Enviar buffer (apenas o root)
        recvcounts.data(),   // Quantidade de elementos para cada processo
        displs.data(),       // Deslocamentos para cada processo
        MPI_DOUBLE,          // Tipo de dados
        sub_vector.data(),   // Buffer de recepção
        my_count,            // Número de elementos a receber
        MPI_DOUBLE,          // Tipo de dados
        0,                   // Processo raiz
        MPI_COMM_WORLD       // Comunicador
    );

    // Cada processo calcula a soma dos seus elementos utilizando OpenMP
    #pragma omp parallel for reduction(+:local_sum)
    for(int i = 0; i < my_count; ++i) {
        local_sum += sub_vector[i];
    }

    // Reunir as somas parciais no processo raiz
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Sincronizar todos os processos antes de parar a medição de tempo
    MPI_Barrier(MPI_COMM_WORLD);

    // Parar a medição de tempo
    end_time = MPI_Wtime();

    // Processo raiz calcula a média e imprime os resultados
    if(rank == 0) {
        double mean = total_sum / static_cast<double>(N);
        std::cout << "Soma Total: " << total_sum << std::endl;
        std::cout << "Média do Vetor: " << mean << std::endl;
        std::cout << "Tempo de execução: " << (end_time - start_time) << " segundos" << std::endl;
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
