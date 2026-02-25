# RPN Calculator

A reverse Polish notation (RPN) calculator with user-defined operators

## Usage

```bash
./rpn                  # Interactive mode
./rpn -e "2 3 +"       # Evaluate expression and exit
./rpn "2 3 +"          # Same as above (shorthand)
./rpn -h               # Show help
```

## Features

- **Arithmetic**: +, -, *, /, %, ^
- **Trigonometric**: sin, cos, tan, asin, acos, atan, atan2
- **Logarithmic**: ln, log, log2, logb (arbitrary base), exp
- **Rounding**: floor, ceil, round, trunc
- **Other Math**: sqrt, abs, neg, inv, gamma, !
- **Constants**: pi, e, phi (golden ratio)
- **Stack Commands**: p(rint), c(clear), d(uplicate), r/swap (reverse top 2), pop, sum, prod, copy
- **Memory**: x= (save top of stack to x), x (recall top of stack),  sto, rcl (deprecated)
- **User-defined Operators**: name{ } (saved), name[ ] (temporary), name (execute)
- **Angle Modes**: deg (degrees), rad (radians), grd (gradians)
- **Settings**: scale (set output precision), fmt (toggle localized number formats)
- **Help**: help or ? (list all operators)
- **Empty Stack Handling**: Operations on empty stack automatically use 0 for missing operands
- **Trailing Zeros Removal**: Zeros at the bottom of the stack are automatically removed
- **Memory Storage**: Unlimited numbered memory locations via `sto` and `rcl` commands
- **Configuration File**: `~/.rpn` can preset memory values, angle mode, and precision

## Memory Operations

```
25 i=   # Stores 25 in named variable j, 25 stays on stack
j       # Recalls value from named variable j
```

### Auto-binding x, y, z, t
When autobind is enabled (default), x/y/z/t are automatically bound to the top 4 stack positions:

```
square{ x x * }       # x is bound to top of stack
5 square              # Result: 25
```


## User-defined Operators

Define custom operators for reuse.
When autobind is enabled, the x,y,z,t, values are persistent during the operation

### Saved Operators
Use `{ }` to define operators that are saved to `~/.rpn`:

```
double{ 2 * }         # Define 'double' operator (saved)
5 double              # Execute: 5 * 2 = 10
```

### Temporary Operators
Use `[ ]` to define operators for the current session only:

```
triple[ 3 * ]         # Define 'triple' operator (temporary)
5 triple              # Execute: 5 * 3 = 15
```

## Configuration File

The calculator loads configuration from `.rpn` in the current directory, or `~/.rpn` if no local config exists.

```
# Comments start with #

# Set angle mode
deg           # or rad, grd

# Set precision (0-15)
scale 10

# User-defined operators (saved to config)
operator double Double value : 2 *
operator square Square value : d *

# Legacy: temporary operators using 'macro' keyword (deprecated)
macro temp_double 2 *

# Customize output format
# Use $op to show operation name, $value for the result
prefix "\t$op → "           # Shows: "    + → 8" for "5 3 +"
prefix "\t→ $op "           # Shows: "    → + 8" for "5 3 +"
prefix "\t→ $value $op"   # Shows: "    → 8 +" for "5 3 +"
prefix "\t→ "               # Default: shows just "    → 8"
```
