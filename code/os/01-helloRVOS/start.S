#include "platform.h"
.data
    #变量声明
    array:
        .byte 'H', 'e', 'l', 'l', 'o', ',', 'R', 'V', 'O', 'S', '!', 0
        .space 12
    # size of each hart's stack is 1024 bytes
    .equ    STACK_SIZE, 1024
    .global _start

    .text

#宏定义
    .macro uart_read_reg reg1, reg2
        lb \reg1, 0(\reg2)
    .endm

    .macro uart_write_reg reg1, v
        sb \v, 0(\reg1)
    .endm

_start:
    csrr t0, mhartid    # 读Hart id
    mv tp, t0
    bnez   t0, park

    slli t0, t0, 10     #t0 向左移动10位, 1024
    la sp, stacks + STACK_SIZE

    add sp, sp, t0; #  把sp移动到Hart对应的栈的栈顶位置上

    li s0, 0x10000000

    j start_kernel
    

park:
    wfi
    j park

stacks:
    .skip STACK_SIZE * MAXNUM_CPU

start_kernel_assamble:
    # prologue
    addi sp, sp, -8; # sp = sp + -8
    sw ra, 0(sp) # 

    call uart_init

    la a0, array

    call uart_puts

uart_init:
    addi sp, sp, -20

    addi s1, s0, 0x01
    uart_write_reg s1, zero # uart_write_reg(IER, 0x00);
                            # sb \v, 0(\reg1)

    addi s1, s0, 0x03            # uint8_t lcr = uart_read_reg(LCR);
    uart_read_reg s2, s1    # lb \reg1, 0(\reg2)

    slli s2, s2, 7          # uart_write_reg(LCR, lcr | (1 << 7));
    uart_write_reg s1, s2   

    li s2, 0x03             # uart_write_reg(DLL, 0x03);
    uart_write_reg s0, s2 

    addi s1, s0, 0x01       # uart_write_reg(DLM, 0x00);
    li s2, 0x00
    uart_write_reg s1, s2
    
    addi s1, s0, 0x03       # uart_write_reg(LCR, lcr | (3 << 0));
    li s2, 0x03
    uart_write_reg s1, s2

    addi sp, sp, 20#退栈操作
    ret

uart_puts:
    addi sp, sp, -12; # sp = sp + -12
    sw s1, 0(sp)
    sw s2, 4(sp)
    sw ra, 8(sp)

    mv s1, a0
loop:
    lbu a0, 0(s1)
    mv s2, a0
    addi s1, s1, 1; # a0 = a0 + 1
    jal uart_putc  # jump to uart_puts and save position to ra
    bnez s2, loop

    lw s1, 0(sp) # 
    lw s2, 4(sp) # 
    lw ra, 8(sp) # 
    addi sp, sp, 12; # sp = sp + 12
    ret

uart_putc:
    addi sp, sp, -8; # sp = sp + -8
    sw s1, 0(sp)
    sw s2, 4(sp) # 
    
    addi s1, s0, 5; # s1 = s0 + 5
    uart_read_reg s2, s1
check:
    andi s2, s2, 32; # s2 = s2 & 32
    beqz s2, check

    uart_write_reg s0, a0 

    lw s1, 0(sp) # 
    lw s2, 4(sp) # 
    addi sp, sp, 8; # sp = sp + 8
    ret

    .end