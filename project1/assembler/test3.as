      nor 0 0 2   neg1 = ~(0 | 0)     // 0xFFFFFFFF
      add 2 2 1   neg = neg1 + neg1   // 0xFFFFFFFE
      nor 0 1 3   $reg3 = ~(0 | neg)  // 1
      sw  3 3 arr arr[1] = 1
      add 1 2 1   neg = neg + neg2    // 0xFFFFFFFD
      nor 0 1 3   $reg3 = ~(0 | neg)  // 2
      sw  3 3 arr arr[2] = 2
      add 1 2 1   neg = neg + neg2    // 0xFFFFFFFC
      nor 0 1 3   $reg3 = ~(0 | neg)  // 3
      sw  3 3 arr arr[3] = 3
done  halt        make 1, 2, 3 using nor operation without using memory
arr  .fill 0
arr1 .fill 0
arr2 .fill 0
arr3 .fill 0
