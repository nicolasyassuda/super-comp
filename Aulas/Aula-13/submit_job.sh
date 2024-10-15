#!/bin/bash
#SBATCH --job-name=omp_scheduler_test
#SBATCH --output=output_omp_schedulers2.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=00:05:00
#SBATCH --partition=normal
./omp_schedulers