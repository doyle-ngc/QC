#!/bin/sh

export MROOT=$PWD/code

make -C code/simp r
cp code/simp/glueminisat-simp-release ./binary/glueminisat-simp
