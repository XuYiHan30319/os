#include "os.h"

/* interval ~= 1s */
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

static uint32_t _tick = 0;

/* load timer interval(in ticks) for next timer interrupt.*/
void timer_load(int interval)
{
	/* each CPU has a separate source of timer interrupts. */
	int id = r_mhartid();
	// 设置下一次停止的值为现在开始到后续1s
	*(uint64_t *)CLINT_MTIMECMP(id) = *(uint64_t *)CLINT_MTIME + interval;
}

void timer_init()
{
	/*
	 * On reset, mtime is cleared to zero, but the mtimecmp registers
	 * are not reset. So we have to init the mtimecmp manually.
	 */
	// 初始化mtimecmp的值为0+1s
	timer_load(TIMER_INTERVAL);

	/* enable machine-mode timer interrupts. */
	// 打开时间位的暂停
	w_mie(r_mie() | MIE_MTIE);

	/* enable machine-mode global interrupts. */
	w_mstatus(r_mstatus() | MSTATUS_MIE);
}

void timer_handler()
{
	// 停止了多少次了
	_tick++;
	printf("tick: %d\n", _tick);

	timer_load(TIMER_INTERVAL);
}
