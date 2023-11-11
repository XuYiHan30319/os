#include "os.h"

/* defined in entry.S */
extern void switch_to(struct context *next);
extern void exit_task_swith_to(struct context *next);
#define MAX_TASKS 10
#define STACK_SIZE 1024
#define PRIORITY_NUM 256
/*
 * In the standard RISC-V calling convention, the stack pointer sp
 * is always 16-byte aligned.
 */
uint8_t __attribute__((aligned(16))) task_stack[MAX_TASKS][STACK_SIZE]; // 开辟一块空间充当栈
struct context ctx_tasks[MAX_TASKS];									// 任务列表
/*
 * _top is used to mark the max available position of ctx_tasks
 * _current is used to point to the context of current task
 */
static int _top = 0;
static int _current = -1;

static void w_mscratch(reg_t x)
{
	asm volatile("csrw mscratch, %0" : : "r"(x));
}

void sched_init()
{
	w_mscratch(0);
}

void task_exit(void)
{
	// 首先根据current杀掉自己,就是把后面的东西全部往前面移动一个位置,覆盖掉自己
	for (int i = _current; i < _top; i++)
	{
		ctx_tasks[i].ra = ctx_tasks[i + 1].ra;
		ctx_tasks[i].sp = ctx_tasks[i + 1].sp;
		ctx_tasks[i].gp = ctx_tasks[i + 1].gp;
		ctx_tasks[i].tp = ctx_tasks[i + 1].tp;
		ctx_tasks[i].t0 = ctx_tasks[i + 1].t0;
		ctx_tasks[i].t1 = ctx_tasks[i + 1].t1;
		ctx_tasks[i].t2 = ctx_tasks[i + 1].t2;
		ctx_tasks[i].s0 = ctx_tasks[i + 1].s0;
		ctx_tasks[i].s1 = ctx_tasks[i + 1].s1;
		ctx_tasks[i].a0 = ctx_tasks[i + 1].a0;
		ctx_tasks[i].a1 = ctx_tasks[i + 1].a1;
		ctx_tasks[i].a2 = ctx_tasks[i + 1].a2;
		ctx_tasks[i].a3 = ctx_tasks[i + 1].a3;
		ctx_tasks[i].a4 = ctx_tasks[i + 1].a4;
		ctx_tasks[i].a5 = ctx_tasks[i + 1].a5;
		ctx_tasks[i].a6 = ctx_tasks[i + 1].a6;
		ctx_tasks[i].a7 = ctx_tasks[i + 1].a7;
		ctx_tasks[i].s2 = ctx_tasks[i + 1].s2;
		ctx_tasks[i].s3 = ctx_tasks[i + 1].s3;
		ctx_tasks[i].s4 = ctx_tasks[i + 1].s4;
		ctx_tasks[i].s5 = ctx_tasks[i + 1].s5;
		ctx_tasks[i].s6 = ctx_tasks[i + 1].s6;
		ctx_tasks[i].s7 = ctx_tasks[i + 1].s7;
		ctx_tasks[i].s8 = ctx_tasks[i + 1].s8;
		ctx_tasks[i].s9 = ctx_tasks[i + 1].s9;
		ctx_tasks[i].s10 = ctx_tasks[i + 1].s10;
		ctx_tasks[i].s11 = ctx_tasks[i + 1].s11;
		ctx_tasks[i].t3 = ctx_tasks[i + 1].t3;
		ctx_tasks[i].t4 = ctx_tasks[i + 1].t4;
		ctx_tasks[i].t5 = ctx_tasks[i + 1].t5;
		ctx_tasks[i].t6 = ctx_tasks[i + 1].t6;
		ctx_tasks[i].priority = ctx_tasks[i + 1].priority;
	}
	printf("任务%d被干掉\n", _current);
	_top--;
	_current--;
	_current = (_current + 1) % _top; // 变成下一个任务
	if (_top == 0)
	{
		return;
	}
	else
	{
		if (_top <= 0)
		{
			panic("Num of task should be greater than zero!");
			return;
		}
		for (int i = 0; i < _top; i++)
		{
			if (ctx_tasks[i].priority < _current)
			{
				_current = i;
			}
		}
		printf("正在切换到任务%d", _current);
		struct context *next = &(ctx_tasks[_current]);
		exit_task_swith_to(next);
	}
}

/*
 * implment a simple cycle FIFO schedular
 */
void schedule()
{
	if (_top <= 0)
	{
		panic("Num of task should be greater than zero!");
		return;
	}
	_current = (_current + 1) % _top; // 变成下一个任务
	printf("正在切换到任务%d", _current);
	struct context *next = &(ctx_tasks[_current]);
	switch_to(next);
}

struct context *schedule_priority() // 修改之后的优先级调度算法
{
	if (_top <= 0)
	{
		panic("Num of task should be greater than zero!");
		return;
	}
	int priority = _current == -1 ? 257 : ctx_tasks[_current].priority;
	for (int i = 0; i < _top; i++)
	{
		if (ctx_tasks[i].priority < priority)
		{
			_current = i;
			priority = ctx_tasks[i].priority;
		}
	}
	printf("正在切换到任务%d", _current);
	return &(ctx_tasks[_current]);
}

/*
 * DESCRIPTION
 * 	Create a task.
 * 	- start_routin: task routine entry
 * RETURN VALUE
 * 	0: success
 * 	-1: if error occured
 */
int task_create(void (*start_routin)(void), void *param, int priority) // 传入一个函数指针
{
	if (_top < MAX_TASKS)
	{
		ctx_tasks[_top].sp = (reg_t)&task_stack[_top][STACK_SIZE];
		ctx_tasks[_top].ra = (reg_t)start_routin;
		ctx_tasks[_top].priority = priority;
		_top++;
		return 0;
	}
	else
	{
		return -1;
	}
}

/*
 * DESCRIPTION
 * 	task_yield()  causes the calling task to relinquish the CPU and a new
 * 	task gets to run.
 */
void task_yield()
{
	// schedule();
	schedule_priority();
}

/*
 * a very rough implementaion, just to consume the cpu
 */
void task_delay(volatile int count)
{
	count *= 50000;
	while (count--)
		;
}
