        mov RCX, 3
loop:   print RCX
        dec RCX
        cmp RCX, 0
        je done
        jmp loop
done:   halt
