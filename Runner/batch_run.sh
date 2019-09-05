#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format


job_num=$(($SLURM_ARRAY_TASK_ID))
input_file=`sed -n "${job_num}p" < $file_list`

cd $output_dir
mkdir -p $job_num
cd $job_num

echo "loading " $basic_root
source $basic_root

echo "executing $executable $input_file"

$executable --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal $input_file $build_dir

echo JOB FINISHED!
date $format