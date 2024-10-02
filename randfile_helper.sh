#!/bin/bash

echo "Compiling..."
gcc randfile.c -o randfile
read -p "Press enter to continue"

echo "Removing previous test file"
rm files.txt
read -p "Press enter to continue"

echo "Listing contents:"
touch files.txt
cat files.txt
read -p "Press enter to continue"

echo "Collecting files with filter"
./randfile /home/dh/Downloads/ .mp4
read -p "Press enter to continue"

echo "Listing results"
cat files.txt
read -p "Press enter to continue"
