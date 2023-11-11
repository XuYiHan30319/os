#include "os.h"

#define DELAY 1000

extern int _current;

void user_task0(void)
{
	int i = 0;
	while (1)
	{
		i++;
		uart_puts("Task 0: Running...\n");
		task_delay(DELAY);
		if (i == 5)
		{
			task_exit();
		}
		task_yield();
	}
}

void user_task1(void)
{
	int i = 0;
	while (1)
	{
		i++;
		uart_puts("Task 1: Running...\n");
		task_delay(DELAY);
		if (i == 4)
		{
			task_exit();
		}
		task_yield();
	}
}

void user_task2(void)
{
	int i = 0;
	while (1)
	{
		i += 1;
		uart_puts("Task 2: Running...\n");
		task_delay(DELAY);
		if (i == 7)
		{
			task_exit();
		}
		task_yield();
	}
}

/* NOTICE: DON'T LOOP INFINITELY IN main() */
void os_main(void)
{
	task_create(user_task0, NULL, 1);
	uart_puts("Task 0: Created!\n");

	task_create(user_task1, NULL, 1);
	uart_puts("Task 1: Created!\n");

	task_create(user_task2, NULL, 0);
	uart_puts("Task 2: Created!\n");
}
