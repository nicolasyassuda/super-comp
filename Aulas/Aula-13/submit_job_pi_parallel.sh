#!/bin/bash
#SBATCH --job-name=output_pi_parallel
#SBATCH --output=output_pi_parallel.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=00:05:00
#SBATCH --partition=normal
./omp_schedulers