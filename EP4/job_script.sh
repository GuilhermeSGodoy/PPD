#!/bin/bash
#SBATCH -J ep4-758710
#SBATCH -p fast
#SBATCH -n 1
#SBATCH -t 00:15:00
#SBATCH --cpus-per-task=40
#SBATCH --output=%x.%j.out
#SBATCH --error=%x.%j.err
#SBATCH -N 40 # total number of nodes

# https://docs.sylabs.io/guides/3.3/user-guide/mpi.html

echo "*** EXECUTANDO MPI ***"

#mpirun -n $NP singularity exec my_container.sif vecadd_mpi 10000000
mpirun -n 1 singularity exec my_container.sif vecadd_mpi 10000000
mpirun -n 2 singularity exec my_container.sif vecadd_mpi 10000000
#mpirun -n 5 singularity exec my_container.sif vecadd_mpi 10000000
#mpirun -n 10 singularity exec my_container.sif vecadd_mpi 10000000
#mpirun -n 20 singularity exec my_container.sif vecadd_mpi 10000000
#mpirun -n 40 singularity exec my_container.sif vecadd_mpi 10000000

echo "*** MPI CONCLUÍDO ***"
