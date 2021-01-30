export CUDADIR=/usr/local/cuda-11.1 >> ~/.bashrc
export PATH=/usr/local/cuda-11.1/bin${PATH:+:${PATH}} >> ~/.bashrc
export LD_LIBRARY_PATH=/usr/local/cuda-11.1/lib64${LD_LIBRARY_PATH:+:${PATH}} >> ~/.bashrc

# source ~/.bashrc

echo "CUDADIR set to $CUDADIR"
echo "PATH set to $PATH"
echo "LD_LIBRARY_PATH set to $LD_LIBRARY_PATH"

