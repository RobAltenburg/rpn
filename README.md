# rpn
A lightweight, command-line, rpn calculator.

## General operation

This calculator maintains a stack of operands *(x, y, z, ...)*. Entering a number pushes it onto a stack, and enetering an 
operator may consume elements from the stack and, in most cases, will push its result back on the stack.

So, typing ```25 [return]``` pushes "25" on the stack, and then typing ```sqrt [return]``` pops "25" from the stack and pushes the result leaving "5" in position *x*.

*Shortcut:*  For single-character operators, you can type ```25+``` instead of ```25 [return] + [return]```.

### Constants
These push a value on the stack without consuming a stack element: *value -> x*

* **pi**
* **e**
* **pi2** (pi/2)
* **p** (memory[0], This is a user-defined constant.)

### Uniary Operators
These consume x from the stack and push the result: *f(x) -> x*

* **log**, **log10**, **log2**, **exp**
* **sin**, **cos**, **tan**, **asin**, **acos**, **atan**
* **sqrt**
* **gamma**, **!**
* **abs** (|x|)
* **chs** (-1 * x)
* **inv** (1/x)
* **px**  (memory[x], pulls from memory slot x)

##### Binary Operators
These consume x, y from the stack and push the result: *f(x, y) -> x*

* **+**, **-**, \*, **/**  
* **expt**, **pow** (y^x)
* **logx** (logx(y))
* **mod** (y % x)
* **eex** (y * 10^x)
* **atan2** (atan x/y)

### Trinary Operators
These consume x, y, z from the stack and push the result: *f(x, y, z) -> x*

* **dms2deg** ( x=deg, y=min, z=sec to degrees)

### Stack Operators
These consume the entire stack and push the result: *f(stack) -> x*

* **sum**, **product**, **mean**
* **reverse**  (reverses order of the stack)
* **r**, **cdr** (roll left, drops first element of the stack)
* **c** (clears the stack)
* **car** (first element of the stack)

### Non-Stack Operators
These neither consume nor return elements: *void -> void*

* **bin**, **oct**, **dec**, **hex**  (set the radix)
* **deg**, **rad**, **grd** (sets the input for trig functions)
* **xl** (return the entire stack on exit)
* **help** (display help)

### Other Operators
These have non-standard behavior

* **deg2dms** (x(deg) -> x (deg), y (min), z (sec))
* **y** (yanks x -> memory[0] leaving x on the stack)
* **yx** (yanks x -> memory[x] leaving x on the stack)
* **scale** (sets display scale to x decimal places)
* **radix** (sets the display radix to x)



