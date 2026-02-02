# RPN Calculator

A reverse Polish notation (RPN) calculator with support for multiple angle modes and comprehensive error handling.

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
- **Memory**: sto, rcl
- **Angle Modes**: deg (degrees), rad (radians), grd (gradians)
- **Settings**: scale (set output precision), fmt (toggle localized number formats)
- **Help**: help or ? (list all operators)
- **Empty Stack Handling**: Operations on empty stack automatically use 0 for missing operands
- **Trailing Zeros Removal**: Zeros at the bottom of the stack are automatically removed
- **Memory Storage**: Unlimited numbered memory locations via `sto` and `rcl` commands
- **Configuration File**: `~/.rpn` can preset memory values, angle mode, and precision

## Memory Operations

### Store (sto)
Stores the top value in a numbered memory location. The value remains on the stack.

```
25 1 sto    # Stores 25 in memory location 1, 25 stays on stack
```

Memory locations must be integers. Non-integer locations produce an error without altering the stack.

### Recall (rcl)
Retrieves a value from memory and pushes it onto the stack.

```
1 rcl       # Recalls value from memory location 1
```

Recalling from an uninitialized location returns 0.

## Configuration File

Create a `~/.rpn` file to set defaults. Format:

```
# Comments start with #

# Set angle mode
deg           # or rad, grd

# Set precision (0-15)
scale 10

# Preset memory locations
mem 0 3.14159265359   # π
mem 1 2.71828182846   # e
mem 10 42             # Answer to everything
```
