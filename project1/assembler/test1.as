      lw  0 1 one   $reg1 = 1
      add 0 0 2     $reg2 sum = 0
      add 0 0 3     $reg3 i = 0
      lw  0 4 ten   $reg4 = 10
L1    beq 3 4 exit  escape loop when i == 10
      add 2 3 2     sum += i
      add 3 1 3     i++
      beq 0 0 L1    go back to the beginning of the loop
exit  halt          calculate the sum of 1 to 10
one  .fill 1
ten  .fill 10
