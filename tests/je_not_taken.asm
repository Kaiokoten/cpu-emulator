mov RAX, 1
mov RCX, 2
cmp RAX, RCX
je skip
mov RAX, 99
skip:
print RAX
halt
