mov RAX, 1
jmp skip
mov RAX, 99
skip:
mov RBX, 42
print RAX
print RBX
halt
