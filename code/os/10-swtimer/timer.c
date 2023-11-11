#include "os.h"

extern void switch_to(struct context *next);
extern void schedule(void);
extern struct context ctx_os;
// 系统时钟是否开启
// #define TIME_CLOCK
/* interval ~= 1s */
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

uint32_t _tick = 0;
extern uint32_t waiting_time;

#define MAX_TIMER 10 // 设置最多10个定时器
static struct timer timer_list[MAX_TIMER];

/* load timer interval(in ticks) for next timer interrupt.*/
void timer_load(int interval)
{
	/* each CPU has a separate source of timer interrupts. */
	int id = r_mhartid();

	*(uint64_t *)CLINT_MTIMECMP(id) = *(uint64_t *)CLINT_MTIME + interval;
}

void timer_init()
{
	struct timer *t = &(timer_list[0]);
	// 初始化一下
	for (int i = 0; i < MAX_TIMER; i++)
	{
		t->func = NULL; /* use .func to flag if the item is used */
		t->arg = NULL;
		t++;
	}

	/*
	 * On reset, mtime is cleared to zero, but the mtimecmp registers
	 * are not reset. So we have to init the mtimecmp manually.
	 */
	timer_load(TIMER_INTERVAL);

	/* enable machine-mode timer interrupts. */
	w_mie(r_mie() | MIE_MTIE);
}

struct timer *timer_create(void (*handler)(void *arg), void *arg, uint32_t timeout)
{
	/* TBD: params should be checked more, but now we just simplify this */
	if (NULL == handler || 0 == timeout)
	{
		return NULL;
	}

	/* use lock to protect the shared timer_list between multiple tasks */
	// 防止定时器被打断
	spin_lock();

	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++)
	{
		if (NULL == t->func)
		{
			break;
		}
		t++;
	}
	// 用完了
	if (NULL != t->func)
	{
		spin_unlock();
		return NULL;
	}
	// 开始运行
	t->func = handler;
	t->arg = arg;
	// 维护系统的tick数目
	t->timeout_tick = _tick + timeout;

	spin_unlock();

	return t;
}

void timer_delete(struct timer *timer)
{
	spin_lock();

	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++)
	{
		if (t == timer)
		{
			t->func = NULL;
			t->arg = NULL;
			break;
		}
		t++;
	}

	spin_unlock();
}

/* this routine should be called in interrupt context (interrupt is disabled) */
// 定时器优化,可以按照超时时间排序,也可以用链表来操作,也可以使用跳表等算法
static inline void timer_check()
{
	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++)
	{
		// 检查这些定时器有没有达到时间的
		if (NULL != t->func)
		{
			if (_tick >= t->timeout_tick)
			{
				t->func(t->arg); // 这里执行任务,在中断上下文中执行

				/* once time, just delete it after timeout */
				t->func = NULL;
				t->arg = NULL;

				break;
			}
		}
		t++;
	}
}

void timer_handler()
{
	_tick++;
#ifdef TIME_CLOCK
	// 系统时钟
	printf("当前系统时间为:%d:%d:%d\n", _tick / 3600, (_tick % 3600) / 60, (_tick) % 60);
#endif
	// 检查软件定时器
	timer_check();
	// 再等待一秒
	timer_load(TIMER_INTERVAL);
	// 在这里检查时间
	if (waiting_time == _tick)
	{
		// schedule();
		switch_to(&ctx_os);
	}
}
