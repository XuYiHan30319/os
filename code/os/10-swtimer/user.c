#include "os.h"

#define DELAY 4000

struct userdata
{
	int counter;
	char *str;
};

/* Jack must be global全局变量,让他累加计数 */
struct userdata person = {0, "Jack"};

void timer_func(void *arg)
{
	if (NULL == arg)
		return;

	struct userdata *param = (struct userdata *)arg;

	param->counter++;
	printf("======> TIMEOUT: %s: %d\n", param->str, param->counter);
}

void user_task0(int a[2])
{
	// 分别在第3,5,7个时刻进行提示,软件定时器
	// struct timer *t1 = timer_create(timer_func, &person, 3);
	// if (NULL == t1)
	// {
	// 	printf("timer_create() failed!\n");
	// }
	// struct timer *t2 = timer_create(timer_func, &person, 5);
	// if (NULL == t2)
	// {
	// 	printf("timer_create() failed!\n");
	// }
	// struct timer *t3 = timer_create(timer_func, &person, 7);
	// if (NULL == t3)
	// {
	// 	printf("timer_create() failed!\n");
	// }
	int i = 0;
	char c[102];
	// scanf(&c, sizeof(c));
	// printf("接受到了c,%s", c);
	while (1)
	{
		i++;
		if (i == 10)
		{
			printf("正在杀死任务0\n");
			task_yield(0);
		}
		printf("Task %d: Running... \n", a[0]);
		task_delay(DELAY);
	}
}

void user_task1(void)
{
	char *k = (char *)malloc(sizeof(char) * 9000); // 这里我的malloc返回了个NULL
	k[0] = '1';
	k[1] = '2';
	k[2] = '3';
	k[3] = '4';
	printf("Task 1: Running...s=%s\n", k);
	free(k);

	int *s2 = (int *)malloc(sizeof(int));
	*s2 = 8;
	free(s2);

	(int *)malloc(sizeof(int));
	int i = 0;
	while (1)
	{
		i++;
		if (i == 4)
		{
			printf("正在杀死任务1\n");
			task_yield(0);
		}
		uart_puts("Task 1: Running...\n");
		task_delay(DELAY);
	}
}

void user_task2(uint32_t *parama)
{
	int i = 0;
	while (1)
	{
		i++;
		printf("Task 2: Running...parmas:%d %d\n", *parama, *(parama + 1));
		if (i == 4)
		{
			printf("正在杀死任务2\n");
			task_yield(0);
		}
		task_delay(DELAY);
	}
}

char user_input = 0;

void input_task(void)
{
	while (1)
	{
		user_input = xyh_uart_getc();
		if (user_input == 's' || user_input == 'w' || user_input == 'a' || user_input == 'd')
			;
		{
			// printf("接受到输入了");
			task_yield(1);
		}
	}
}

void game()
{
	int x = 0;
	int y = 0;
	while (1)
	{
		if (user_input == 's')
		{
			if (y < 4)
			{
				y++;
			}
			user_input = 0;
		}
		else if (user_input == 'w')
		{
			if (y > 0)
			{
				y--;
			}
			user_input = 0;
		}
		else if (user_input == 'd')
		{
			if (x < 22)
			{
				x++;
			}
			user_input = 0;
		}
		else if (user_input == 'a')
		{
			if (x > 0)
			{
				x--;
			}
			user_input = 0;
		}
		else
		{
			continue;
		}
		for (int i = 0; i < 30; i++)
		{
			printf("\n");
		}
		printf("------------------------\n");
		for (int i = 0; i < 5; i++)
		{
			if (i == y)
			{
				printf("|");
				for (int j = 0; j < x; j++)
				{
					printf(" ");
				}
				printf("x");
				for (int j = x; j < 21; j++)
				{
					printf(" ");
				}
				printf("|\n");
			}
			else
			{
				printf("|                      |\n");
			}
		}
		printf("------------------------\n");
		task_yield(1);
	}
}

uint32_t a[2] = {-1, 4};
// 这里用来创造初识线程
void os_main(void)
{
	task_create(user_task0, a, 1, 5);
	task_create(user_task1, NULL, 1, 1);
	task_create(user_task1, NULL, 1, 1);
	task_create(user_task1, NULL, 1, 1);
	task_create(user_task2, a, 2, 1);
	// 编译哈
	// task_create(run, NULL, 1, 1);
	// uart_putc('\n');
	// 游戏
	//  task_create(input_task, NULL, 1, 1);
	//  task_create(game, NULL, 1, 1);
}
