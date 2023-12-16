#include "os.h"

extern void trap_vector(void);
extern void uart_isr(void);
extern void schedule(void);
extern void do_syscall(struct context *cxt);

void trap_init()
{
	/*
	 * set the trap-vector base-address for machine-mode
	 */
	w_mtvec((reg_t)trap_vector);
}


reg_t trap_handler(reg_t epc, reg_t cause, struct context *cxt)
{
	reg_t return_pc = epc;
	reg_t cause_code = cause & 0xfff;

	if (cause & 0x80000000)
	{
		/* Asynchronous trap - interrupt */
		switch (cause_code)
		{
		case 3:
			uart_puts("software interruption!\n");
			/*
			 * acknowledge the software interrupt by clearing
			 * the MSIP bit in mip.
			 */
			int id = r_mhartid();
			*(uint32_t *)CLINT_MSIP(id) = 0;

			schedule();

			break;
		case 7:
			uart_puts("timer interruption!\n");
			break;
		case 11:
			uart_puts("external interruption!\n");
			break;
		default:
			uart_puts("unknown async exception!\n");
			break;
		}
	}
	else
	{
		/* Synchronous trap - exception */
		// printf("Sync exceptions!, code = %d\n", cause_code);
		switch (cause_code)
		{
		case 8: // 8表示用户态触发的ecall
			// uart_puts("System call from U-mode!\n");
			do_syscall(cxt);
			return_pc += 4; // 注意在ecall触发的中断是不会自己+4的
			break;
		default:
			panic("OOPS! What can I do!");
			// return_pc += 4;
		}
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