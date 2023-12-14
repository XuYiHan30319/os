#include "syscall.h"

.global gethid
gethid:
	li a7, SYS_gethid
	ecall
	ret

# 来一个printf("%d",a)
.global myprintf
myprintf:
	li a7,SYS_printf
	ecall
	ret

.global myprintfWithoutCanshu
myprintfWithoutCanshu:
	li a7,SYS_printf
	ecall
	ret