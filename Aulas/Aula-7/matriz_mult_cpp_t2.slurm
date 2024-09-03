#!/bin/bash
#SBATCH --job-name=mochila-t2
# Define o nome do job como "matrix_mult_openmp". Esse nome aparece nas listas de jobs e é útil para identificar o job.

#SBATCH --output=mochila-t2-5.out
# Especifica o arquivo onde a saída padrão (stdout) do job será salva.

#SBATCH --ntasks=1
# Define o número de tarefas que o job executará. Neste caso, o job executa uma única tarefa.

#SBATCH --partition=normal
# Especifica a partição (ou fila) onde o job será submetido. Aqui, o job será submetido a fila "normal".
#SBATCH --cpus-per-task=4               # Número de threads por tarefa (2 threads por processo)
#SBATCH --mem=2048                      # Memória total alocada por nó (1024 MB = 1 GB)
#SBATCH --time=03:00:00                # Tempo máximo de execução (10 minutos)
#SBATCH --partition=espec               # fila do cluster a ser utilizada

./mochila-t2
# Executa o programa compilado "mult_matriz_cpp" dentro do nó de computação.