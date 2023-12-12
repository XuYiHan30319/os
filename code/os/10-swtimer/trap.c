#include "os.h"

extern void trap_vector(void);
extern char uart_isr(void);
extern void timer_handler(void);
extern void schedule(void);
extern int software_type;
extern struct context *next_task;
char message = 0;

void trap_init()
{
	/*
	 * set the trap-vector base-address for machine-mode
	 * 打开中断并且把触发中断的位置设置为中断向量表
	 */
	w_mtvec((reg_t)trap_vector);
}

void external_interrupt_handler()
{
	int irq = plic_claim();

	if (irq == UART0_IRQ)
	{
		message = uart_isr();
	}
	else if (irq)
	{
		printf("unexpected interrupt irq = %d\n", irq);
	}

	if (irq)
	{
		plic_complete(irq);
	}
}

reg_t trap_handler(reg_t epc, reg_t cause)
{
	reg_t return_pc = epc;
	reg_t cause_code = cause & 0xfff;

	if (cause & 0x80000000)
	{
		/* Asynchronous trap - interrupt */
		switch (cause_code)
		{
		case 3:;
			int id = r_mhartid();
			*(uint32_t *)CLINT_MSIP(id) = 0;
			if (software_type == 0)
			{
				kill_schedule();
			}
			else
			{
				switch_to(next_task);
			}

			break;
		case 7:
			// 定时器中断部分
			// uart_puts("timer interruption!\n");
			timer_handler();
			break;
		case 11:
			// uart_puts("external interruption!\n");
			external_interrupt_handler();
			break;
		default:
			uart_puts("unknown async exception!\n");
			break;
		}
	}
	else
	{
		/* Synchronous trap - exception */
		printf("Sync exceptions!, code = %d\n", cause_code);
		panic("OOPS! What can I do!");
		// return_pc += 4;
	}

	return return_pc;
}

void trap_test()
{
	/*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
	*(int *)0x00000000 = 100; // 手动触发异常

	/*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	// int a = *(int *)0x00000000;

	uart_puts("Yeah! I'm return back from trap!\n");
}