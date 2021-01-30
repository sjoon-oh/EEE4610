#!/bin/bash

filelist=`ls ./dataset/s1024*.mtx`
savefile=""
wait

#./spmv-cusparse 1 dataset/s32-r10.000000-mat.mtx | grep "Average" | sed -E 's/[^0-9. ]*//g'
for filename in $filelist
do
	for i in {1..50}
	do
		echo "Running for $filename, for iteration $i"

		savefile=`echo $filename | sed -E 's/.mtx//g' | sed -E 's/.\/dataset\///g'`
		echo "    Setting file name to $savefile.csv"

		./spmv-ker-vector 1 $filename | grep "Average" | sed -E 's/[^0-9. ]*//g' | sed 's/ //g' >> "./csv/ker_vector/$savefile.csv"
		wait $!
	done
done
