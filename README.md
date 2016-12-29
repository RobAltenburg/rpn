# rpn
A lightweight, command-line, rpn calculator.

## Usage

* Entering a number pushes it on the stack.
* Entering a command manipulates the stack.
* Entering "q" quits.
* If there are insufficient values on the stack for an operation, zero is assumed.

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


