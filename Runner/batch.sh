#!/bin/bash

basic_root=/lustre/nyx/cbm/users/ogolosov/soft/root-6.14.08_std11/bin/thisroot.sh 

current_dir=$(pwd)
partition=main
time=8:00:00
executable=$current_dir/../Runner/run.sh

file_list=${1}
output_dir=${2}

log_dir=${output_dir}/log

mkdir -p $output_dir
mkdir -p $log_dir

n_runs=$(cat $file_list | wc -l)

job_range=1-$n_runs

echo file list=$file_list
echo executable=$executable
echo output_dir=$output_dir
echo log_dir=$log_dir
echo n_runs=$n_runs
echo job_range=$job_range

sbatch -J DT_Reader -p $partition -t $time -a $job_range -e ${log_dir}/%A_%a.e -o ${log_dir}/%A_%a.o --export=executable=$executable,output_dir=$output_dir,file_list=$file_list,basic_root=$hadesroot batch_run.sh