FROM nvidia/cuda:10.2-devel-ubuntu18.04

RUN apt-get -y -qq update && apt-get -y -qq upgrade
RUN apt-get install -y -qq --no-install-recommends build-essential wget autoconf automake libtool curl unzip vim git zlib1g zlib1g-dev python python3 gcc-6 g++-6 gdb ninja-build libncurses5-dev sudo libncursesw5-dev python3-distutils
RUN apt-get install -y -qq --no-install-recommends software-properties-common && \ 
    rm -rf /var/lib/apt/lists/*

RUN rm /usr/bin/python && ln -s /usr/bin/python3.6 /usr/bin/python

RUN  curl -fsSL http://developer.download.nvidia.com/compute/redist/cudnn/v7.6.5/cudnn-10.2-linux-x64-v7.6.5.32.tgz -O && \
    tar --no-same-owner -xzf cudnn-10.2-linux-x64-v7.6.5.32.tgz -C /usr/local && \
    rm cudnn-10.2-linux-x64-v7.6.5.32.tgz && \
    ldconfig

# install conda, python modules
RUN wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh
RUN sh miniconda.sh -u -b -p ~/miniconda
RUN export PATH=$PATH:/root/miniconda/bin && \
    eval "$(conda shell.bash hook)" && \
    pip install -U torch torchvision onnx onnxruntime pytorch-pretrained-bert scipy pandas tensorboardX scikit-image

# ENV CONDA_BLD_PATH /tmp

WORKDIR /root
RUN chmod -R a+w /root

# install cmake 3.15.4
RUN wget https://github.com/Kitware/CMake/releases/download/v3.15.4/cmake-3.15.4-Linux-x86_64.tar.gz
RUN tar xvzf cmake-3.15.4-Linux-x86_64.tar.gz && cd cmake-3.15.4-Linux-x86_64 && cp -r * /

# install protobuf 3.11.0
RUN wget https://github.com/protocolbuffers/protobuf/releases/download/v3.11.0/protobuf-all-3.11.0.tar.gz
RUN tar xvzf protobuf* && \
    cd protobuf-3.11.0 && \
    ./autogen.sh && \
    ./configure && \
    make && make check && make install && \
    ldconfig

ENV MAKEFLAGS -j8

# install llvm with mlir
#RUN git clone http://git.corelab.or.kr/corelab-xpu/llvm-project && \
COPY llvm-project /root/llvm-project
RUN cd llvm-project && git checkout corelab && cd ..
RUN mkdir llvm-project/build && \
    cd llvm-project/build && \
    cmake -G Ninja ../llvm \
      -DLLVM_ENABLE_PROJECTS="mlir;clang" \
      -DLLVM_BUILD_EXAMPLES=ON \
      -DLLVM_TARGETS_TO_BUILD="host;NVPTX" \
      -DCMAKE_BUILD_TYPE=Debug \
      -DLLVM_ENABLE_ASSERTIONS=ON \
      -DLLVM_ENABLE_ZLIB=OFF \
      -DLLVM_ENABLE_RTTI=ON && \
    cmake --build . --target -- ${MAKEFLAGS} && \
    cmake --build . --target check-mlir

ENV LLVM_PROJ_SRC /root/llvm-project/
ENV LLVM_PROJ_BUILD /root/llvm-project/build
ENV PATH $PATH:/root/llvm-project/build/bin:/root/miniconda/bin:/root/onnx-mlir/build/bin:/root/flexc/build/bin:/root/flexc_dev/build/bin
ENV LIBRARY_PATH /usr/local/nvidia/lib:/usr/local/nvidua/lib64:/root/flexc/build/lib:/root/onnx-mlir/build/lib:$LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH /usr/local/nvidia/lib:/usr/local/nvidua/lib64:/root/flexc/build/lib:/root/onnx-mlir/build/lib:$LD_LIBRARY_PATH

# 
