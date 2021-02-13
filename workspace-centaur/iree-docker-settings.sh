#!/bin/bash

cd ~
apt-get update
wait

apt-get install -y clang ninja-build
wait

git clone https://github.com/llvm/llvm-project.git
mkdir llvm-project/build
cd llvm-project/build
wait

apt-get install -y lld
wait
cmake -G Ninja ../llvm \
   -DLLVM_ENABLE_PROJECTS=mlir \
   -DLLVM_BUILD_EXAMPLES=ON \
   -DLLVM_TARGETS_TO_BUILD="X86;NVPTX;AMDGPU" \
   -DCMAKE_BUILD_TYPE=Release \
   -DLLVM_ENABLE_ASSERTIONS=ON \
wait
cmake --build . --target check-mlir
wait

cd ~
git clone https://github.com/google/iree

cd ~/iree
git submodule update --init
wait

cmake -G Ninja -B ../iree-build/ -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ .
wait

cmake --build ../iree-build/
