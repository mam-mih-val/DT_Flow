#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

job_num=$(($SLURM_ARRAY_TASK_ID))

filelist=$lists_dir/$job_num.list

while read line; do
    input_files=$input_files","$line
done < $filelist

cp PrimaryQa.cpp $output_dir
cp efficiency.root $output_dir
cd $output_dir
mkdir -p $job_num
cp efficiency.root $job_num
cd $job_num

echo "loading " $basic_root
source $basic_root

#no corrections
echo Optimization steps
echo
$build_dir/src/optimization --density $density $input_files
echo JOB FINISHED!
date $format