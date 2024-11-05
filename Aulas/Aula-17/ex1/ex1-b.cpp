#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initialize_matrix(int *matrix, int size) {
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

void square_matrix_sequential(int *matrix, int size) {
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

    clock_t start, end;
    double cpu_time_used;

    start = clock();
    square_matrix_sequential(matrix, size);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Tempo de execução (Sem Paralelização OpenMP): %f segundos\n", cpu_time_used);

    free(matrix);
    return 0;
}
