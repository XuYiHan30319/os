#ifndef __USER_API_H__
#define __USER_API_H__

/* user mode syscall APIs */
extern int gethid(unsigned int *hid);
extern void myprintf(const char *s, const char);
extern void myprintfWithoutCanshu(const char *s);
#endif /* __USER_API_H__ */
