#!/usr/bin/env bash
python3 matrix.py "$1"

echo "Single process"
for ((i=1; i<=5; i ++)); do
    ./cg "$1"
done

echo "Multithreading"
for ((i=1; i<=5; i ++)); do
    ./cg-multi "$1"
done