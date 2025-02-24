#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd $DIR

input=$1

if [[ -z $input ]]; then
    echo "missing argument: must be either 'all' or the path to an input file"
    echo "  for example: ./grade.sh all"
    echo "               ./grade.sh test/1/input1"
    exit 1
fi

function grade_one {
    input_path="$1"
    input_dir="$(dirname "$input_path")"
    level="$input_dir/level"
    r="${input_path}.ref"
    t="${input_path}.test"
    d="${input_path}.diff"
    mono ./etg_ref.exe "$level" < "$input_path" > "$r"
    mono ./etg.exe     "$level" < "$input_path" > "$t"
    if diff "$r" "$t" > "$d"; then
        echo "input $input_path PASSED"
        return 1
    else
        echo "input $input_path DOES NOT MATCH: created $d"
        if [[ "$input" != "all" ]]; then
            echo "You can run the following command to compare outputs:"
            echo "  vimdiff $r $t"
        fi
        return 0
    fi
}

if [[ "$input" == "all" ]]; then
    tot=0
    cor=0
    for d in test/*; do
        for i in $(find $d -regex "$d/input[0-9]" | sort); do
            tot=$(($tot + 1))
            grade_one "$i"
            cor=$(($cor + $?))
        done
    done
    echo "$cor out of $tot inputs passed."
else
    grade_one "$input"
fi
