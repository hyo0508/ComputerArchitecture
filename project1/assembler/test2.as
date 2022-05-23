      lw 0 1 one    $reg1 = 1
      lw 0 2 num1   $reg2 num1 = 5
      lw 0 3 num2   $reg3 num2 = 8
L1    beq 5 3 exit  escape loop when i == num2
      add 4 2 4     result += num1
      add 5 1 5     i++
      beq 0 0 L1    go back to the beginning of the loop
exit  halt          calculate 5 * 8 using add operation
one   .fill 1
num1  .fill 5
num2  .fill 8
