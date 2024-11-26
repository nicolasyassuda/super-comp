#!/bin/bash
#SBATCH --job-name=ex3
#SBATCH --output=ex3-output.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=2
#SBATCH --mem=1024
#SBATCH --time=00:02:00 
#SBATCH --partition=espec 

export OMP_NUM_THREADS=4

./ex3
