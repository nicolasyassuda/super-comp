#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);          // Inicializa o ambiente MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Obtém o rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Obtém o número de processos

    char message[20];

    if (rank == 0) {
        strcpy(message, "Olá");
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        printf("Processo %d enviou: %s\n", rank, message);

        MPI_Recv(message, 20, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d recebeu: %s\n", rank, message);
    } 
    else if (rank == 1) {
        MPI_Recv(message, 20, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d recebeu: %s\n", rank, message);

        strcpy(message, "Oi");
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        printf("Processo %d enviou: %s\n", rank, message);
    }
    
    MPI_Finalize();  // Finaliza o ambiente MPI
    return 0;
}
