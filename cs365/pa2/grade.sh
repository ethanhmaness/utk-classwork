#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd $DIR

part="$1"

if [[ -z "$part" ]]; then
    echo "missing argument: must be part1 part2 part3 or part4"
    echo "  for example: ./grade.sh part4"
    exit 1
fi

if [[ ! -f "${part}.sh" ]]; then
    echo "missing script ${part}.sh"
fi

if [[ -d "${part}" ]]; then
    rm -rf "${part}"
fi

cp -r "${part}_start" "${part}"
cp "${part}.sh" "${part}"
cd "${part}"
./${part}.sh
rm "${part}.sh"

cd $DIR

while read f; do
    match="${part}/${f/${part}_ref\//}"
    if [[ ! -f "${match}" ]]; then
        echo "[!] missing file ${match}"
        fail="yes"
    else
        if ! diff "$f" "$match" > /dev/null; then
            echo "[!] files ${f} and ${match} differ"
            fail="yes"
        fi
    fi
done < <(find "${part}_ref" -type f)

while read f; do
    match="${part}_ref/${f/${part}\//}"
    if [[ ! -f "${match}" ]]; then
        echo "[!] extraneous file ${f}"
        fail="yes"
    fi
done < <(find "${part}" -type f)

if [[ "$fail" == "yes" ]]; then
    echo "${part} FAIL"
    exit 1
else
    echo "${part} PASS"
fi
