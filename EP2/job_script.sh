#!/bin/bash
#SBATCH -J ep2-758710
#SBATCH -p fast
#SBATCH -n 1
#SBATCH -t 00:15:00
#SBATCH --cpus-per-task=40
#SBATCH --output=%x.%j.out
#SBATCH --error=%x.%j.err

# Executar a versão com Pthread variando o número de threads
echo "*** EXECUTANDO PTHREADS ***"
srun singularity run my_container.sif laplace_pth 1000 1
srun singularity run my_container.sif laplace_pth 1000 2
srun singularity run my_container.sif laplace_pth 1000 5
srun singularity run my_container.sif laplace_pth 1000 10
srun singularity run my_container.sif laplace_pth 1000 20
srun singularity run my_container.sif laplace_pth 1000 40
echo "*** PTHREADS CONCLUÍDO ***"
