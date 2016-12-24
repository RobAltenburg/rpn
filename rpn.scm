(use fmt numbers)

(define PI 3.141592653589793)
(define E 2.7182818284590452)
(define DEG2RAD (/ PI 180))
(define GRD2RAD (/ PI 200))

(define scale 5)
(define rpn-radix 10)
(define drg DEG2RAD)

(define memory (make-vector 11 0))

;; make sure the stack is never empty
(define (nz-car stack)
  (if (equal? '() stack)
    0
    (car stack)))

(define (nz-cdr stack #!optional (depth 1))
  (cond
    [(equal? '() stack) '()]
    [(equal? 1 depth) (cdr stack)]
    [else (nz-cdr (cdr stack) (- depth 1))]))

(define (process command stack)
  (let ((x (nz-car stack))
        (y (nz-car (nz-cdr stack))))
    (cond
        [(equal? command "+") (loop (cons (+ y x) (nz-cdr stack 2)))]
        [(equal? command "-") (loop (cons (- y x) (nz-cdr stack 2)))]
        [(equal? command "*") (loop (cons (* y x) (nz-cdr stack 2)))]
        [(equal? command "/") (loop (cons (/ y x) (nz-cdr stack 2)))]
        [(equal? command "chs") (loop (cons (* -1 x) (nz-cdr stack 2)))]
        [(equal? command "1/x") (loop (cons (/ 1 x) (nz-cdr stack 2)))]
        [(equal? command "sqrt") (loop (cons (sqrt x) (nz-cdr stack)))]
        [(equal? command "expt") (loop (cons (expt y x) (nz-cdr stack 2)))]
        [(equal? command "abs") (loop (cons (abs x) (nz-cdr stack)))]
        [(equal? command "log") (loop (cons (log x) (nz-cdr stack)))]
        [(equal? command "log10") (loop (cons (log x 10) (nz-cdr stack)))]
        [(equal? command "exp") (loop (cons (exp x) (nz-cdr stack)))]
        [(equal? command "sin") (loop (cons (sin (* drg x)) (nz-cdr stack)))]
        [(equal? command "cos") (loop (cons (cos (* drg x)) (nz-cdr stack)))]
        [(equal? command "tan") (loop (cons (tan (* drg x)) (nz-cdr stack)))]
        [(equal? command "asin") (loop (cons (/ (asin x) drg) (nz-cdr stack)))]
        [(equal? command "acos") (loop (cons (/ (acos x) drg) (nz-cdr stack)))]
        [(equal? command "atan") (loop (cons (/ (atan x) drg) (nz-cdr stack)))]
        [(equal? command "mod") (loop (cons (modulo y x) (nz-cdr stack 2)))]

        ;; constatants
        [(equal? command "pi") (loop (cons PI stack))]
        [(equal? command "e") (loop (cons E stack))]

        ;; memory
        ;; valid slots are 0..10  other references are mod 10
        [(equal? command "ms") (vector-set! memory (modulo x 10) y) (loop (nz-cdr stack))]
        [(equal? command "m") (loop (cons (vector-ref memory (modulo x 10)) (nz-cdr stack)))]
        
        ;; internals
        [(equal? command "scale") (set! scale x) (loop (nz-cdr stack))]
        [(equal? command "radix") (set! rpn-radix x) (loop (nz-cdr stack))]
        [(equal? command "bin") (set! rpn-radix 2) (loop stack)]
        [(equal? command "hex") (set! rpn-radix 16) (loop stack)]
        [(equal? command "dec") (set! rpn-radix 10) (loop stack)]
        [(equal? command "rad") (set! drg 1) (loop stack)]
        [(equal? command "deg") (set! drg DEG2RAD) (loop stack)]
        [(equal? command "grd") (set! drg GRD2RAD) (loop stack)]

        [(equal? command "r") (loop (nz-cdr stack))] ;; roll or left shift the list
        [(equal? command "c") (loop '(0))] ;; clear stack
        [(equal? command "car") (loop (list (car stack)))] ;; first 
        [(equal? command "cdr") (loop (nz-cdr stack))] ;; rest
        [else
               (fmt #t (radix rpn-radix (fix scale (exact->inexact (nz-car stack)))) nl)]
    )))

(define (loop stack)
  (fmt #t (radix rpn-radix (fix scale (pretty stack))) "> ")
  (let* ((line (read-line))
         (val (string->number line)))
    (if val  
       (loop (cons val stack))
       (process line stack))))

(loop '())

