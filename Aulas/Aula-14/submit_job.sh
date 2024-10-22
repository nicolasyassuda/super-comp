#!/bin/bash
#SBATCH --job-name=omp_scheduler_test
#SBATCH --output=parallel2-montecarlo-output.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=00:20:00
#SBATCH --partition=normal
./parallel2-montecarlo