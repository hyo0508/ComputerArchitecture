        lw  0 1 five
        lw  1 2 32768   error: offsetFields that don't fit in 16 bits (32768)
start   add 1 2 1
        beq 0 1 2
        beq 0 0 start
        noop
done    halt
five    .fill 5
neg1    .fill -1
stAddr  .fill start
