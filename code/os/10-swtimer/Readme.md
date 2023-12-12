
# 修改内容

## 内存分配
原来实现的内存管理方法为Bit-map,刚开始就计算好要用多少个比特位来操作所有的空间.但是对于malloc,我认为这样有点浪费(事实证明后续实现的link-list也好不到哪去)
于是我尝试在page的基础上实现了malloc和free函数
相较于os课程中的<内存管理>一节,我的malloc可以看做是更细粒度内存管理,采用的是first-fit algorithm,遇到第一个比他小的块就占用,然后将剩余的部分向后合并.当释放的时候同理,看看能不能向前/后合并,然后看看能不能释放这一块page.所用的结构体为双向链表,可以分配任意大小的块.

## 虚拟内存和MMU
这块太难了,不会

## 进程回收
实现task的回收机制,task可以调用task_yield(0)函数来中断自己

## 进程调度
FIFO调度: 最初版本就是FIFO
Priority调度:修改create_task函数如下:int  task_create(void (*task)(void* param), void *param, uint8_t priority, uint32_t timeslice);根据优先级进行运行,同级别的任务之间轮询.

## 锁机制
原来的实现的机制是直接关闭timer中断,因为实现原子指令需要调用汇编,太麻烦了
本人实现了信号量机制,详情见:lock.c,定义结构体semaphore,用来存放等待的任务队列以及资源的剩余数量,为了实现任务的阻塞效果,在task结构体中添加vaild来指示当前任务是否可以继续运行.然后修改schedule函数,使其忽略等待的任务.

## 外部中断


## timer



## 关机
没办法关机,只能退出qemu模拟器