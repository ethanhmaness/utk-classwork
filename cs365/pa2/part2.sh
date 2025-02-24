#!/usr/bin/env bash

# Your solution for part 2 goes here.
max_hp=0
best_make=""
while read -r line;
do
    if [ "$line" = "Make,Model,MPG,Cylinders,Displacement,Horsepower,Weight,Acceleration,Model,Origin" ];
    then
        continue
    fi
    make=$(echo $line | cut -d "," -f 1)
    hp=$(echo $line | cut -d "," -f 6)
    if [ "$(echo "$hp > $max_hp" | bc)" == 1 ];
    then
        max_hp=$hp
        best_make=$make
    fi
done < cars.csv
echo "${best_make} ${max_hp}" > answer.txt