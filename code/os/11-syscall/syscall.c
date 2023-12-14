#include "os.h"
#include "syscall.h"

int sys_gethid(unsigned int *ptr_hid)
{
	printf("--> sys_gethid, arg0 = 0x%x\n", ptr_hid);
	if (ptr_hid == NULL)
	{
		return -1;
	}
	else
	{
		*ptr_hid = r_mhartid();
		return 0;
	}
}

void do_syscall(struct context *cxt)
{
	uint32_t syscall_num = cxt->a7; // 使用a7进行传递系统调用号

	switch (syscall_num)
	{
	case SYS_gethid:
		cxt->a0 = sys_gethid((unsigned int *)(cxt->a0));
		break;
	case SYS_printf:; //?好逆天的bug,必须加一个;不然会编译错误
		char *s1 = (char *)cxt->a0;
		char *s2 = (char *)cxt->a1;
		printf(s1, s2);
		break;
	case SYS_printfWithoutcanshu:;
		char *s = (char *)cxt->a0;
		printf(s);
		break;
	default:
		printf("Unknown syscall no: %d\n", syscall_num);
		cxt->a0 = -1;
	}

	return;
}