#!/usr/bin/env bash

if [ -t 1 ]; then
    CBLACK=$'\e[30m'
    CRED=$'\e[31m'
    CGREEN=$'\e[32m'
    CYELLOW=$'\e[33m'
    CBLUE=$'\e[34m'
    CMAGENTA=$'\e[35m'
    CCYAN=$'\e[36m'
    CWHITE=$'\e[37m'
    CRESET=$'\e[0m'
fi

test_cases=$(cat <<-END

(display (list-len '())) (newline)	0
(display (list-len '(1))) (newline)	1
(display (list-len '(1 2 3))) (newline)	3
(display (list-len '(2 3 5 7 11 13 17 19 23 29))) (newline)	10
(display (list-len '(() () (1 2 ())))) (newline)	3

(display (inc-list 0)) (newline)	()
(display (inc-list 1)) (newline)	(1)
(display (inc-list 2)) (newline)	(1 2)
(display (inc-list 5)) (newline)	(1 2 3 4 5)
(display (inc-list 10)) (newline)	(1 2 3 4 5 6 7 8 9 10)

(display (rev-list '())) (newline)	()
(display (rev-list '(1))) (newline)	(1)
(display (rev-list '(1 2 3))) (newline)	(3 2 1)
(display (rev-list '(2 3 5 7 11 13 17 19 23 29))) (newline)	(29 23 19 17 13 11 7 5 3 2)
(display (rev-list '(() (1) (1 2 ())))) (newline)	((1 2 ()) (1) ())

(display (my-map (lambda (x) x) '())) (newline)	()
(display (my-map (lambda (x) x) '(1))) (newline)	(1)
(display (my-map (lambda (x) (* x x)) '(1 2 3 4 5))) (newline)	(1 4 9 16 25)
(display (my-map length '(() (1) (1 2 3) (2 3 5 7 11 13 17 19 23 29)))) (newline)	(0 1 3 10)
(display (my-map (lambda (x) (map (lambda (y) (* y y)) x)) '(() (1) (1 2 3) (2 3 5 7 11 13 17 19 23 29)))) (newline)	(() (1) (1 4 9) (4 9 25 49 121 169 289 361 529 841))

(display (merge-sort '())) (newline)	()
(display (merge-sort '(1))) (newline)	(1)
(display (merge-sort '(2 3 1 4))) (newline)	(1 2 3 4)
(display (merge-sort '(23 3 7 5 2 13 17 19 11 29))) (newline)	(2 3 5 7 11 13 17 19 23 29)
(display (merge-sort '(62 65 30 56 68 44 45 23 27 79 72 73 25 70 80 20 55 63 78 1))) (newline)	(1 20 23 25 27 30 44 45 55 56 62 63 65 68 70 72 73 78 79 80)

END

)

n=0
i=0
while IFS= read -r line; do
    [[ "$line" == "" ]] && continue

    n=$(($n + 1))

    test=$(echo "$line" | awk -F '	' '{ print $1; }')
    ans=$(echo "$line" | awk -F '	' '{ print $2; }')

    echo "${CCYAN}${test}${CRESET}"

    if check=$(timeout 3s guile -l pa3.scm -c "$test" 2>&1); then
        check=$(echo "$check" | sed '/^;;;/d')

        if diff_out=$(diff -y <(echo "$ans") <(echo "$check")); then
            echo "  ${CGREEN}[ PASS ]${CRESET}"
            i=$(($i + 1))
        else
            echo "  ${CRED}[ FAIL ] (displaying diff: CORRECT | INCORRECT)${CRESET}"
            echo "$diff_out" | sed 's/^/  /'
        fi
    else
        check=$(echo "$check" | sed '/^;;;/d')

        if echo "$check" | grep "ERROR:" 2>&1 > /dev/null; then
            echo "  ${CRED}[ FAIL ] (Guile error)${CRESET}"
            echo "$check" | sed 's/^/  /'
        else
            echo "  ${CRED}[ FAIL ] (took longer than 3 seconds)${CRESET}"
            echo "    are you missing a recursion base case?"
        fi
    fi
done <<< "$test_cases"

printf "${CYELLOW}SUMMARY: %d/%d correct\n${CRESET}" $i $n
