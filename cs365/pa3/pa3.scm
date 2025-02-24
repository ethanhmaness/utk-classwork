; Calculates the length of a list
(define (list-len l)
    ; Empty list has length 0
    ; Otherwise remove the first element, length is 1 plus the length of the remaining list
    (if (null? l)
        0
        (+ (list-len (cdr l)) 1)
    )
)

; Creates list of integers from 1 to n
(define (inc-list n)
    ; Function for recursively building reverse-order list
    (define (rev-inc-list a)
        ; If a = 0, list is empty
        ; Otherwise append a to list and call with next lowest integer
        (if (= a 0) 
            '()
            (cons a (rev-inc-list (- a 1)))
        )
    )
    ; Un-reverse the list
    (reverse (rev-inc-list n))
)

; Reverses a list
(define (rev-list l)
    ; Empty list reversed is empty list
    ; Otherwise move first element to last element, reverse the rest of the list
    (if (null? l)
        '()
        (append (rev-list (cdr l)) (list (car l)))
    )
)

; Applies a unary function f to every element of list l
(define (my-map f l)
    ; Mapping empty list on any function is still empty list
    ; Otherwise apply function to first element, then map the rest of the list
    (if (null? l)
        '()
        (append
            (list (apply f (list (car l))))
            (my-map f (cdr l))
        )
    )
)

; Sorts a list using merge sort
(define (merge-sort l)
    ;; Split a list into two halves, returned in a pair. You may uncomment this.
    (define (split l)
        (define (split-rec pair)
            (let ((front (car pair)) (back (cdr pair)))
                (if (>= (length front) (length back))
                    pair
                    (split-rec (cons (append front (list (car back))) (cdr back)))
                )
            )
        )
        (split-rec (cons (list (car l)) (cdr l)))
    )

    ; Merges two SORTED lists by repeatedly taking the first element of whichever list has the lower first element
    (define (merge a b)
        (if (null? a)
            b
            (if (null? b)
                a
                (if (< (car a) (car b))
                    (cons (car a) (merge (cdr a) b))
                    (cons (car b) (merge (cdr b) a))
                )
            )
        )
    )

    ; The actual merge-sort routine
    ; Splits a list, calls merge-sort on the halves, merges them back together
    (if (null? l)
        '()
        (if (null? (cdr l))
            l
            (let ((s (split l)))
              (let ((a (car s)) (b (cdr s)))
                (merge (merge-sort a) (merge-sort b))
                )
            )
        )
    )
)
