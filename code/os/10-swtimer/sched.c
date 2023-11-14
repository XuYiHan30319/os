#include "os.h"

/* defined in entry.S */
#define min(a, b) (a <= b ? a : b);
#define max(a, b) (a >= b ? a : b);
/*
 * In the standard RISC-V calling convention, the stack pointer sp
 * is always 16-byte aligned.
 */
extern void switch_to(struct context *next);

uint8_t __attribute__((aligned(16))) task_stack[MAX_TASKS][STACK_SIZE];
uint8_t __attribute__((aligned(16))) os_stack[STACK_SIZE];
struct context ctx_tasks[MAX_TASKS]; // 可以用链表重写,但是我太懒了
struct context *next_task;
struct context ctx_os; // os线程位置,不需要特意设置,理论上在第一次跳转的时候自动绑定位置 ////但是我不会
extern uint32_t _tick;
uint32_t waiting_time = 0;
/*
 * _top is used to mark the max available position of ctx_tasks
 * _current is used to point to the context of current task
 */
static uint32_t UID = 0;
int _top = 0;
int _current = -1;
int software_type;

void sched_init()
{
	w_mscratch(0);

	/* enable machine-mode software interrupts. */
	w_mie(r_mie() | MIE_MSIE);
}

void schedule()
{

	switch_to(&ctx_os);
}

struct context *get_next_task()
{
	if (_top <= 0)
	{
		panic("Num of task should be greater than zero!");
		return;
	}
	// 在这里进行priority的操作,使用最基本的数据结构,在priority内实现轮询,priority低的先做
	// 并且我们保证任务都在前面
	int cur = -1, max_priority = 256;
	int flag = 1;
	for (int i = 0; i < _top; i++)
	{
		if (ctx_tasks[i].vaild)
		{
			max_priority = min(max_priority, ctx_tasks[i].priority);
		}
	}
	int a[MAX_TASKS]; // 找出所有优先级等于max_priority的
	int n = 0;
	for (int i = 0; i < _top; i++)
	{
		if (ctx_tasks[i].vaild && ctx_tasks[i].priority == max_priority)
		{
			a[n] = i;
			if (_current == i)
			{
				cur = n;
			}
			n++;
		}
	}
	if (cur == -1)
	{
		_current = a[0];
	}
	else
	{
		_current = a[(cur + 1) % n];
	}
	struct context *next = &(ctx_tasks[_current]);
	waiting_time = _tick + next->timeslace;
	return next;
}

void kill_schedule()
{
	int uid = ctx_tasks[_current].UID;
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
		ctx_tasks[i].pc = ctx_tasks[i + 1].pc;
		ctx_tasks[i].priority = ctx_tasks[i + 1].priority;
		ctx_tasks[i].timeslace = ctx_tasks[i + 1].timeslace;
		ctx_tasks[i].UID = ctx_tasks[i + 1].UID;
		ctx_tasks[i].vaild = ctx_tasks[i + 1].vaild;
	}
	printf("任务%d被干掉\n", uid);
	_current = -1;
	_top--;
	switch_to(&ctx_os);
}

void create_os_task(void (*task)(void *param))
{
	ctx_os.sp = (reg_t)&os_stack;
	ctx_os.pc = (reg_t)task;
	ctx_os.vaild = 1;
	printf("创建内核task\n");
	switch_to(&ctx_os);
}

int task_create(void (*task)(void *param), void *param, uint8_t priority, uint32_t timeslice)
{
	if (_top < MAX_TASKS)
	{
		ctx_tasks[_top].sp = (reg_t)&task_stack[_top][STACK_SIZE];
		ctx_tasks[_top].pc = (reg_t)task;
		ctx_tasks[_top].a0 = param;
		ctx_tasks[_top].priority = priority;
		ctx_tasks[_top].timeslace = timeslice;
		ctx_tasks[_top].UID = UID++;
		ctx_tasks[_top].vaild = 1;
		printf("创建任务,UID为%d\n", ctx_tasks[_top].UID);
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
void task_yield(int code)
{
	/* trigger a machine-level software interrupt */
	software_type = code;
	int id = r_mhartid();
	*(uint32_t *)CLINT_MSIP(id) = 1;
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
