#!/usr/bin/env bash

# Your solution for part 4 goes here.
for FILE in *.sample;
do
    awk '{
        if (NR != 2)
        {
            printf($3 " ");
        }
        }' < "$FILE"
    echo ""
done | sort -n | cut -d " " -f 2 | tr -d '\n' > sequence.txt
fold -w1 sequence.txt | awk '
    /A/ {a++;}
    /T/ {t++;}
    /C/ {c++;}
    /G/ {g++;}
    END {
        print "A " a;
        print "T " t;
        print "C " c;
        print "G " g;
    }' > pairs.txt
fold -w3 sequence.txt | awk '
    {counts[$1]++; sum++;}
    END {
        for (key in counts)
        {
            printf("%s %lf%\n", key, 100*counts[key]/sum);
        }
    }' | sort > codons.txt