#include "platform.h"

	# size of each hart's stack is 1024 bytes
	.equ	STACK_SIZE, 1024

	.global	_start

	.text
_start:
	# park harts with id != 0
	csrr	t0, mhartid		# read current hart id
	mv	tp, t0			# keep CPU's hartid in its tp for later usage.
	bnez	t0, park		# if we're not on the hart 0
					# we park the hart

	# Set all bytes in the BSS section to zero.
	la	a0, _bss_start
	la	a1, _bss_end
	bgeu	a0, a1, 2f
1:
	sw	zero, (a0)
	addi	a0, a0, 4
	bltu	a0, a1, 1b
2:
	# Setup stacks, the stack grows from bottom to top, so we put the
	# stack pointer to the very end of the stack range.
	slli	t0, t0, 10		# shift left the hart id by 1024
	la	sp, stacks + STACK_SIZE	# set the initial stack pointer
					# to the end of the first stack space
	add	sp, sp, t0		# move the current hart stack pointer
					# to its place in the stack space

	# At the end of start_kernel, schedule() will call MRET to switch
	# to the first task, so we parepare the mstatus here.
	# Notice: default mstatus is 0
	# Set mstatus.MPP to 3, so we still run in Machine mode after MRET.表示mret后machine模式
	# Set mstatus.MPIE to 1, so MRET will enable the interrupt. 表示中断被打开

	# 打开寄存器中的某些位置让他可以进去machine模式,并且打开了中断,保证进入的是machine级别
	li	t0, 3 << 11 | 1 << 7
	csrr	a1, mstatus
	or	t0, t0, a1
	csrw	mstatus, t0
	# 跳转到start_kernel函数开始运行内核
	j	start_kernel		# hart 0 jump to c

park:# 停止该线程上内核的运行
	wfi
	j	park

	# In the standard RISC-V calling convention, the stack pointer sp
	# is always 16-byte aligned.
.align 16
stacks:
	.skip	STACK_SIZE * MAXNUM_CPU # allocate space for all the harts stacks

	.end				# End of file
