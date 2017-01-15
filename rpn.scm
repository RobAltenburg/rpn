;;;
;;; rpn
;;; A lightweight, command-line, rpn calculator
;;;
;;; Rob Altenburg -- 12/2016
;;;

(use fmt numbers posix ncurses mathh)

;;; Utility {{{1
(define-syntax alist-keys
  (syntax-rules ()
    ((_ alist) (map car alist))))

(define (nz-car stack)
  "car that ensures a value is returned"
  (if (equal? '() stack)
    0
    (car stack)))

(define (nz-cdr stack #!optional (depth 1))
  "cdr that works on an empty stack"
  (cond
    [(equal? '() stack) '()]
    [(equal? 1 depth) (cdr stack)]
    [else (nz-cdr (cdr stack) (- depth 1))]))

;;;}}}

;;; Help {{{1
(define (print-help-string)
  (when is-terminal? ;; only show help in interactive mode
  (move 4 0)
  (printw "Constants: ~A~%" (alist-keys constants))
  (printw "Uniary Operators (x -> x): ~%   ~A~%" (alist-keys uniary-operators))
  (printw "Binary Operators (x y -> x): ~A~%" (alist-keys binary-operators))
  (printw "Trinary Operators (x y z -> x): ~A~%" (alist-keys trinary-operators))
  (printw "Stack Operators (stack -> stack): ~%   ~A~%" (alist-keys stack-operators))
  (printw "Non-Stack Operators (x -> void): ~A~%" (alist-keys non-stack-operators))
  (printw "Other Operators: (deg2dms y x scale radix)~%")
  (refresh)))
;;;}}}

;;; Error Handler {{{1
(define (check thunk y)
  (if is-terminal?
	(begin (clr-error)
  (condition-case (thunk)
                  [(exn arithmetic) (printw "Arithmetic Error") (refresh) y]
                  [(exn) (printw "Other error") (refresh) y])))
  (condition-case (thunk)
                  [(exn arithmetic) (display "Arithmetic Error") (newline) y]
                  [(exn) (display "Other error") (newline) y]))
;; }}}

;;; Constants {{{1
(define PI 3.141592653589793)
(define E 2.7182818284590452)
(define DEG2RAD (/ PI 180))
(define GRD2RAD (/ PI 200))
;;; }}}

;;; Global Behavior Settings {{{1
(define scale 5)
(define rpn-radix 10)
(define drg DEG2RAD)
(define xl #f) ; exit with full stack
(define memory (make-vector 11 0))
;;; }}}

;;; Operator a-lists (most functions are defined here) {{{1
(define constants
  `((pi ,PI)
    (pi2 ,(/ PI 2))
    (e ,E)
    (p (vector-ref memory 0)) ; memory recall
    ))

(define uniary-operators
  `((log ,log)
    (log10 ,log10) ; mathh
    (log2 ,log2) ; mathh
    (abs ,abs)
    (exp ,exp)
    (sqrt ,sqrt)
    (gamma ,gamma) ; mathh
    (! ,(lambda (x) (gamma (+ x 1))))
    (chs ,(lambda (x) (* -1 x)))
    (sin ,(lambda (x) (sin (* drg x))))
    (cos ,(lambda (x) (cos (* drg x))))
    (tan ,(lambda (x) (tan (* drg x))))
    (asin ,(lambda (x) (/ (asin x) drg)))
    (acos ,(lambda (x) (/ (acos x) drg)))
    (atan ,(lambda (x) (/ (atan x) drg)))
    (inv ,(lambda (x) (/ 1 x)))
    (px ,(lambda (x) (vector-ref memory (modulo x 10)))) ; memory recall
    ))

(define binary-operators
  `((+ ,+)
    (- ,-)
    (* ,*)
    (/ ,/)
    (expt ,expt)
    (pow ,expt)
    (logx ,log)
    (mod ,modulo)
    (eex ,(lambda (x y) (* x (expt 10 y))))
    (atan2 ,(lambda (x y) (/ (atan y x) drg)))))

(define trinary-operators
  `((dms2deg ,(lambda (d m s) (+ d (/ m 60) (/ s 360))))))

(define stack-operators
  `((sum ,(lambda (stk) (list (fold + 0 stk))))
    (product ,(lambda (stk) (list (fold * 1 stk))))
    (mean ,(lambda (stk) (list (/ (fold + 0 stk) (length stk)))))
    (reverse ,(lambda (stk) (reverse stk)))
    (inexact ,(lambda (stk) (map exact->inexact stk)))
    (r ,(lambda (stk) (nz-cdr stk))) ;; roll or left shift the stack
    (c ,(lambda (stk) '(0))) ;; clear stack
    (car ,(lambda (stk) (list (car stk)))) ;; first
    (cdr ,(lambda (stk) (nz-cdr stk))))) ;; rest

(define non-stack-operators
   `((bin (set! rpn-radix 2))
     (oct (set! rpn-radix 8))
     (dec (set! rpn-radix 10))
     (hex (set! rpn-radix 16))
     (rad (set! drg 1))
     (deg (set! drg DEG2RAD))
     (grd (set! drg GRD2RAD))
     (xl (set! xl (not xl)))
     (help (print-help-string))))

;; extract the operator symbols from the alists and
;; include the special-case symbols
(define operator-symbols
(append (car (map (lambda (alist) (map car alist))
            (list (append constants
                   uniary-operators
                   binary-operators
                   trinary-operators
                   stack-operators
                   non-stack-operators))))
     '(deg2dms y x scale radix)))

;;; }}}

;;; Process Commands {{{1
(define (process command stack)
  (let ((x (nz-car stack))
        (y (nz-car (nz-cdr stack)))
        (z (nz-car (nz-cdr (nz-cdr stack)))))
    (cond
	[(equal? command "") stack] ;; nop

        [(alist-ref (string->symbol command) constants) =>
                (lambda (c-list) (cons (eval (car c-list)) stack))]

        [(alist-ref (string->symbol command) uniary-operators) =>
                (lambda (f-list) (cons
                                   (check (lambda () ((car f-list) x)) y)
                                   (nz-cdr stack)))]

        [(alist-ref (string->symbol command) binary-operators) =>
                (lambda (f-list) (cons
                                   (check (lambda () ((car f-list) y x)) y)
                                   (nz-cdr stack 2)))]

        [(alist-ref (string->symbol command) trinary-operators) =>
                (lambda (f-list) (cons
                                   (check (lambda () ((car f-list) x y z)) z)
                                   (nz-cdr stack 3)))]

        [(alist-ref (string->symbol command) stack-operators) =>
                (lambda (f-list) ((car f-list) stack))]

        [(alist-ref (string->symbol command) non-stack-operators) =>
                (lambda (f-list) (eval (car f-list)) stack)]

        ;; Special Cases
        ;; deg2dms is a special case uniary operator since it returns multiple values
        [(or (equal? command "deg2dms") (equal? command "hr2hms"))
            (let* ((fx (floor (abs x)))
                   (fy (floor (* 60 (- (abs x) fx))))
                   (fz (- (abs x) fx (/ fy 60))))
                (append (list fx fy (* 3600 fz)) (nz-cdr stack)))]

        ;; memory
        ;; valid slots are 0..10  other references are mod 10
        ;; using y(ank) or p(ut) defaults to slot 0
        ;; the puts are not special cases
        [(equal? command "y") (vector-set! memory 0 x) stack]
        [(equal? command "yx") (vector-set! memory (modulo x 10) y) (nz-cdr stack)]

        ;; change behavior
        [(equal? command "scale") (set! scale x) (nz-cdr stack)]
        [(equal? command "radix") (set! rpn-radix x) (nz-cdr stack)]

        [else  ;; bad command
	   (if is-terminal?
	       (begin (clr-error) (printw "bad command") (refresh))
	       (begin (display "bad command") (newline)))
               stack]
    )))

;;}}}

;;; ncurses Routines {{{1
(define (clr-stack)
  (move 1 1)
  (clrtoeol))

(define (clr-entry)
  (move 2 2)
  (clrtoeol))

(define (clr-error)
  (move 3 2)
  (clrtoeol))

(define (read-char-ncurses stack #!optional (str ""))
"allow certain operations to avoid the need to hit [return]"
  (clr-entry) (printw str) (refresh)
  (let ((char (getch)))
	(cond
	  [(equal? #\return char) (cons str stack)]
	  [(equal? #\delete char)
				(read-char-ncurses stack
				  (string-drop-right str 1))]
	  [(equal? #\space char) (cons str stack)]
	  [(equal? #\+ char) (cons "+" (if (equal? "" str) stack (dispatch str stack)))]
	  [(equal? #\* char) (cons "*" (if (equal? "" str) stack (dispatch str stack)))]
	  [(equal? #\/ char) (cons "/" (if (equal? "" str) stack (dispatch str stack)))]
	  [(equal? #\- char) (if (equal? "" str)
		                (read-char-ncurses stack "-")
                                (cons "-" (dispatch str stack)))]
	  [(equal? #\! char) (cons "!" (if (equal? "" str) stack (dispatch str stack)))]
	  [(equal? #\q char) (cons "q" stack)]
	  [else
		(clr-error)
		(read-char-ncurses stack (string-append str (string char)))])))
;; }}}

;;; Dispatcher {{{1
(define (dispatch line stack)
  "if a number is input, cons it to the stack
   else, assume it's a command and process it"
    (let ((val (string->number line)))
        (if val
            (cons val stack)
            (process line stack))))
;;; }}}

;;; Main Loop {{{1
(define (loop #!optional (stack '()))
  (when is-terminal?
      (clr-stack)
      (printw (fmt #f (radix rpn-radix
                     (fix scale
                          (pretty
                            (map exact->inexact stack))))))
	  (move 2 0) (clrtoeol) (printw "> ")
      (refresh))
  (let* ((stk (if is-terminal?
				(read-char-ncurses stack)
				(cons (read-line) stack)))
		 (line (nz-car stk)))
	(cond
		[(equal? line "q") stack]
		[(eof-object? line) stack]
        [else (loop (dispatch line (nz-cdr stk)))])))

;; }}}

(define is-terminal? (terminal-port? (current-input-port)))

(initscr) (noecho) (raw) (nonl)
(keypad (stdscr) #t)
(printw "rpn -----------") (refresh)
(define retvar (loop))
(endwin)

(if xl
(fmt #t
     (map exact->inexact retvar))
(fmt #t (radix rpn-radix (fix scale (exact->inexact (nz-car retvar))))))
(newline)
