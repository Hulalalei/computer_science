.section .data
.section .text
    .global main

main:
    movq $0, %rax

    # 执行_exit(0)后，后面的函数可以避免二次执行
    movq $60, %rax
    xorq %rbx, %rbx
    syscall

print:
    movq $1, %rax
    movq $1, %rdi
    syscall
    ret
