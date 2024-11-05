#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void initialize_matrix(int *matrix, int size) {
    #pragma omp parallel for
    for(int i = 0; i < size * size; i++) {
        matrix[i] = rand() % 100;
    }
}

void print_matrix(int *matrix, int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            printf("%d ", matrix[i * size + j]);
        }
        printf("\n");
    }
}

void square_matrix_parallel(int *matrix, int size) {
    #pragma omp parallel for
    for(int i = 0; i < size * size; i++) {
        matrix[i] = matrix[i] * matrix[i];
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Uso: %s <tamanho_da_matriz>\n", argv[0]);
        return -1;
    }

    int size = atoi(argv[1]);
    int *matrix = (int *)malloc(size * size * sizeof(int));

    srand(time(NULL));
    initialize_matrix(matrix, size);

    double start_time, end_time, cpu_time_used;

    start_time = omp_get_wtime();
    square_matrix_parallel(matrix, size);
    end_time = omp_get_wtime();

    cpu_time_used = end_time - start_time;
    printf("Tempo de execução (Paralelo com OpenMP): %f segundos\n", cpu_time_used);

    free(matrix);
    return 0;
}
