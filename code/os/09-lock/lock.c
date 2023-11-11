#include "os.h"

int spin_lock()
{
	w_mstatus(r_mstatus() & ~MSTATUS_MIE); // 蚌,直接关掉中断
	return 0;
}

int spin_unlock()
{
	w_mstatus(r_mstatus() | MSTATUS_MIE); // 蚌,直接开启中断
	return 0;
}
