#!/bin/bash

rm *.o *.a
g++ -O2 -c *.cpp
ar rc libraknet.a *.o
rm *.o


