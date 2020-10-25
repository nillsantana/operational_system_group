#!/bin/bash

for i in 1 2 4 8 16 32; do
  for j in $(seq 1 10); do
            ./multmatriz.o $i 1000 1000
#    ./calcpi.o $i
  done
done
