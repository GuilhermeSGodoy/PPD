#!/bin/bash
#SBATCH -J ep1-758710
#SBATCH -p fast
#SBATCH -n 1
#SBATCH -t 00:15:00
#SBATCH --cpus-per-task=40
#SBATCH --output=%x.%j.out
#SBATCH --error=%x.%j.err

# Executar a versão sequencial com 1 bilhão de passos
echo "*** EXECUTANDO SEQUENCIAL ***"
srun singularity run my_container.sif pi_seq 1000000000
echo "*** SEQUENCIAL CONCLUÍDO ***"

# Executar a versão com Pthread variando o número de threads
echo "*** EXECUTANDO PTHREADS ***"
srun singularity run my_container.sif pi_pth 1000000000 1
srun singularity run my_container.sif pi_pth 1000000000 2
srun singularity run my_container.sif pi_pth 1000000000 5
srun singularity run my_container.sif pi_pth 1000000000 10
srun singularity run my_container.sif pi_pth 1000000000 20
srun singularity run my_container.sif pi_pth 1000000000 40
echo "*** PTHREADS CONCLUÍDO ***"

# Executar a versão com OpenMP variando o número de threads
echo "*** EXECUTANDO OMP ***"
export OMP_NUM_THREADS=1
srun singularity run my_container.sif pi_omp 1000000000

export OMP_NUM_THREADS=2
srun singularity run my_container.sif pi_omp 1000000000

export OMP_NUM_THREADS=5
srun singularity run my_container.sif pi_omp 1000000000

export OMP_NUM_THREADS=10
srun singularity run my_container.sif pi_omp 1000000000

export OMP_NUM_THREADS=20
srun singularity run my_container.sif pi_omp 1000000000

export OMP_NUM_THREADS=40
srun singularity run my_container.sif pi_omp 1000000000
echo "*** OMP CONCLUÍDO ***"
