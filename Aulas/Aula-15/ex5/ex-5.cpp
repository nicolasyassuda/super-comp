#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);               // Inicializa o ambiente MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Obtém o rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Obtém o número de processos

    int value = rank + 1;
    int sum = 0;

    if (rank == 0) {
        sum = value;
        MPI_Send(&sum, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        printf("Processo %d iniciou com valor %d e enviou soma parcial %d para o processo %d\n", rank, value, sum, rank + 1);
    } else {
        MPI_Recv(&sum, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sum += value;
        printf("Processo %d recebeu soma parcial %d, somou seu valor %d, totalizando %d\n", rank, sum - value, value, sum);

        if (rank < size - 1) {
            MPI_Send(&sum, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            printf("Processo %d enviou soma parcial %d para o processo %d\n", rank, sum, rank + 1);
        } else {
            printf("Processo %d é o último e exibe a soma total: %d\n", rank, sum);
        }
    }

    MPI_Finalize();
    return 0;
}
