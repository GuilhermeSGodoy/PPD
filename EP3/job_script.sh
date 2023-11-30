#!/bin/bash
#SBATCH -J ep1-758710
#SBATCH -p fast
#SBATCH -n 1
#SBATCH -t 00:15:00
#SBATCH --cpus-per-task=40
#SBATCH --output=%x.%j.out
#SBATCH --error=%x.%j.err

echo "*** EXECUTANDO OMP ***"
srun singularity run my_container.sif laplace_omp 1000 1

srun singularity run my_container.sif laplace_omp 1000 2

srun singularity run my_container.sif laplace_omp 1000 5

srun singularity run my_container.sif laplace_omp 1000 10

srun singularity run my_container.sif laplace_omp 1000 20

srun singularity run my_container.sif laplace_omp 1000 40
echo "*** OMP CONCLUÍDO ***"
