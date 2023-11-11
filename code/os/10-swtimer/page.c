#include "os.h"

#define MALLOC_DEBUG
/*
 * Following global vars are defined in mem.S
 */
// 由ld文件计算出来的各个位置的开始值
extern uint32_t TEXT_START;
extern uint32_t TEXT_END;
extern uint32_t DATA_START;
extern uint32_t DATA_END;
extern uint32_t RODATA_START;
extern uint32_t RODATA_END;
extern uint32_t BSS_START;
extern uint32_t BSS_END;
extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;
#define false 0
#define true 1
#define bool char
/*
 * _alloc_start points to the actual start address of heap pool
 * _alloc_end points to the actual end address of heap pool
 * _num_pages holds the actual max number of pages we can allocate.
 */
static uint32_t _alloc_start = 0;
static uint32_t _alloc_end = 0;
static uint32_t _num_pages = 0;

#define PAGE_SIZE 4096
#define PAGE_ORDER 12

#define PAGE_TAKEN (uint8_t)(1 << 0)
#define PAGE_LAST (uint8_t)(1 << 1)

/*
 * Page Descriptor
 * flags:
 * - bit 0: flag if this page is taken(allocated)
 * - bit 1: flag if this page is the last page of the memory block allocated
 */
struct Page
{
	uint8_t flags;
};

static inline void _clear(struct Page *page)
{
	page->flags = 0;
}

static inline int _is_free(struct Page *page)
{
	if (page->flags & PAGE_TAKEN)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static inline void _set_flag(struct Page *page, uint8_t flags)
{
	page->flags |= flags;
}

static inline int _is_last(struct Page *page)
{
	if (page->flags & PAGE_LAST)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * align the address to the border of page(4K)
 */
static inline uint32_t _align_page(uint32_t address)
{
	uint32_t order = (1 << PAGE_ORDER) - 1;
	return (address + order) & (~order);
}

void page_init()
{
	/*
	 * We reserved 8 Page (8 x 4096) to hold the Page structures.
	 * It should be enough to manage at most 128 MB (8 x 4096 x 4096)
	 */
	_num_pages = (HEAP_SIZE / PAGE_SIZE) - 8;
	printf("HEAP_START = %x, HEAP_SIZE = %x, num of pages = %d\n", HEAP_START, HEAP_SIZE, _num_pages);

	struct Page *page = (struct Page *)HEAP_START;
	for (int i = 0; i < _num_pages; i++)
	{
		_clear(page);
		page++;
	}

	_alloc_start = _align_page(HEAP_START + 8 * PAGE_SIZE);
	_alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);

	printf("TEXT:   0x%x -> 0x%x\n", TEXT_START, TEXT_END);
	printf("RODATA: 0x%x -> 0x%x\n", RODATA_START, RODATA_END);
	printf("DATA:   0x%x -> 0x%x\n", DATA_START, DATA_END);
	printf("BSS:    0x%x -> 0x%x\n", BSS_START, BSS_END);
	printf("HEAP:   0x%x -> 0x%x\n", _alloc_start, _alloc_end);
}

/// @brief 申请页空间,他的页空间的方式是匹配第一个遇到的
/// @param npages
/// @return
void *page_alloc(int npages)
{
	int found = 0;
	struct Page *page_i = (struct Page *)HEAP_START;
	printf("申请了%d页\n", npages);
	for (int i = 0; i <= (_num_pages - npages); i++)
	{
		if (_is_free(page_i))
		{
			found = 1;
			/*
			 * meet a free page, continue to check if following
			 * (npages - 1) pages are also unallocated.
			 */
			struct Page *page_j = page_i + 1;
			for (int j = i + 1; j < (i + npages); j++)
			{
				if (!_is_free(page_j))
				{
					found = 0;
					break;
				}
				page_j++;
			}
			/*
			 * get a memory block which is good enough for us,
			 * take housekeeping, then return the actual start
			 * address of the first page of this memory block
			 */
			if (found)
			{
				struct Page *page_k = page_i;
				for (int k = i; k < (i + npages); k++)
				{
					_set_flag(page_k, PAGE_TAKEN);
					page_k++;
				}
				page_k--;
				_set_flag(page_k, PAGE_LAST);
				return (void *)(_alloc_start + i * PAGE_SIZE);
			}
		}
		page_i++;
	}
	return NULL;
}

/*
 * Free the memory block
 * - p: start address of the memory block
 */
void page_free(void *p)
{
	/*
	 * Assert (TBD) if p is invalid
	 */

	if (!p || (uint32_t)p >= _alloc_end)
	{
		return;
	}
	/* get the first page descriptor of this memory block */
	struct Page *page = (struct Page *)HEAP_START;
	page += ((uint32_t)p - _alloc_start) / PAGE_SIZE;
	int num = 0;
	/* loop and clear all the page descriptors of the memory block */
	while (!_is_free(page))
	{
		num++;
		if (_is_last(page))
		{
			_clear(page);
			break;
		}
		else
		{
			_clear(page);
			page++;
		}
	}
	printf("开始回收页,回收数目为%d\n", num);
}

typedef struct Block
{
	// 维护一个双向链表
	size_t size;
	uint32_t used;
	struct Block *pre;
	struct Block *next;
} Block;

Block *begin = NULL;

/// @brief 用于malloc,里面实现了更细粒度的内存管理机制
/// @param size
/// @return
void *malloc(size_t size)
{
	// 初始化,分配一个页
	if (begin == NULL)
	{
#ifdef MALLOC_DEBUG
		printf("malloc初始化\n");
#endif
		begin = (Block *)page_alloc(1);
		if (begin == NULL)
		{
			return NULL;
		}
		begin->size = PAGE_SIZE - sizeof(Block);
		begin->used = false;
		begin->next = NULL;
		begin->pre = NULL;
	}
	// 查看现有的页是否可以被使用
	Block *temp = begin, *pre;
	while (temp != NULL)
	{
		if (!temp->used && temp->size >= size)
		{
			// 可以分配
			// 剩下的空间是否足够支撑分裂
			if (temp->size > size + sizeof(Block))
			{
				// 分裂自己
				Block *next = (Block *)((uint8_t *)(temp + 1) + size);
				next->next = temp->next;
				temp->next = next;
				next->pre = temp;
				next->used = false;
				next->size = temp->size - size - sizeof(Block);
				if (next->next != NULL && !next->next->used)
				{
					// 合并后面的,要求页号连续
					int page_now = ((uint32_t)next - _alloc_start) / PAGE_SIZE;
					int page_next = ((uint32_t)next->next - _alloc_start) / PAGE_SIZE;
					bool lianxu = true;
					for (int page = page_now; page < page_next; page++)
					{
						struct Page *pages = (struct Page *)HEAP_START;
						pages += page; // 第几页
						if (_is_last(pages))
						{
							lianxu = false;
							break;
						}
					}
					if (lianxu)
					{
						next->size += next->next->size;
						if (next->next->next != NULL)
						{
							next->next->next->pre = next;
						}
						next->next = next->next->next;
					}
				}
			}
			// 大小刚好或者后面的内存不够分配出一个新的Block的话啥都不用干
			temp->size = size;
			temp->used = true;
			return (uint8_t *)temp + sizeof(Block);
		}
		else
		{
			// 大小不足,找下一块
			pre = temp;
			temp = temp->next;
		}
	}
	// 注意我们要保证页是连续的,否则会出现异常.不允许跨页合并
	// 到这里还没有被分配成功,说明前面的都太小了,需要新开页,在新开的页大小基础上+1
	int num_of_new_page = size / PAGE_SIZE + 1;
	Block *newb = page_alloc(num_of_new_page);
	if (newb == NULL)
	{
		return NULL;
	}
	// 这里因为页不是连续的所以不能跟前面的页合并
	pre->next = newb;
	newb->pre = pre;
	newb->size = size;
	newb->used = true;
	if (num_of_new_page * PAGE_SIZE - size - 2 * sizeof(Block) > 0)
	{
		Block *next = (Block *)((uint8_t *)(newb + 1) + size);
		newb->next = next;
		next->pre = newb;
		next->used = false;
		next->next = NULL;
		next->size = num_of_new_page * PAGE_SIZE - size - 2 * sizeof(Block);
		newb->next = next;
	}
	else
	{
		// 就这样吧,防止他丢失数据
		newb->size = size + num_of_new_page * PAGE_SIZE - size - 2 * sizeof(Block);
		newb->next = NULL;
	}
	return (uint32_t)newb + sizeof(Block);
}

void free(void *ptr)
{
	Block *block = (Block *)((uint32_t)ptr - sizeof(Block));
	block->used = false;

	// 整理内存,向前后合并,然后看看可不可以释放
	if (block->pre != NULL && block->pre->used == false)
	{
		// 如果前面一个块和他是在一个连续区间
		int page_pre = ((uint32_t)block->pre - _alloc_start) / PAGE_SIZE;
		int page_now = ((uint32_t)block - _alloc_start) / PAGE_SIZE;
		bool lianxu = true;
		for (int page = page_pre; page < page_now; page++)
		{
			struct Page *pages = (struct Page *)HEAP_START;
			pages += page; // 第几页
			if (_is_last(pages))
			{
				lianxu = false;
				break;
			}
		}
		if (lianxu)
		{
#ifdef MALLOC_DEBUG
			printf("向前合并\n");
#endif
			// 从前一个到我这都是连续的,可以合并
			Block *pre = block->pre;
			pre->size += block->size + sizeof(Block);
			pre->next = block->next;
			if (block->next != NULL)
			{
				block->next->pre = pre;
			}
			block = pre; // 这里把block变成前面的指针
		}
	}

	if (block->next != NULL && block->next->used == false)
	{
		// 合并后面的
		int page_now = ((uint32_t)block - _alloc_start) / PAGE_SIZE;
		int page_next = ((uint32_t)block->next - _alloc_start) / PAGE_SIZE;
		bool lianxu = true;
		for (int page = page_now; page < page_next; page++)
		{
			struct Page *pages = (struct Page *)HEAP_START;
			pages += page; // 第几页
			if (_is_last(pages))
			{
				lianxu = false;
				break;
			}
		}
		if (lianxu)
		{
#ifdef MALLOC_DEBUG
			printf("向后合并\n");
#endif
			// 从我到后面一个这都是连续的,可以合并
			Block *next = block->next;
			block->size += next->size + sizeof(Block);
			block->next = next->next;
			if (next->next != NULL)
			{
				next->next->pre = block;
			}
		}
	}
	// 至此合并结束,查看block这个空间是否是可回收的
	int size = block->size + sizeof(Block);
	if (size % PAGE_SIZE == 0)
	{
		// 表示是页的整数倍,可能可以被回收
		int page_num = size / PAGE_SIZE;
		int page_now = ((uint32_t)block - _alloc_start) / PAGE_SIZE;
		// 只要看最后一个位置是否是last
		int last_page = page_now + page_num - 1;
		struct Page *pages = (struct Page *)HEAP_START;
		if (_is_last(pages + last_page))
		{
			// 可以回收,回收!
			if (block->pre == NULL)
			{
				begin = block->next;
				if (block->next != NULL)
				{
					block->next->pre = NULL;
				}
				page_free(block);
				return;
			}
			block->pre->next = block->next;
			if (block->next != NULL)
			{
				block->next->pre = block->pre;
			}
			page_free(block);
		}
	}
}