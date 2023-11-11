#include "os.h"

extern void schedule(void);
/* interval ~= 1s */
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

static uint32_t _tick = 0;

/* load timer interval(in ticks) for next timer interrupt.*/
void timer_load(int interval)
{
	/* each CPU has a separate source of timer interrupts. */
	int id = r_mhartid();
	// 到这个时候就会启动,添加一个1s
	*(uint64_t *)CLINT_MTIMECMP(id) = *(uint64_t *)CLINT_MTIME + interval;
}

void timer_init()
{
	/*
	 * On reset, mtime is cleared to zero, but the mtimecmp registers
	 * are not reset. So we have to init the mtimecmp manually.
	 */
	// timer_load(TIMER_INTERVAL ce);
	// 这里面设置了第一次停止的时间,所以在第一次运行的时候会有问题,可以把他设置到其他地方

	/* enable machine-mode timer interrupts. */
	w_mie(r_mie() | MIE_MTIE);
}

void timer_handler()
{
	_tick++;
	uint32_t tick = _tick;
	printf("tick: %d\n", _tick);
	int minute, hour, second;
	hour = tick / 3600;
	tick -= hour * 3600;
	minute = tick / 60;
	tick -= minute * 60;
	char time[20];
	time[0] = '0' + hour / 10;
	time[1] = '0' + hour % 10;
	time[2] = ':';
	time[3] = '0' + minute / 10;
	time[4] = '0' + minute % 10;
	time[5] = ':';
	time[6] = '0' + tick / 10;
	time[7] = '0' + tick % 10;
	time[8] = '\n';

	printf("%s", time);

	schedule();
}
