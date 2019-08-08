#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

cd $output_dir

job_num=$(($SLURM_ARRAY_TASK_ID))
input_file=`sed -n "${job_num}p" < $file_list`

mkdir -p $job_num
cd $job_num

echo "loading " $basic_root
source $basic_root

echo "executing $executable $input_file"

$executable $input_file

echo JOB FINISHED!
date $format