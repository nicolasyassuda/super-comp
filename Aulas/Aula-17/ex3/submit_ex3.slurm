#!/bin/bash
#SBATCH --job-name=ex2
#SBATCH --output=ex3-3.txt
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=4
#SBATCH --time=00:20:00
#SBATCH --partition=normal

mpirun ./ex3 100000