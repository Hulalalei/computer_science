################################### 全局变量 ###################################
.section .data
    .equ len, 120
    .equ wid, 30
################################### 地图 ###################################
    map:
        .string "*********************************************************************************************\n" # 94bytes
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*                                                                                           *\n"
        .string "*********************************************************************************************\n"
    map_d:
################################### 睡眠 ###################################
    sleep_time:
        .long 10000
        .long 0
.section .bss
    res_ptr:
        .skip 8
################################### 代码区 ###################################
.section .text
    .global main

main:
    # call init
    call sleep


    # 执行_exit(0)后，后面的函数可以避免二次执行
    movq $60, %rax
    xorq %rbx, %rbx
    syscall


################################### 函数 ###################################
################################### init ###################################
init:
# + 95 || -95实现上下移动
# i = len / 94;  j = len % 94;
# len = i * 94 + j;
# i & j
    movq $1, %r8
    movq $1, %r9
    movq $3, %r10
    movq $len, %r11
# 基址 + 变址 索引数组
    movq $map, %rbx
    movq $0, %rax
    movb $0x40, %cl # @
    movb $0x20, %ch # ' '
    movq $map, %rsi
    movq $(map_d - map), %rdx
i_bg:
    movb %cl, (%rbx, %rax, 1)
# print的系统寄存器，同时保存调用者保存寄存器
    pushq %rcx
    pushq %rax
    call print
    popq %rax
    popq %rcx
# 向右移动
    movb %ch, (%rbx, %rax, 1)
    incq %rax
# i ++
    addq $1, %r8
    cmpq %r8, %r10
    jae i_bg
    ret

################################### print ###################################
print:
    movq $1, %rax
    movq $1, %rdi
    syscall
    ret

################################### sleep ###################################
sleep:
    movl $162, %eax
    movl $sleep_time, %ebx
    movl $res_ptr, %ecx
    syscall
    ret
