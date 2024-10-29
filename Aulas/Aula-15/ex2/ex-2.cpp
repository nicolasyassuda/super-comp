#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);  // Inicializa o ambiente MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Obtém o rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Obtém o número de processos

    if (size < 3) {
        if (rank == 0) {
            printf("Este programa requer pelo menos 3 processos.\n");
        }
        MPI_Finalize();
        return 0;
    }

    int message; 

    if (rank == 0) {
        message = 100;
        MPI_Send(&message, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        printf("Processo %d enviou mensagem %d para o processo %d\n", rank, message, rank + 1);

        MPI_Recv(&message, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d recebeu mensagem %d do processo %d\n", rank, message, size - 1);
    } else {
        MPI_Recv(&message, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d recebeu mensagem %d do processo %d\n", rank, message, rank - 1);

        message++;

        int next_rank = (rank + 1) % size; // Para que ele feche o ultimo rank em 0 para fechar o anel.
        MPI_Send(&message, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
        printf("Processo %d enviou mensagem %d para o processo %d\n", rank, message, next_rank);
    }

    MPI_Finalize();
    return 0;
}
