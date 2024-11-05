// parallel_search_mpi_openmp.cpp
#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <vector>
#include <cstdlib> // Para rand() e srand()
#include <ctime>   // Para time()
#include <iomanip> // Para std::setprecision

// Função para inicializar o vetor com valores aleatórios
void initialize_vector(std::vector<int>& vec, int size) {
    for(int i = 0; i < size; ++i) {
        vec[i] = rand() % 1000; // Valores entre 0 e 999
    }
}

// Função para imprimir o vetor (opcional, para depuração)
void print_vector(const std::vector<int>& vec, int size) {
    for(int i = 0; i < size; ++i) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    int rank, size;
    int N; // Tamanho total do vetor
    int target; // Valor a ser buscado
    std::vector<int> vector_total; // Apenas o processo raiz irá armazenar o vetor completo
    std::vector<int> sub_vector;    // Vetor local para cada processo
    int elementos_por_processo, resto;
    std::vector<int> recvcounts;
    std::vector<int> displs;
    std::vector<int> local_positions; // Posições encontradas localmente
    std::vector<int> all_positions;   // Todas as posições encontradas (no root)
    double start_time, end_time;

    // Inicializar o ambiente MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obter o rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obter o número total de processos

    // Verificar argumentos
    if(argc != 3) {
        if(rank == 0) {
            std::cerr << "Uso: " << argv[0] << " <tamanho_do_vetor_N> <valor_a_buscar>" << std::endl;
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    N = std::atoi(argv[1]);
    target = std::atoi(argv[2]);

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

    // Broadcast do valor alvo para todos os processos
    MPI_Bcast(&target, 1, MPI_INT, 0, MPI_COMM_WORLD);

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
        MPI_INT,             // Tipo de dados
        sub_vector.data(),   // Buffer de recepção
        my_count,            // Número de elementos a receber
        MPI_INT,             // Tipo de dados
        0,                   // Processo raiz
        MPI_COMM_WORLD       // Comunicador
    );

    // Cada processo busca as ocorrências do valor alvo utilizando OpenMP
    // Armazena as posições relativas ao sub-vetor
    std::vector<int> local_found_indices;
    #pragma omp parallel
    {
        // Cada thread terá seu próprio vetor para evitar condições de corrida
        std::vector<int> thread_found_indices;
        #pragma omp for nowait
        for(int i = 0; i < my_count; ++i) {
            if(sub_vector[i] == target) {
                thread_found_indices.push_back(i);
            }
        }
        // Combinar os resultados das threads
        #pragma omp critical
        {
            local_found_indices.insert(local_found_indices.end(),
                                       thread_found_indices.begin(),
                                       thread_found_indices.end());
        }
    }

    // Enviar o número de ocorrências encontradas para o root
    int local_count = local_found_indices.size();
    std::vector<int> counts(size, 0); // Número de ocorrências por processo
    MPI_Gather(&local_count, 1, MPI_INT, counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Preparar para coletar as posições encontradas
    std::vector<int> recv_displs; // Deslocamentos para receber os dados
    std::vector<int> all_found_indices; // Todas as posições encontradas (no root)

    if(rank == 0) {
        recv_displs.resize(size, 0);
        int total_found = 0;
        for(int i = 0; i < size; ++i) {
            recv_displs[i] = total_found;
            total_found += counts[i];
        }
        all_found_indices.resize(total_found);
    }

    // Enviar as posições encontradas para o root
    MPI_Gatherv(
        local_found_indices.data(), // Enviar buffer
        local_count,                // Número de elementos a enviar
        MPI_INT,                    // Tipo de dados
        all_found_indices.data(),   // Receber buffer (apenas o root)
        counts.data(),              // Quantidade de elementos que cada processo enviará
        recv_displs.data(),         // Deslocamentos para cada processo
        MPI_INT,                    // Tipo de dados
        0,                          // Processo raiz
        MPI_COMM_WORLD              // Comunicador
    );

    // Sincronizar todos os processos antes de parar a medição de tempo
    MPI_Barrier(MPI_COMM_WORLD);

    // Parar a medição de tempo
    end_time = MPI_Wtime();

    // Processo raiz ajusta as posições globais e imprime os resultados
    if(rank == 0) {
        // Ajustar as posições para refletirem o vetor global
        std::vector<int> global_found_indices;
        for(int i = 0; i < size; ++i) {
            for(int j = 0; j < counts[i]; ++j) {
                int global_pos = displs[i] + all_found_indices[recv_displs[i] + j];
                global_found_indices.push_back(global_pos);
            }
        }

        // Exibir as posições encontradas
        std::cout << "Valor alvo " << target << " encontrado nas posições: ";
        if(global_found_indices.empty()) {
            std::cout << "Nenhuma ocorrência encontrada." << std::endl;
        } else {
            for(auto pos : global_found_indices) {
                std::cout << pos << " ";
            }
            std::cout << std::endl;
        }

        // Exibir o tempo de execução
        std::cout << "Tempo de execução: " << (end_time - start_time) << " segundos" << std::endl;
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
