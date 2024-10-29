#!/bin/bash
#SBATCH --job-name=ex4           # Nome do trabalho
#SBATCH --output=ex-4.out         # Nome do arquivo de saída
#SBATCH --error=ex4.err          # Nome do arquivo de erro
#SBATCH --nodes=4                      # Número de nós
#SBATCH --ntasks-per-node=1            # Número de tarefas por nó
#SBATCH --time=00:05:00                # Tempo máximo de execução (hh:mm:ss)
#SBATCH --partition=espec            # Partição/filas para submissão
#SBATCH --mem=512MB   

# Executar o programa

mpirun ./ex-4
