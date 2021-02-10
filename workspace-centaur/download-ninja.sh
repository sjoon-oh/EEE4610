#!/bin/bash

wget https://github.com/ninja-build/ninja/releases/download/v1.10.2/ninja-linux.zip
unzip ninja-linux.zip
export PATH="`pwd`:$PATH"
which ninja
