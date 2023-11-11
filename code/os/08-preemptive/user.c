#include "os.h"

#define DELAY 1000

void user_task0(void)
{
	int i = 0;
	// uart_puts("Task 0: I'm back!\n");
	while (5)
	{
		i++;
		uart_puts("Task 0: Running...\n");
		if (i == 10)
		{
			task_exit();
		}
		task_delay(DELAY);
	}
	// task_exit();
}

void user_task1(void)
{
	int i = 0;
	while (5)
	{
		i++;
		task_delay(DELAY);
		if (i == 5)
		{
			task_exit();
		}
	}
}

void user_task2(void)
{
	int i = 0;
	while (5)
	{
		i++;
		uart_puts("Task 2: Running...\n");
		if (i == 9)
		{
			task_exit();
		}
		task_delay(DELAY);
	}
}

void user_task3(void)
{
	int i = 0;
	while (5)
	{
		i++;
		uart_puts("Task 3: Running...\n");
		if (i == 9)
		{
			task_exit();
		}
		task_delay(DELAY);
	}
}

/* NOTICE: DON'T LOOP INFINITELY IN main() */
void os_main(void)
{
	uart_puts("Task 0: Created!\n");
	task_create(user_task0, NULL, 0, 5);
	uart_puts("Task 1: Created!\n");
	task_create(user_task1, NULL, 10, 3);
	uart_puts("Task 2: Created!\n");
	task_create(user_task2, NULL, 1, 3);
	uart_puts("Task 3: Created!\n");
	task_create(user_task3, NULL, 1, 3);
}
