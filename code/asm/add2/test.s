# Add
# Format:
#	ADD RD, RS1, RS2
# Description:
#	The contents of RS1 is added to the contents of RS2 and the result is 
#	placed in RD.
	.text
	.global _start

_start:
	li x6, 1
	li x7, -2
	li x9,3
	add x5,x6,x7
	sub x8,x5,x9

stop:
	j stop
	.end

# register int a, b, c, d, e; 
# b = 1; 
# c = 2; 
# e = 3; 
# a = b + c; 
# d = a - e;