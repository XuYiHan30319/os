#include "os.h"

/*
 * Following global vars are defined in mem.S
 */
// extern表示该变量定义在其他文件中要去找
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

#define PAGE_TAKEN (uint8_t)(1 << 0) // 为了节约内存,我们以位运算的形式定义flag,第0为表示有没有被使用,第一位表示有没有结束
#define PAGE_LAST (uint8_t)(1 << 1)

#define PTR_LEN ((uint32_t)0x7FFFFFFF) // 是用&运算得到对应位置的值
#define PTR_USED (uint32_t)(1 << 31)
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
// static和inline联合使用既能提高程序的运行效率又能让其声明后被多个.c文件使用。
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
	 * 这里的话是因为每一个bit可以对应一个页的状况,所以我们足够满足128MB的存储要求
	 */
	_num_pages = (HEAP_SIZE / PAGE_SIZE) - 8;
	printf("HEAP_START = %x, HEAP_SIZE = %x, num of pages = %d\n", HEAP_START, HEAP_SIZE, _num_pages);

	struct Page *page = (struct Page *)HEAP_START; // 这个类用来管理,相当于是一个头
	for (int i = 0; i < _num_pages; i++)
	{
		_clear(page);
		page++;
	}

	_alloc_start = _align_page(HEAP_START + 8 * PAGE_SIZE); // 页真正开始的位置
	_alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);	// 页的结束位置

	printf("TEXT:   0x%x -> 0x%x\n", TEXT_START, TEXT_END);
	printf("RODATA: 0x%x -> 0x%x\n", RODATA_START, RODATA_END);
	printf("DATA:   0x%x -> 0x%x\n", DATA_START, DATA_END);
	printf("BSS:    0x%x -> 0x%x\n", BSS_START, BSS_END);
	printf("HEAP:   0x%x -> 0x%x\n", _alloc_start, _alloc_end);
}

/*
 * Allocate a memory block which is composed of contiguous physical pages
 * - npages: the number of PAGE_SIZE pages to allocate
 * 分配一块连续的内存空间
 */
void *page_alloc(int npages)
{
	uint8_t find = 0;
	struct Page *page_start = (struct Page *)HEAP_START;
	for (int i = 0; i < _num_pages - npages; i++)
	{
		if (_is_free(page_start))
		{
			find = 1;
			struct Page *page_j = page_start + 1;
			for (int j = i + 1; j < i + _num_pages; j++)
			{
				if (!_is_free(page_j))
				{
					find = 0;
					break;
				}
				else
				{
					page_j++;
				}
			}
			if (find)
			{
				struct Page *page_k = page_start;
				for (int k = 0; k < npages; k++)
				{
					_set_flag(page_k, PAGE_TAKEN);
					page_k++;
				}
				page_k--;
				_set_flag(page_k, PAGE_LAST);
				return (void *)(_alloc_start + PAGE_SIZE * i);
			}
		}
		page_start++;
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
	{ // p为空或者p越界
		return;
	}
	/* get the first page descriptor of this memory block */
	struct Page *page = (struct Page *)HEAP_START; // 作为指针指向一块内存空间,就是当前的位置
	page += ((uint32_t)p - _alloc_start) / PAGE_SIZE;
	/* loop and clear all the page descriptors of the memory block */
	while (!_is_free(page))
	{
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
}

void page_test()
{
	void *p = page_alloc(2);
	printf("p = 0x%x\n", p);
	// page_free(p);

	void *p2 = page_alloc(7);
	printf("p2 = 0x%x\n", p2);
	page_free(p2);

	void *p3 = page_alloc(4);
	printf("p3 = 0x%x\n", p3);
}

void *malloc(size_t size)
{
	// 一个指针32位,第31位1表示使用中,后31位表示长度
}

void free(void *ptr)
{
}