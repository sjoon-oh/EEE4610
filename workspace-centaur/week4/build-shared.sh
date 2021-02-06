#!/bin/bash

echo "cuSPARSE testbench compilation..."
nvcc -w tb_ker_vector_shared.cu mmio.cu -o spmv-ker-vector-shared -lcusparse
echo "Done."

echo "Execution test"
if [ -e s512.tar.gz ]; then
	echo "s512.tar.gz exists.Skipping download."
else
	wget --no-check-certificate 'https://docs.google.com/uc?export=download&id=1IC2fyFspFe7H8FAOjUrwKhkUkB1B6lq7' -O s512.tar.gz
fi
wait
echo "Finished downloading."
echo "Extract start."
tar -xvf s512.tar.gz
echo "Done extracting."
echo "Removing unnecessary files."

rm s512-r5.000000-mat.mtx
rm s512-r10.000000-mat.mtx
rm s512-r15.000000-mat.mtx
rm s512-r20.000000-mat.mtx
rm s512-r25.000000-mat.mtx
rm s512-r30.000000-mat.mtx
rm s512-r35.000000-mat.mtx
# rm s512-r40.000000-mat.mtx
rm s512-r45.000000-mat.mtx
rm s512-r50.000000-mat.mtx
rm s512-r55.000000-mat.mtx
rm s512-r60.000000-mat.mtx
rm s512-r65.000000-mat.mtx
rm s512-r70.000000-mat.mtx
rm s512-r75.000000-mat.mtx

rm s512.tar.gz
wait
echo "Done."
echo "Running spmv-ker-vector"
./spmv-ker-vector-shared 1 s512-r40.000000-mat.mtx | grep Elapsed
echo "Done."


