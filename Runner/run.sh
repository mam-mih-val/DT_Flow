inFile=$1

#build q-vectors

#no corrections
../build/src/correct $inFile nothing
mv output.root output_0.root

#recentering
../build/src/correct $inFile qn.root
mv output.root output_1.root

# #twist and rescale
# ../build/src/correct $inFile qn.root
# mv output.root output_2.root

#correlate q-vecors from desired correction step
ls output_1.root > list
../build/src/correlate output_1.root