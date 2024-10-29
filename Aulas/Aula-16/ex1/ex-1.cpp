#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int array_size = 100;   // Tamanho total do array
    int *array = NULL;      // Array que será dividido
    int local_size;         // Tamanho da parte do array para cada processo
    int *local_array;       // Parte do array para cada processo
    float local_sum = 0.0, local_avg = 0.0;
    float *local_averages = NULL;
    float global_avg = 0.0;

    MPI_Init(&argc, &argv);                 // Inicializa o ambiente MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // Obtém o rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // Obtém o número de processos

    local_size = array_size / size;         // Define o tamanho da parte de cada processo
    local_array = (int *)malloc(local_size * sizeof(int));

    if (rank == 0) {
        array = (int *)malloc(array_size * sizeof(int));
        for (int i = 0; i < array_size; i++) {
            array[i] = rand() % 100;
        }
        printf("Array original: ");
        for (int i = 0; i < array_size; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
    }

    MPI_Scatter(array, local_size, MPI_INT, local_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_size; i++) {
        local_sum += local_array[i];
    }
    local_avg = local_sum / local_size;

    if (rank == 0) {
        local_averages = (float *)malloc(size * sizeof(float));
    }
    MPI_Gather(&local_avg, 1, MPI_FLOAT, local_averages, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        float total_sum = 0.0;
        for (int i = 0; i < size; i++) {
            total_sum += local_averages[i];
        }
        global_avg = total_sum / size;
        printf("Média global: %f\n", global_avg);

        free(array);
        free(local_averages);
    }

    free(local_array);

    MPI_Finalize();  // Finaliza o ambiente MPI
    return 0;
}
