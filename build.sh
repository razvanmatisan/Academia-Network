#!/bin/bash

# Quickly builds our project for local testing
make clean
make -f Makefile_tema3 clean

make
make -f Makefile_tema3
