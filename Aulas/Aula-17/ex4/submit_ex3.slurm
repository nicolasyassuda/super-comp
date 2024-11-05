#!/bin/bash
#SBATCH --job-name=ex2
#SBATCH --output=ex4-4.txt
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=4
#SBATCH --time=00:20:00
#SBATCH --partition=normal

mpirun ./ex4 10000 50