inFile=$1
build_dir=$2
#build q-vectors

#no corrections
$build_dir/src/correct $inFile nothing
mv output.root output_0.root

# #recentering
$build_dir/src/correct $inFile qn.root
mv output.root output_1.root

# # #twist and rescale
# ../build/src/correct $inFile qn.root
# mv output.root output_2.root

#correlate q-vecors from desired correction step
ls output_1.root > list
$build_dir/src/correlate output_1.root