#!/usr/bin/env bash

# Your solution for part 3 goes here.
awk -F',' '/.*/ {
    if (NR != 1)
    {
        make = $1;
        hp = int($6);
        if (hp > max_hp)
        {
            max_hp = hp;
            best_make = make;
        }
    }
}
END {printf("echo %s %lf > answer.txt", best_make, max_hp) | "sh"}' < cars.csv