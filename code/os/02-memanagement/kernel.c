#include "os.h"

/*
 * Following functions SHOULD be called ONLY ONE time here,
 * so just declared here ONCE and NOT included in file os.h.
 */
extern void uart_init(void);
extern void page_init(void);
extern uint8_t uart_getc();
extern int uart_putc(char);
extern void uart_puts(char *);
extern void page_test();

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, RVOS!\n");

	page_init();
	page_test();
	char s;
	while (1)
	{
		s = uart_getc();
		if (s == '\r')
		{
			uart_putc('\n');
		}
		uart_putc(s);
	}; // stop here!
}
