;;;
;;; rpn
;;; A lightweight, command-line, rpn calculator
;;;
;;; Rob Altenburg -- 12/2016
;;;

(use fmt numbers posix ncurses)


;;; Help {{{1
(define (print-help-string)
  (when is-terminal? ;; only show help in interactive mode
  (move 4 0)
  (printw "Operators: ")
  (printw "+ - * / inv sqrt pow expt abs log log10 logx exp~%")
  (printw "    sin cos tan asin acos atan atan2 mod hms2hr hr2hms ~%")
  (printw "Stack Operations: sum product mean reverse~%")
  (printw "Constants: pi e~%")
  (printw "Memory: y yx p px~%")
  (printw "Display Behavior: scale radix bin hex dec~%")
  (printw "Conversions: rad deg grd~%")
  (printw "Manipulate Stack: inexact reverse r c car cdr~%")
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

;;; Stack Functions {{{1
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

;;; Process Commands {{{1
(define (process command stack)
  (let ((x (nz-car stack))
        (y (nz-car (nz-cdr stack)))
        (z (nz-car (nz-cdr (nz-cdr stack)))))
    (cond
	    [(equal? command "") stack] ;; nop
      ;; operations {{{2
        [(equal? command "+") (cons (+ y x) (nz-cdr stack 2))]
        [(equal? command "-") (cons (- y x) (nz-cdr stack 2))]
        [(equal? command "*") (cons (* y x) (nz-cdr stack 2))]
        [(equal? command "/") (cons 
             (check (lambda () (/ y x)) y) (nz-cdr stack 2))]
        [ (equal? command "inv") (cons 
                (check (lambda () (/ 1 x)) y) (nz-cdr stack 2))]
        [(equal? command "sqrt") (cons (sqrt x) (nz-cdr stack))]
        [(or (equal? command "pow") 
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

      ;; conversions {{{2
        [(or (equal? command "dms2deg") (equal? command "hms2hr"))
            (cons (+ x (/ y 60) (/ z 3600)) (nz-cdr stack 3))]
        [(or (equal? command "deg2dms") (equal? command "hr2hms")) 
            (let* ((fx (floor (abs x)))
                   (fy (floor (* 60 (- (abs x) fx))))
                   (fz (- (abs x) fx (/ fy 60))))
                (append (list fx fy (* 3600 fz)) (nz-cdr stack)))]
      ;; }}}

        ;; list operations {{{2
        [(equal? command "sum") (list (fold + 0 stack))]
        [(equal? command "product") (list (fold * 0 stack))]
        [(equal? command "mean") (list (/ (fold + 0 stack) (length stack)))]
        [(equal? command "reverse") (reverse stack)]
        ;; }}}

        ;; constatants {{{2
        [(equal? command "pi") (cons PI stack)]
        [(equal? command "e") (cons E stack)]
        ;; }}}

        ;; memory {{{2
        ;; valid slots are 0..10  other references are mod 10
        ;; using y(ank) or p(ut) defaults to slot 0
        [(equal? command "y") (vector-set! memory 0 x) stack]
        [(equal? command "yx") (vector-set! memory (modulo x 10) y) (nz-cdr stack)]
        [(equal? command "p") (cons (vector-ref memory 0) stack)]
        [(equal? command "px") (cons (vector-ref memory (modulo x 10)) (nz-cdr stack))]
        ;; }}}

        ;; change behavior {{{2
        [(equal? command "scale") (set! scale x) (nz-cdr stack)]
        [(equal? command "radix") (set! rpn-radix x) (nz-cdr stack)]
        [(equal? command "bin") (set! rpn-radix 2) stack]
        [(equal? command "hex") (set! rpn-radix 16) stack]
        [(equal? command "dec") (set! rpn-radix 10) stack]
        [(equal? command "rad") (set! drg 1) stack]
        [(equal? command "deg") (set! drg DEG2RAD) stack]
        [(equal? command "grd") (set! drg GRD2RAD) stack]
        [(equal? command "xl") (set! xl (not xl)) stack]
        ;; }}}

        ;; directly manipulate stack {{{2
        [(equal? command "inexact") (map exact->inexact stack)]
        [(equal? command "reverse") (reverse stack)]
        [(equal? command "r") (nz-cdr stack)] ;; roll or left shift the list
        [(equal? command "c") '(0)] ;; clear stack
        [(equal? command "car") (list (car stack))] ;; first 
        [(equal? command "cdr") (nz-cdr stack)] ;; rest
        ;; }}}

		;; Miax. 
        [(equal? command "help") (print-help-string) stack] ;; rest
		[else  ;; bad command
		  (if is-terminal?
			(begin 
               (clr-error) (printw "bad command") (refresh))
			(begin
			  (display "bad command") (newline)))
               stack]
    )))
		;
;;}}}

;;; ncurses: Position Routines {{{1
(define (clr-stack)
  (move 1 1)
  (clrtoeol))

(define (clr-entry)
  (move 2 2)
  (clrtoeol))

(define (clr-error)
  (move 3 2)
  (clrtoeol))

;;;}}}

;;; read-char-ncurses {{{1
;;; allow certain operations to avoid the need to hit [return]
(define (read-char-ncurses stack #!optional (str ""))
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
	  [(equal? #\- char) (cons "-" (if (equal? "" str) stack (dispatch str stack)))] 
	  [(equal? #\q char) (cons "q" stack)] 
	  [else
		(clr-error)
		(read-char-ncurses stack (string-append str (string char)))])))
;; }}}

;;; Dispatch {{{1
(define (dispatch line stack)
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
