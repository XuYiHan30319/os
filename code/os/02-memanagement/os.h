#ifndef __OS_H__
#define __OS_H__

#include "types.h"
#include "platform.h"

#include <stddef.h>
#include <stdarg.h>

/* uart */
extern int uart_putc(char ch);
extern void uart_puts(char *s);

/* printf */
extern int printf(const char *s, ...);
extern void panic(char *s);

/* memory management */
extern void *page_alloc(int npages); // f分配页
extern void page_free(void *p);      // 释放页

#endif /* __OS_H__ */
