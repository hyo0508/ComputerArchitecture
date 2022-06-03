      lw  0 1 one   $reg1 = 1
      add 0 0 2     $reg2 cubic = 0
      lw  0 3 max   $reg3 num = 31
      add 0 0 4     $reg4 i = 0
L1    beq 4 3 exit1 escape loop1 when i == 31
      add 0 0 5     $reg5 j = 0
L2    beq 5 3 exit2 escape loop2 when j == 31
      add 2 3 2     cubic += number
      add 5 1 5     j++
      beq 0 0 L2    go back to the beginning of the loop2
exit2 add 4 1 4     i++
      beq 0 0 L1    go back to the beginning of the loop1
exit1 halt          calculate cubic of 6
one .fill 1
max .fill 6
