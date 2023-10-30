#!/bin/bash

#SBATCH --job-name=pi_calculation
#SBATCH --output=pi_calculation.out
#SBATCH --error=pi_calculation.err
#SBATCH --ntasks=1
#SBATCH --time=00:10:00

module load singularity

# Executar a versão sequencial com 1 bilhão de passos
echo "Executando sequencial"
singularity exec my_container.sif /opt/examples/pi-integral/pi_seq 1000000000
echo "Sequencial concluído"

# Executar a versão com Pthread variando o número de threads
echo "Executando pthreads"
for threads in 1 2 5 10 20 40
do
    singularity exec my_container.sif /opt/examples/pi-integral/pi_pth 1000000000 $threads
done
echo "Pthreads concluído"

# Executar a versão com OpenMP variando o número de threads
echo "Executando OMP"
for threads in 1 2 5 10 20 40
do
    export OMP_NUM_THREADS=$threads
    singularity exec my_container.sif /opt/examples/pi-integral/pi_omp 1000000000
done
echo "OMP concluído"
