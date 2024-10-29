#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);               // Inicializa o ambiente MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Obtém o rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Obtém o número de processos

    int message = 123;

    if (rank == 0) {
        MPI_Send(&message, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        printf("Processo %d enviou a mensagem %d para o processo %d\n", rank, message, rank + 1);
    } else {
        MPI_Recv(&message, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d recebeu a mensagem %d do processo %d\n", rank, message, rank - 1);

        // Se não for o último processo, envia a mensagem para o próximo
        if (rank < size - 1) {
            MPI_Send(&message, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            printf("Processo %d enviou a mensagem %d para o processo %d\n", rank, message, rank + 1);
        }
    }

    MPI_Finalize();
    return 0;
}
