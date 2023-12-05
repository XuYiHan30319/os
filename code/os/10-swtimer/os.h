#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "riscv.h"
#include "platform.h"

#include <stddef.h>
#include <stdarg.h>

#define MAX_TASKS 10
#define STACK_SIZE 1024
/* uart */
extern int uart_putc(char ch);
extern void uart_puts(char *s);
extern int uart_getc(void);
extern uint8_t xyh_uart_getc(void);

/* printf */
extern int printf(const char *s, ...);
extern void scanf(char *, int);
extern void panic(char *s);

/* memory management */
extern void *page_alloc(int npages);
extern void page_free(void *p);

/* task management */
struct context
{
	/* ignore x0 */
	reg_t ra;
	reg_t sp;
	reg_t gp;
	reg_t tp;
	reg_t t0;
	reg_t t1;
	reg_t t2;
	reg_t s0;
	reg_t s1;
	reg_t a0;
	reg_t a1;
	reg_t a2;
	reg_t a3;
	reg_t a4;
	reg_t a5;
	reg_t a6;
	reg_t a7;
	reg_t s2;
	reg_t s3;
	reg_t s4;
	reg_t s5;
	reg_t s6;
	reg_t s7;
	reg_t s8;
	reg_t s9;
	reg_t s10;
	reg_t s11;
	reg_t t3;
	reg_t t4;
	reg_t t5;
	reg_t t6;
	// upon is trap frame
	// save the pc to run in next schedule cycle
	reg_t pc; // offset: 31 *4 = 124
	uint8_t priority;
	uint32_t timeslace;
	uint32_t UID;  // 身份标识
	uint8_t vaild; // 是否可以继续运行
};

typedef struct queue_node
{
	struct context *ctx;
	struct queue_node *next;
} queue_node;

typedef struct queue
{
	queue_node *head;
	queue_node *tail;

} queue;

typedef struct semaphore
{
	int count;
	queue *task_queue;
} semaphore;

extern void run();
extern queue *init_queue();
extern void push_queue(queue *, struct context *);
extern void pop_queue(queue *);
extern struct context *queue_head(queue *);

// extern int task_create(void (*task)(void));
extern int
task_create(void (*task)(void *param), void *param, uint8_t priority, uint32_t timeslice);
extern void task_delay(volatile int count);
extern void task_yield(int code);
extern void free(void *ptr);
extern void *malloc(size_t size);
/* plic */
extern int plic_claim(void);
extern void plic_complete(int irq);

/* lock */
extern int spin_lock(void);
extern int spin_unlock(void);

/* software timer */
struct timer
{
	void (*func)(void *arg);
	void *arg;			   // 给函数传参的参数
	uint32_t timeout_tick; // 超时计数器
};
extern void create_os_task(void (*task)(void *param));
extern void kill_schedule();
extern struct timer *timer_create(void (*handler)(void *arg), void *arg, uint32_t timeout); // 创建定时器
extern void timer_delete(struct timer *timer);												// 删除定时器

extern void semWait(semaphore *s);
extern void semSignal(semaphore *s);

#endif /* __OS_H__ */
