      nor 0 0 2       $reg2 = -1
      add 2 2 7       
      nor 0 7 1       $reg1 = 1
      nor 0 0 3       $reg3 ra = 0xFFFFFFFF (fake)
      add 1 1 4
      add 4 4 4       $reg4 a0 = 4, $reg5 sp = 0, $reg6 v0 = 0
fact  sw  5 3 st      save ra
      add 5 1 5     
      sw  5 4 st      save a0
      add 5 1 5     
      beq 4 0 L1      test for n == 0
      add 4 2 4       if not, decrement n
      lw  0 7 facAd    |
      jalr  7 3         | recursive call
ret   add 5 2 5         |
      lw  5 4 st        | restore a0
      add 5 2 5         |
      lw  5 3 st        | restore ra
      add 0 0 2         .
      add 0 0 7         .
for   beq 2 4 exit      .
      add 7 6 7         .
      add 2 1 2         . multiply to get result
      beq 0 0 for       .
exit  add 7 0 6         .
      nor 0 0 2         .
      lw  0 7 retAd     |
      beq 3 7 ret       | jr $ra
      beq 0 0 done      |
L1    add 0 1 6       else, result is 1
      add 5 2 5         |
      add 5 2 5         | pop 2 items
      lw  0 7 retAd     | 
      beq 3 7 ret       | jr $ra
      beq 0 0 done      |
done  halt
facAd .fill fact
retAd .fill ret
st    .fill 0
st1   .fill 0
st2   .fill 0
st3   .fill 0
st4   .fill 0
st5   .fill 0
st6   .fill 0
st7   .fill 0
st8   .fill 0
st9   .fill 0
