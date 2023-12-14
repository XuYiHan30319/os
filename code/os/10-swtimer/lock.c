#include "os.h"

extern struct context ctx_tasks[MAX_TASKS];
extern uint32_t _tick;
extern int _top;
extern int _current;

// 关闭定时器中断
int spin_lock()
{
	w_mstatus(r_mstatus() & ~MSTATUS_MIE);
	return 0;
}

// 打开定时器中断
int spin_unlock()
{
	w_mstatus(r_mstatus() | MSTATUS_MIE);
	return 0;
}

semaphore s;

void semWait(semaphore *s)
{
	spin_lock(); // 关闭中断
	s->count--;
	if (s->count < 0)
	{
		push_queue(s->task_queue, &ctx_tasks[_current]); // 等待队列中添加当前任务
		printf("队列成功添加任务\n");
		ctx_tasks[_current].vaild = 0; // 阻塞
		task_yield(1);				   // 放弃当前任务的继续执行
	}
	spin_unlock(); // 打开中断
}

void semSignal(semaphore *s)
{
	spin_lock();
	if (s->count < 0)
	{
		struct context *next_vaild = queue_head(s->task_queue); // 从队列中取出一个可以继续执行的任务
		if (next_vaild != NULL)
		{
			printf("成功移出任务\n");
			pop_queue(s->task_queue);
			next_vaild->vaild = 1;
		}
	}
	s->count++;
	spin_unlock();
}

void init_semaphore()
{
	// 最多2个资源
	s.count = 2;
	s.task_queue = init_queue();
}

void lock_task1()
{
	while (1)
	{
		semWait(&s);
		task_delay(10000);
		printf("我是任务:%d,我拿到了task!\n", _current);
		semSignal(&s);
		printf("我是任务:%d,我释放了task!\n", _current);
	}
}
void lock_main()
{
	init_semaphore();
	task_create(lock_task1, NULL, 1, 1);
	task_create(lock_task1, NULL, 1, 1);
	task_create(lock_task1, NULL, 1, 1);
	task_create(lock_task1, NULL, 1, 1);
	task_create(lock_task1, NULL, 1, 1);
	uart_putc('\n');
}