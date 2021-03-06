signal=adc
channelSelection=0
minSignal=0
maxSignal=999
pidCode=14
isList=0
method=FULL

while [ "$#" -gt "2" ]; do
  #echo $#
  case "$1" in
  --signal)
    signal=$2
    echo found signal=$signal
    shift
    ;;
  --perchannel)
    channelSelection=$2
    echo found Channel Selection = $channelSelection
    shift
    ;;
  --min)
    minSignal=$2
    echo found minimal signal = $minSignal
    shift
    ;;
  --max)
    maxSignal=$2
    echo found maximal signal = $maxSignal
    shift
    ;;
  --pid)
    pidCode=$2
    echo found pid code = $pidCode
    shift
    ;;
  --trigger)
    trigger=$2
    echo found trigger=$trigger
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

inFile=$1
build_dir=$2
#build q-vectors

#no corrections
echo "executing $build_dir/src/correct --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal $inFile nothing"
$build_dir/src/correct --method $method --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --pid $pidCode --list $isList $inFile nothing
mv output.root full_0.root

#recentering
$build_dir/src/correct --method $method --trigger $trigger --signal $signal --perchannel $channelSelection --min $minSignal --max $maxSignal --pid $pidCode --list $isList $inFile  qn.root
mv output.root full_1.root

$build_dir/src/occupancy --pid $pidCode --trigger $trigger $inFile full_1.root
