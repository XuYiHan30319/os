#include "os.h"

// 软件定时器
extern void uart_init(void);
extern void page_init(void);
extern void sched_init(void);
extern void schedule(void);
extern void os_main(void);
extern void trap_init(void);
extern void plic_init(void);
extern void timer_init(void);
extern struct context *next_task;
extern struct context *get_next_task();
extern void switch_to(struct context *next);
extern void lock_main();

void welcome()
{
	printf("██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗    ████████╗ ██████╗     ██╗  ██╗ ██████╗ ███████╗\n");
	printf("██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝    ╚══██╔══╝██╔═══██╗    ╚██╗██╔╝██╔═══██╗██╔════╝\n");
	printf("██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗         ██║   ██║   ██║     ╚███╔╝ ██║   ██║███████╗\n");
	printf("██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝         ██║   ██║   ██║     ██╔██╗ ██║   ██║╚════██║\n");
	printf("╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗       ██║   ╚██████╔╝    ██╔╝ ██╗╚██████╔╝███████║\n");
	printf(" ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝       ╚═╝    ╚═════╝     ╚═╝  ╚═╝╚═════╝ ╚══════╝\n ");
}

void os_start()
{
	os_main();
	// lock_main();
	welcome();
	while (1)
	{
		next_task = get_next_task();
		// printf("切换到任务%d\n", next_task->UID);
		task_yield(1);
	}
}

void start_kernel(void)
{
	uart_init(); // 打开一些寄存器,让他可以出
	uart_puts("Hello, RVOS!\n");

	page_init(); // 初始化页表

	trap_init(); // 中断向量表

	plic_init();

	timer_init();

	sched_init();

	create_os_task(os_start);

	while (1)
	{
		// 让任务先回到这里再调用
	}
}
