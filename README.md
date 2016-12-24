# rpn
Command line rpn calc

Lightweight, command-line, rpn calculator.

## Usage

* Entering a number pushes it on the stack.
* Entering a command manipulates the stack.
* Entering a non-numeric non-command prints the first value of the stack and exits.
* If there are insufficient values on the stack for an operation, zero is assumed.

### Example
```
()
> 1
(1.00000)
> 2
(2.00000 1.00000)
> +
(3.00000)
>
```

