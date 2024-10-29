#!/bin/bash
#SBATCH --job-name=ex3           # Nome do trabalho
#SBATCH --output=ex-3.out         # Nome do arquivo de saída
#SBATCH --error=ex3.err          # Nome do arquivo de erro
#SBATCH --nodes=4                      # Número de nós
#SBATCH --ntasks-per-node=1            # Número de tarefas por nó
#SBATCH --time=00:05:00                # Tempo máximo de execução (hh:mm:ss)
#SBATCH --partition=espec            # Partição/filas para submissão
#SBATCH --mem=512MB   

# Executar o programa

mpirun ./ex-3
