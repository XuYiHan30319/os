#include "os.h"

extern void trap_vector(void);
extern void uart_isr(void);

void trap_init()
{
	/*
	 * set the trap-vector base-address for machine-mode
	 */
	// 注册中断的入口地址,初始化地址
	w_mtvec((reg_t)trap_vector);
}

void external_interrupt_handler()
{
	int irq = plic_claim(); // 获得发生终端的设备号码

	if (irq == UART0_IRQ)
	{
		printf("这是一个UART0接收到的输入中断\n");
		uart_isr();
	}
	else if (irq)
	{
		printf("unexpected interrupt irq = %d\n", irq);
	}

	if (irq)
	{
		plic_complete(irq); // 指使外部中断处理完成
	}
}

reg_t trap_handler(reg_t epc, reg_t cause)
{
	reg_t return_pc = epc;
	reg_t cause_code = cause & 0xfff;

	if (cause & 0x80000000) // 看那一位是不是个1,是的话就是中断,否则就是异常
	{
		/* Asynchronous trap - interrupt */
		switch (cause_code)
		{
		case 3:
			uart_puts("software interruption!\n");
			break;
		case 7:
			uart_puts("timer interruption!\n");
			break;
		case 11:
			// 机器模式下的操作,接受UART的输入然后使用external
			uart_puts("external interruption检测到你的输入🌶\n");
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
	*(int *)0x00000000 = 100;

	/*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	// int a = *(int *)0x00000000;

	uart_puts("Yeah! I'm return back from trap!\n");
}
