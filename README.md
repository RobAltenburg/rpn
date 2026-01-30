# RPN Calculator

A reverse Polish notation (RPN) calculator with support for multiple angle modes and comprehensive error handling.

## Features

- **Operators**: +, -, *, /, %, ^, sqrt, sin, cos, tan, atan, atan2, ln, log, exp, abs, neg, inv, gamma
- **Stack Commands**: p(rint), c(clear), d(uplicate), r(everse top 2), pop, sto, rcl, Enter (duplicate top or push 0)
- **Angle Modes**: deg (degrees), rad (radians), grd (gradians)
- **Settings**: scale (set output precision)
- **Empty Stack Handling**: Operations on empty stack automatically use 0 for missing operands
- **Trailing Zeros Removal**: Zeros at the bottom of the stack are automatically removed (since popping from empty returns 0)
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
