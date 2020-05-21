#!/bin/bash

clang-format $1 > aux.c
cp aux.c $1
rm aux.c
