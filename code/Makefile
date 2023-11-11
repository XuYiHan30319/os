include common.mk

run:
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo "------------------------------------"
	${CC} ${CFLAGS} -o os.elf hello.o
	${QEMU} ${QFLAGS} -kernel os.elf

ubunturun:
	gcc -c hello.c -o hello.o


.PHONY : run