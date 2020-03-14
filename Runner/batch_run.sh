#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

job_num=$(($SLURM_ARRAY_TASK_ID))

filelist=$lists_dir/$job_num.list

while read line; do
    input_files=$input_files","$line
done < $filelist

cp PrimaryQa.cpp $output_dir
cd $output_dir
mkdir -p $job_num
cd $job_num

echo "loading " $basic_root
source $basic_root

#no corrections
echo Correction steps
echo
echo "executing $build_dir/src/correct --method $method --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --list 1 $input_files nothing"

$build_dir/src/correct --method $method --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --pid $pidCode --list 1 $input_files nothing
mv output.root output_0.root

#recentering
$build_dir/src/correct --method $method --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --pid $pidCode --list 1 $input_files qn.root
mv output.root output_1.root

#twist and rescale
$build_dir/src/correct --method $method --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --pid $pidCode --list 1 $input_files qn.root
mv output.root output_2.root

$build_dir/src/occupancy --pid $pidCode --trigger $trigger $input_files output_2.root

#correlate q-vecors from desired correction step
echo Correlation step
echo
$build_dir/src/correlate --method $method output_2.root
echo JOB FINISHED!
date $format