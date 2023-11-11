	.text
	.global _start
array:
	.word 'h'
	.word 'e'
	.word 'l'
	.word 'l'
	.word 'o'
	.word 'w'
	.word 'o'
	.word 'r'
	.word 'l'
	.word 'd'
	.word '!'
	.word '\0'

_start:
	li a0,0
	la a1,array
	li a3,'\0'
loop:
	addi a0,a0,1
	lw a2,a1
	addi a1,a1,4
	bne a2, a3, _end # if t0 != t1 then target
	

_end:
	j _end


	.end


# char array[] = {'h', 'e', 'l', 'l', 'o', ',', 'w', 'o', 'r', 'l', 'd', '!', '\0'}; 
# int len = 0; 
# while (array[len] != '\0') { 
#   len++; 
# }