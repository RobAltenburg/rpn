                      rpn
A character-based Reverse Polish notaion calculator.

'x' is the top of the stack, followed by y, z, and unnamed values after that.
The maximimum stack size is memory dependent.

Quick example:

  Keystrokes  Stack
  10 [Enter]  x: 10
  5  [Enter]  y: 10,  x: 5  
  *           x: 50
  2*          x: 100
  [Enter]     y: 100, x: 100
  ? [Enter]   Display Help (press any key to continue
  q [Enter]   Exit program

Immediate mode keys that don't require an [Enter]:
  +, -, *, /, ^, %

Less obvious keys:

~       Swaps the values of x and y
sto     Stores y in memory[x]
rcl     Pushes memory[x] to x
mc      Clears all memory locations
save    Store stack size in memory[0], Store stack in memory[1], memory[2], ...
restore Copies memory in the save format to the stack
.       Repeats the last command
hypot   Hypotenuse of a right triangle x: sqrt(x^2, y^2)
deg     Set degree mode [default] (also rad & grd)
hex     Temporarily display hexidecimal values of the stack
chs     Change sign of x
