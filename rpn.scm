(use fmt numbers)

(define PI 3.141592653589793)

(define (nz-cdr stack #!optional (depth 1))
  (cond
    [(equal? 1 (length stack)) '(0)]
    [(equal? 1 depth) (cdr stack)]
    [else (nz-cdr (cdr stack) (- depth 1))]))

(define (rad->deg x)
  (* (/ 180 PI) x))


(define (process command stack)
  (let ((x (car stack))
        (y (car (nz-cdr stack))))
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
        [(equal? command "sin") (loop (cons (sin (rad->deg x)) (nz-cdr stack)))]
        [(equal? command "cos") (loop (cons (cos (rad->deg x)) (nz-cdr stack)))]
        [(equal? command "tan") (loop (cons (tan (rad->deg x)) (nz-cdr stack)))]
        [(equal? command "asin") (loop (cons (rad->deg (asin x)) (nz-cdr stack)))]
        [(equal? command "acos") (loop (cons (rad->deg (acos x)) (nz-cdr stack)))]
        [(equal? command "atan") (loop (cons (rad->deg (atan x)) (nz-cdr stack)))]
        
        [(equal? command "r") (loop (nz-cdr stack))] ;; roll or left shift the list
        [else
               (fmt #t (exact->inexact (car stack)) nl)]
    )))

(define (loop stack)
  (fmt #t stack nl "> ")
  (let* ((line (read-line))
         (val (string->number line)))
    (if val  
       (loop (cons val stack))
       (process line stack))))

(loop '(0))

