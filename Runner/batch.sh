#!/bin/bash

pidCode=14
method=FW3S
trigger=PT3
efficiency='nothing'
file_list=''

while [ "$#" -gt "1" ]; do
  #echo $#
  case "$1" in
  -p)
    pidCode=$2
    echo found pid code = $pidCode
    shift
    ;;
  -t)
    trigger=$2
    echo found trigger=$trigger
    shift
    ;;
  -m)
    method=$2
    echo found method=$method
    shift
    ;;
  -e)
    efficiency=$2
    echo found efficiency=$efficiency
    shift
    ;;
  -i)
    file_list=$2
    echo found input_data=$file_list
    shift
    ;;
  --)
    shift
    break
    ;;
    # *) echo Error: incorrect option
  esac
  shift
done

basic_root=/lustre/nyx/hades/user/mmamaev/soft/root/install/bin/thisroot.sh

current_dir=$(pwd)
partition=main
time=8:00:00
executable=$current_dir/../Runner/run.sh
build_dir=$current_dir/../build

output_dir=${1}

lists_dir=${output_dir}/lists
log_dir=${output_dir}/log

mkdir -p $output_dir
mkdir -p $log_dir
mkdir -p $lists_dir

csplit -s -f "$lists_dir/" -b %1d.list "$file_list" -k 2 {*}
rm $lists_dir/0.list

n_runs=$(ls $lists_dir/*.list | wc -l)

job_range=1-$n_runs

echo file list=$file_list
echo executable=$executable
echo output_dir=$output_dir
echo log_dir=$log_dir
echo lists_dir=$lists_dir
echo n_runs=$n_runs
echo job_range=$job_range
echo
echo CONFIGURATION:
echo pid code: $pidCode
echo method: $method
echo trigger: $trigger

sbatch -J DT_Reader -p $partition -t $time -a $job_range -e ${log_dir}/%A_%a.e -o ${log_dir}/%A_%a.o --export=executable=$executable,output_dir=$output_dir,file_list=$file_list,basic_root=$basic_root,lists_dir=$lists_dir,build_dir=$build_dir,pidCode=$pidCode,trigger=$trigger,method=$method,efficiency=$efficiency batch_run.sh
