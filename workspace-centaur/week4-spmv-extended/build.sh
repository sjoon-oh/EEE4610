#!/bin/bash

nvcc -w *.cu -o spmv-week4 -lcusparse
