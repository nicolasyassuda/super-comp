#!/bin/bash
#SBATCH --job-name=partD
#SBATCH --output=partD-output.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=2
#SBATCH --mem=1024
#SBATCH --time=00:02:00 
#SBATCH --partition=espec 

export OMP_NUM_THREADS=4

./partD
