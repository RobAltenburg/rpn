;;;
;;; rpn
;;; A lightweight, command-line, rpn calculator
;;;
;;; Rob Altenburg -- 12/2016
;;;

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
        [(equal? command "+") (cons (+ y x) (nz-cdr stack 2))]
        [(equal? command "-") (cons (- y x) (nz-cdr stack 2))]
        [(equal? command "*") (cons (* y x) (nz-cdr stack 2))]
        [(equal? command "/") (cons 
             (check (lambda () (/ y x)) y) (nz-cdr stack 2))]
        [(equal? command "chs") (cons (* -1 x) (nz-cdr stack 2))]
        [(or (equal? command "1/x") 
             (equal? command "inv")) (cons 
                (check (lambda () (/ 1 x)) y) (nz-cdr stack 2))]
        [(equal? command "sqrt") (cons (sqrt x) (nz-cdr stack))]
        [(or (equal? command "pow") 
             (equal? command "**") 
             (equal? command "expt")) (cons (expt y x) (nz-cdr stack 2))]
        [(equal? command "abs") (cons (abs x) (nz-cdr stack))]
        [(equal? command "log") (cons (log x) (nz-cdr stack))]
        [(equal? command "log10") (cons (log x 10) (nz-cdr stack))]
        [(equal? command "logx") (cons (log y x) (nz-cdr stack 2))]
        [(equal? command "exp") (cons (exp x) (nz-cdr stack))]
        [(equal? command "sin") (cons (sin (* drg x)) (nz-cdr stack))]
        [(equal? command "cos") (cons (cos (* drg x)) (nz-cdr stack))]
        [(equal? command "tan") (cons 
                (check (lambda () (tan (* drg x))) x) (nz-cdr stack ))]
        [(equal? command "asin") (cons (/ (asin x) drg) (nz-cdr stack))]
        [(equal? command "acos") (cons (/ (acos x) drg) (nz-cdr stack))]
        [(equal? command "atan") (cons (/ (atan x) drg) (nz-cdr stack))]
        [(equal? command "atan2") (cons (/ (atan y x) drg) (nz-cdr stack 2))]
        [(equal? command "mod") (cons 
                (check (lambda () (modulo y x)) y) (nz-cdr stack 2))]

        ;; }}}

      ;; conversions {{{1
        [(or (equal? command "dms->deg") (equal? command "hms->hr"))
            (cons (+ x (/ y 60) (/ z 3600)) (nz-cdr stack 3))]
        [(or (equal? command "deg->dms") (equal? command "hr->hms")) 
            (let* ((fx (floor (abs x)))
                   (fy (floor (* 60 (- (abs x) fx))))
                   (fz (- (abs x) fx (/ fy 60))))
                (append (list fx fy (* 3600 fz)) (nz-cdr stack)))]
      ;; }}}

        ;; list operations {{{1
        [(equal? command "sum") (list (fold + 0 stack))]
        [(equal? command "product") (list (fold * 0 stack))]
        [(equal? command "reverse") (reverse stack)]
        ;; }}}

        ;; constatants {{{1
        [(equal? command "pi") (cons PI stack)]
        [(equal? command "e") (cons E stack)]
        ;; }}}

        ;; memory {{{1
        ;; valid slots are 0..10  other references are mod 10
        ;; using y(ank) or p(ut) defaults to slot 0
        [(equal? command "y") (vector-set! memory 0 x) stack]
        [(equal? command "yx") (vector-set! memory (modulo x 10) y) (nz-cdr stack)]
        [(equal? command "p") (cons (vector-ref memory 0) stack)]
        [(equal? command "px") (cons (vector-ref memory (modulo x 10)) (nz-cdr stack))]
        ;; }}}

        ;; change behavior {{{1
        [(equal? command "scale") (set! scale x) (nz-cdr stack)]
        [(equal? command "radix") (set! rpn-radix x) (nz-cdr stack)]
        [(equal? command "bin") (set! rpn-radix 2) stack]
        [(equal? command "hex") (set! rpn-radix 16) stack]
        [(equal? command "dec") (set! rpn-radix 10) stack]
        [(equal? command "rad") (set! drg 1) stack]
        [(equal? command "deg") (set! drg DEG2RAD) stack]
        [(equal? command "grd") (set! drg GRD2RAD) stack]
        ;; }}}

        ;; directly manipulate stack {{{1
        [(equal? command "inexact") (map exact->inexact stack)]
        [(equal? command "r") (nz-cdr stack)] ;; roll or left shift the list
        [(equal? command "c") '(0)] ;; clear stack
        [(equal? command "car") (list (car stack))] ;; first 
        [(equal? command "cdr") (nz-cdr stack)] ;; rest
        ;; }}}
        
		[else  ;; exit
               (fmt #t "bad command" nl)
               stack]
    )))

;; main loop {{{1
(define (loop stack)
  (when (terminal-port? (current-input-port))
      (fmt #t (radix rpn-radix 
                     (fix scale 
                          (pretty 
                            (map exact->inexact stack)))) "> "))
  (let ((line (read-line)))
	(cond
		[(equal? line "q")
				(fmt #t (radix rpn-radix (fix scale (exact->inexact (nz-car stack)))) nl)]
		[(eof-object? line)
				(fmt #t (radix rpn-radix (fix scale (exact->inexact (nz-car stack)))) nl)]
        [else
		  (let ((val (string->number line)))
            (if val
                (loop (cons val stack))
                (loop (process line stack))))])))
;; }}}

(loop '())

