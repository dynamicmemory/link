#!/bin/bash 

cd build
rm -rf *
cmake .. -Dpybind11_DIR=$(python -m pybind11 --cmakedir)
cmake --build .
