#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);               // Inicializa o ambiente MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Obtém o rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Obtém o número de processos

    char message[50];

    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            sprintf(message, "Mensagem do processo 0 para o processo %d", i);
            MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            printf("Processo %d enviou: %s\n", rank, message);

            MPI_Recv(message, 50, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Processo %d recebeu resposta: %s\n", rank, message);
        }
    } else {
        MPI_Recv(message, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d recebeu: %s\n", rank, message);

        sprintf(message, "Resposta do processo %d para o processo 0", rank);
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        printf("Processo %d enviou resposta: %s\n", rank, message);
    }

    MPI_Finalize(); 
    return 0;
}
