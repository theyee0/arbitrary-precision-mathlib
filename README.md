# arbitrary-precision-mathlib
Math Library supporting arbitrary precision computation in any natural number base

## Usage
### Demo
A demo of the calculations is provided in `src/rpn.c`, which implements an rpn calculator that can add or subtract. It can be built with
```
gcc -g -Iinc src/rpn.c -o rpn
```
then, the resulting `rpn` executable can be run. On each input line you can enter a number, `+`, or `-`. To exit, enter `quit`.

### Testing
Testing is provided in `src/test.c`. It is the target for the makefile. To build the tests, run
```
make debug
```
and run the resulting `apml_test` executable.

### Library Usage
The library is implemented as a single-header C file. This means that there are no source files to add to your project if you want to use this.
```
#include "apml.h"
```
Then, you should be able to access all the definitions from the file.

#### Quickstart
This is not a full listing of all the functions implemented, only the basic ones you really need to get started.

`apml_t` represents a pointer to the number object. It's implemented as a typedef to a pointer to the struct that represents the number. As it's a pointer, you will need to allocate memory, potentially like `apml_t num = malloc(sizeof(*num))`.
`apml_init()` can be used to initialize an `apml_t` object. Provide the `apml_t` object and the base, and the function will initialize the `apml_t`
`apml_realloc()` can be used to allocate more memory for an `apml_t` object. The amount of memory is given as number of new entries, not the number of bytes.
`apml_add_im()` can be used to add two numbers. The result will be stored in the first number you pass to it.
`apml_sub_im()` is similar to `apml_add_im()`, except that it performs subtraction rather than addition.
