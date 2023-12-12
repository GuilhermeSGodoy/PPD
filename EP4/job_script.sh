#!/bin/bash
#SBATCH -J ep4-758710
#SBATCH -p fast
#SBATCH -n 1
#SBATCH -t 00:15:00
#SBATCH --cpus-per-task=40
#SBATCH --output=%x.%j.out
#SBATCH --error=%x.%j.err

echo "*** EXECUTANDO MPI ***"

srun singularity run my_container.sif vecadd_mpi 1 1000000
srun singularity run my_container.sif vecadd_mpi 2 1000000
srun singularity run my_container.sif vecadd_mpi 5 1000000
srun singularity run my_container.sif vecadd_mpi 10 1000000
srun singularity run my_container.sif vecadd_mpi 20 1000000
srun singularity run my_container.sif vecadd_mpi 40 1000000

echo "*** MPI CONCLUÍDO ***"
