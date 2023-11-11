# 原始内容


## 到这里的源代码实现是一个软件定时器


# 修改内容
## 内存分配
在page的基础上实现了malloc和free函数

## 进程回收
实现task的回收机制,task可以调用task_yield(0)函数来中断自己

## 进程调度
FIFO调度:最初版本就是FIFO
Priority调度:修改create_task函数如下:int  task_create(void (*task)(void* param), void *param, uint8_t priority, uint32_t timeslice);根据优先级进行运行,同级别的任务之间轮询

## 锁机制
原来的实现的机制是直接关闭timer中断,因为实现原子指令需要调用汇编,太麻烦了

本人实现了task的


## timer



## 关机..?
没办法关机,只能退出qemu模拟器