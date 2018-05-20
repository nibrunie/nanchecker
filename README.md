# nanchecker
dynamic NaN behavior checker

nanchecker is a small utlity which checks the CPU architecture behavior when
it encouters floating-point NaN values (Not a Number). It tries to determine
if the CPU returns one of the input NaN, a canonic NaN (TBD) or a NaN based
on an other predicate (TBD) e.g. max payload.


# Build and run #

    make test
