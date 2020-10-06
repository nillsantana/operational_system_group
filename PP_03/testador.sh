#!/bin/bash

for i in 1 2 4 8 16 32;
#for i in 2;
  do
    for j in $(seq 1 5);
      do
#        echo "Thread $i exec $j"
        ./multmatriz.o $i 100 100
#        ./calcpi.o $i
      done
  done