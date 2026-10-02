mov RAX, 1
mov RCX, 1
cmp RAX, RCX
je skip
mov RAX, 99
skip:
print RAX
halt
