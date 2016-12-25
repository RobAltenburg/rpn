(use fmt numbers posix)

;; error handler {{{1
(define (check thunk y)
  (condition-case (thunk)
                  [(exn arithmetic) (fmt #t "Arithmetic Error" nl) y]
                  [(exn) (fmt #t "Other error") y]))
;; }}}

;;; constants {{{1
(define PI 3.141592653589793)
(define E 2.7182818284590452)
(define DEG2RAD (/ PI 180))
(define GRD2RAD (/ PI 200))
;;; }}}

;;; behavior {{{1
(define scale 5)
(define rpn-radix 10)
(define drg DEG2RAD)
(define memory (make-vector 11 0))
;;; }}}

;;; stack functions {{{1
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
;;; }}}

(define (process command stack)
  (let ((x (nz-car stack))
        (y (nz-car (nz-cdr stack)))
        (z (nz-car (nz-cdr (nz-cdr stack)))))
    (cond

      ;; operations {{{1
        [(equal? command "+") (loop (cons (+ y x) (nz-cdr stack 2)))]
        [(equal? command "-") (loop (cons (- y x) (nz-cdr stack 2)))]
        [(equal? command "*") (loop (cons (* y x) (nz-cdr stack 2)))]
        [(equal? command "/") (loop (cons 
             (check (lambda () (/ y x)) y) (nz-cdr stack 2)))]
        [(equal? command "chs") (loop (cons (* -1 x) (nz-cdr stack 2)))]
        [(or (equal? command "1/x") 
             (equal? command "inv")) (loop (cons 
                (check (lambda () (/ 1 x)) y) (nz-cdr stack 2)))]
        [(equal? command "sqrt") (loop (cons (sqrt x) (nz-cdr stack)))]
        [(or (equal? command "pow") 
             (equal? command "**") 
             (equal? command "expt")) (loop (cons (expt y x) (nz-cdr stack 2)))]
        [(equal? command "abs") (loop (cons (abs x) (nz-cdr stack)))]
        [(equal? command "log") (loop (cons (log x) (nz-cdr stack)))]
        [(equal? command "log10") (loop (cons (log x 10) (nz-cdr stack)))]
        [(equal? command "logx") (loop (cons (log y x) (nz-cdr stack 2)))]
        [(equal? command "exp") (loop (cons (exp x) (nz-cdr stack)))]
        [(equal? command "sin") (loop (cons (sin (* drg x)) (nz-cdr stack)))]
        [(equal? command "cos") (loop (cons (cos (* drg x)) (nz-cdr stack)))]
        [(equal? command "tan") (loop (cons 
                (check (lambda () (tan (* drg x))) x) (nz-cdr stack )))]
        [(equal? command "asin") (loop (cons (/ (asin x) drg) (nz-cdr stack)))]
        [(equal? command "acos") (loop (cons (/ (acos x) drg) (nz-cdr stack)))]
        [(equal? command "atan") (loop (cons (/ (atan x) drg) (nz-cdr stack)))]
        [(equal? command "atan2") (loop (cons (/ (atan y x) drg) (nz-cdr stack 2)))]
        [(equal? command "mod") (loop (cons 
                (check (lambda () (modulo y x)) y) (nz-cdr stack 2)))]

        ;; }}}

      ;; conversions {{{1
        [(or (equal? command "dms->deg") (equal? command "hms->hr"))
            (loop (cons (+ x (/ y 60) (/ z 3600)) (nz-cdr stack 3)))]
        [(or (equal? command "deg->dms") (equal? command "hr->hms")) 
            (let* ((fx (floor (abs x)))
                   (fy (floor (* 60 (- (abs x) fx))))
                   (fz (- (abs x) fx (/ fy 60))))
                (loop (append (list fx fy (* 3600 fz)) (nz-cdr stack))))]
      ;; }}}

        ;; list operations {{{1
        [(equal? command "sum") (loop (list (fold + 0 stack)))]
        [(equal? command "product") (loop (list (fold * 0 stack)))]
        [(equal? command "reverse") (loop (reverse stack))]
        ;; }}}

        ;; constatants {{{1
        [(equal? command "pi") (loop (cons PI stack))]
        [(equal? command "e") (loop (cons E stack))]
        ;; }}}

        ;; memory {{{1
        ;; valid slots are 0..10  other references are mod 10
        ;; using y(ank) or p(ut) defaults to slot 0
        [(equal? command "y") (vector-set! memory 0 x) (loop stack)]
        [(equal? command "yx") (vector-set! memory (modulo x 10) y) (loop (nz-cdr stack))]
        [(equal? command "p") (loop (cons (vector-ref memory 0) stack))]
        [(equal? command "px") (loop (cons (vector-ref memory (modulo x 10)) (nz-cdr stack)))]
        ;; }}}

        ;; change behavior {{{1
        [(equal? command "scale") (set! scale x) (loop (nz-cdr stack))]
        [(equal? command "radix") (set! rpn-radix x) (loop (nz-cdr stack))]
        [(equal? command "bin") (set! rpn-radix 2) (loop stack)]
        [(equal? command "hex") (set! rpn-radix 16) (loop stack)]
        [(equal? command "dec") (set! rpn-radix 10) (loop stack)]
        [(equal? command "rad") (set! drg 1) (loop stack)]
        [(equal? command "deg") (set! drg DEG2RAD) (loop stack)]
        [(equal? command "grd") (set! drg GRD2RAD) (loop stack)]
        ;; }}}

        ;; directly manipulate stack {{{1
        [(equal? command "inexact") (loop (map exact->inexact stack))]
        [(equal? command "r") (loop (nz-cdr stack))] ;; roll or left shift the list
        [(equal? command "c") (loop '(0))] ;; clear stack
        [(equal? command "car") (loop (list (car stack)))] ;; first 
        [(equal? command "cdr") (loop (nz-cdr stack))] ;; rest
        ;; }}}

        [else  ;; exit
               (fmt #t "========" nl (radix rpn-radix (fix scale (exact->inexact (nz-car stack)))) nl)]
    )))

;; main loop {{{1
(define (loop stack)
  (when (terminal-port? (current-input-port))
      (fmt #t (radix rpn-radix 
                     (fix scale 
                          (pretty 
                            (map exact->inexact stack)))) "> "))
  (let ((line (read-line)))
    (if (eof-object? line)
        (fmt #t (radix rpn-radix (fix scale (exact->inexact (nz-car stack)))) nl)
        (let ((val (string->number line)))
            (if val
                (loop (cons val stack))
                (process line stack))))))
;; }}}

(loop '())

