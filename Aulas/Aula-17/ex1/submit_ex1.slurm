#!/bin/bash
#SBATCH --job-name=ex1
#SBATCH --output=ex1-4.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=00:20:00
#SBATCH --partition=normal

./ex1 100000