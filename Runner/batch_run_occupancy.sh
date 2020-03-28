#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

job_num=$(($SLURM_ARRAY_TASK_ID))

filelist=$lists_dir/$job_num.list

while read line; do
    input_files=$input_files","$line
done < $filelist

cd $output_dir
mkdir -p $job_num
cd $job_num

echo "loading " $basic_root
source $basic_root

$build_dir/src/correct --method FULL --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --pid $pidCode $input_files nothing
mv output.root full_0.root

$build_dir/src/correct --method FULL --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --pid $pidCode $input_files qn.root
mv output.root full_1.root

$build_dir/src/occupancy --pid $pidCode --trigger $trigger $input_files full_1.root

echo JOB FINISHED!
date $format