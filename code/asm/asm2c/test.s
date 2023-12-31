# ASM call C

	.text			# Define beginning of text section
	.global	_start		# Define entry _start
	.global	foo		# foo is a C function defined in test.c

_start:
	la sp, stack_end	# prepare stack for calling functions

	# RISC-V uses a0 ~ a7 to transfer parameters
	li a0, 1
	li a1, 2
	call foo
	# RISC-V uses a0 & a1 to transfer return value
	# check value of a0

stop:
	j stop			# Infinite loop to stop execution

	nop			# just for demo effect

stack_start: #这里就循环了12次开辟一段内存空间
	.rept 12
	.word 0
	.endr
stack_end:

	.end			# End of file
