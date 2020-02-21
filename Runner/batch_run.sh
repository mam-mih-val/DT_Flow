#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

job_num=$(($SLURM_ARRAY_TASK_ID))
input_file=`sed -n "${job_num}p" < $file_list`

cp PrimaryQa.cpp $output_dir
cd $output_dir
mkdir -p $job_num
cd $job_num

echo "loading " $basic_root
source $basic_root

#no corrections
echo Correction steps
echo
echo "executing $build_dir/src/correct --method $method --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal $input_file nothing"

$build_dir/src/correct --method $method --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --pid $pidCode $input_file nothing
mv output.root output_0.root

#recentering
$build_dir/src/correct --method $method --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --pid $pidCode $input_file qn.root
mv output.root output_1.root

#twist and rescale
$build_dir/src/correct --method $method --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --pid $pidCode $input_file qn.root
mv output.root output_2.root

#correlate q-vecors from desired correction step
echo Correlation step
echo
ls output_1.root > list
$build_dir/src/correlate --method $method output_1.root
root -l -q -b "PrimaryQa.cpp(\"output_${nSteps}.root\")"
echo JOB FINISHED!
date $format