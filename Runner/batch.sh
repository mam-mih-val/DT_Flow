#!/bin/bash

signal=adc
channelSelection=0
minSignal=0
maxSignal=999
pidCode=14
nSteps=2 # 1 is for recentring, 2 is for twist and rescaling

while [ "$#" -gt "2" ]
do
    #echo $#
    case "$1" in
    --signal) signal=$2
    echo found signal=$signal
    shift ;;
    --perchannel) channelSelection=$2
    echo found Channel Selection = $channelSelection
    shift ;;
    --min) minSignal=$2
    echo found minimal signal = $minSignal
    shift ;;
    --max) maxSignal=$2
    echo found maximal signal = $maxSignal
    shift ;;
    --pid) pidCode=$2
    echo found pid code = $pidCode
    shift ;;
    --nsteps) nSteps=$2
    echo found number of steps: $nSteps
    shift ;;
    --) shift
    break ;;
    # *) echo Error: incorrect option
    esac
    shift
done

inFile=$1
build_dir=$2

basic_root=/lustre/nyx/cbm/users/ogolosov/soft/root-6.14.08_std11/bin/thisroot.sh 

current_dir=$(pwd)
partition=main
time=8:00:00
executable=$current_dir/../Runner/run.sh
build_dir=$current_dir/../build

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

echo CONFIGURATION:
echo signal type: $signal
echo signal range: $minSignal-$maxSignal
echo channel selection: $channelSelection
echo pid code: $pidCode
echo number of correction steps: $nSteps

sbatch -J DT_Reader -p $partition -t $time -a $job_range -e ${log_dir}/%A_%a.e -o ${log_dir}/%A_%a.o --export=executable=$executable,output_dir=$output_dir,file_list=$file_list,basic_root=$basic_root,build_dir=$build_dir,minSignal=$minSignal,maxSignal=$maxSignal,signal=$signal,channelSelection=$channelSelection,nSteps=$nSteps,pidCode=$pidCode batch_run.sh