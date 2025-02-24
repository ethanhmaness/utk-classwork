#!/usr/bin/env bash

# Your solution for part 1 goes here.
for FILE in *.txt;
do
    name=$(basename $FILE .txt)
    mkdir $name
    awk -v dir=$name '{print $2 > (dir "/" $1 ".txt")}' < $FILE
    rm $FILE
done