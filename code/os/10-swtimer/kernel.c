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
	// welcome();
	os_main();
	// lock_main();
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

	page_init(); // 初始化页表

	trap_init(); // 中断向量表

	plic_init(); // 设置中断优先级

	timer_init(); // 时钟初始化

	sched_init();

	create_os_task(os_start); // 创建内核任务,借用内核线程进行调度

	while (1)
	{
		// 让任务先回到这里再调用
	}
}
