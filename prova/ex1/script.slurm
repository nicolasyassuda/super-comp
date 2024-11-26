#!/bin/bash
#SBATCH --job-name=exercicio1
#SBATCH --output=exercicio1-output.txt
#SBATCH --nodes=5
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --mem=16G
#SBATCH --time=01:00:00 
#SBATCH --partition=prova 

export OMP_NUM_THREADS=15

./programa
