# rpn
A lightweight, command-line, rpn calculator.

## Usage

* Entering a number pushes it on the stack.
* Entering a command manipulates the stack.
* Entering "q" quits.
* If there are insufficient values on the stack for an operation, zero is assumed.

## Supported Commands
  
  *Operators
		+ - * / inv sqrt pow expt abs log log10 logx exp
		sin cos tan asin acos atan atan2 mod hms2hr hr2hms
   *Stack Operators: sum product mean reverse
   *Constants: pi e
   *Memory: y yx p px
   *Display Behavior: scale radix bin hex dec
   *Conversions: rad deg grd
   *Manipulate Stack: inexact reverse r c car cdr


### Example
```
rpn
()
> 1
(1.00000)
> 2
(2.00000 1.00000)
> +
(3.00000)
>
```

Note that "internal" commands use the stack and follow the same rules:
```
rpn
()
> 24
(24.00000)
> 2
(2.00000 24.00000)
> radix
(#b11000)
>
```


