#!/bin/bash
make
str="${1}"
left_number=$2
right_number=$3
detectMethod="${4}"
./bin/mpfrCodeGene.exe "${str}" "${detectMethod}"
echo "------------------------------------------------------------"
echo "| step1: generate high-precision version code successfully |"
echo "------------------------------------------------------------"
cmake -B build
cmake --build build -j `nproc`

./build/ErrorDetection $2 $3 "${4}"

