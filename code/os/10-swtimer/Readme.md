# 原始内容


## 到这里的源代码实现是一个软件定时器


# 修改内容
## 内存分配
当前已经完成了malloc和free函数

## 进程回收
这里需要实现进程的回收机制

## 进程调度
FIFO调度:最初版本就是FIFO
Priority调度:修改create_task函数如下:int  task_create(void (*task)(void* param), void *param, uint8_t priority, uint32_t timeslice); 

## timer


## 锁机制
当前实现的机制是直接关闭锁,因为很难实现原子指令

## 关机..?
怎么关机啊,草