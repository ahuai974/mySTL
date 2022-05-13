#ifndef	PROJECT1_ALLOC_H
#define PROJECT1_ALLOC_H

#include<new> //为了配置空间
#include<cstddef> //for ptrdiff_t,size_t
#include<cstdlib> //for exit()
//此配置器为SGI中的二级配置器，使用自由链表，减少碎片化内存的开支，充分榨干空间
//内存区块越小，额外开销所占的比例越大
//当分配空间大于4096字节时使用malloc

namespace mySTL
{
	union  FreeList
	{
		union FreeList*  next;//指向下一个FreeList，也就是下一个区块
		char data[1];//本块内存首地址
	};
	enum
	{
		EAlign128 = 8,
		EAlign256 = 16,
		EAlign512 = 32,
		EAlign1024 = 64,
		EAlign2048 = 128,
		EAlign4096 = 256
	};//不同区块不同上调大小
	enum { ESmallObjectBytes = 4096 }; // 小对象的内存大小
	enum { EFreeListsNumber = 56 };// free lists 个数
	//二级空间配置器alloc
	//内存超过4096bytes时，直接调用std：：malloc，std：：free完成空间配置
	//内存较小时，使用内存池管理维护，每配置一块内存，维护对应的自由链表
	class alloc// 由于不涉及多线程，这里未使用模板编程
	{
	private:
		static size_t heap_size;//分配heap空间大小
		static char* start_freelist;//内存池起始位置，只在M_chunk_alloc()变化
		static char* end_freelist;//内存池结束位置，只在M_chunk_alloc()变化
		static FreeList* free_list[EFreeListsNumber];
	public:
		static void* allocate(size_t n);
		static void  deallocate(void *p, size_t n);
		static void* reallocate(void *p, size_t old_size, size_t new_size);
	private:
		static size_t M_align(size_t bytes);
		static size_t M_round_up(size_t bytes);
		static size_t M_freelist_index(size_t bytes);
		static void*  M_refill(size_t n);
		static char*  M_chunk_alloc(size_t size, size_t &nobj);


	};
	//静态成员变量初始化
	char* alloc::start_freelist = nullptr;
	char* alloc::end_freelist = nullptr;
	size_t alloc::heap_size = 0;
	//对56个区块进行初始化
	FreeList* alloc::free_list[EFreeListsNumber] = {
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
		nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr
	};
	inline void* alloc::allocate(size_t n)
	{
		FreeList* my_free_list;
		FreeList* result;
		if (n > static_cast<size_t>(ESmallObjectBytes))
		{
			return std::malloc(n);
		}
		my_free_list = free_list[M_freelist_index(n)];//找到对应freelist
		result = my_free_list;
		if (result == nullptr)
		{
			void *r = M_refill(M_round_up(n));
			return r;
		}
		//调整free list ,拨出第一个节点
		my_free_list = result->next;
		return(result);

	}
	// 释放 p 指向的大小为 n 的空间, p 不能为 0
	inline  void alloc::deallocate(void* p, size_t n)
	{
		FreeList* q = reinterpret_cast<FreeList*>(p);
		FreeList* my_free_list;
		if (n > static_cast<size_t>(ESmallObjectBytes))
		{
			std::free(p);
			return;
		}
		//由于union 特性，实现区块回收
		my_free_list = free_list[M_freelist_index(n)];
		q->next = my_free_list;
		my_free_list = q;
	}
	// 重新分配空间，接受三个参数，参数一为指向新空间的指针，参数二为原来空间的大小，参数三为申请空间的大小
	inline void* alloc::reallocate(void *p, size_t old_size, size_t new_size)
	{
		deallocate(p, old_size);
		p = allocate(new_size);
		return p;
	}
	//将bytes上调对齐
	inline size_t alloc::M_align(size_t bytes)
	{
		if (bytes <= 512)
		{
			return bytes <= 256
				? bytes <= 128 ? EAlign128 : EAlign256
				: EAlign512;
		}
		return bytes <= 2048
			? bytes <= 1024 ? EAlign1024 : EAlign2048
			: EAlign4096;
	}
	// 将 bytes 上调至对应区间大小
	inline size_t alloc::M_round_up(size_t n)
	{
		return ((n + M_align(n) - 1) & ~(M_align(n) - 1));
	}
	// 根据区块大小，选择第 n 个 free lists
	inline size_t alloc::M_freelist_index(size_t bytes)
	{
		if (bytes <= 512)
		{
			return bytes <= 256
				? bytes <= 128
				? ((bytes + EAlign128 - 1) / EAlign128 - 1)
				: (15 + (bytes + EAlign256 - 129) / EAlign256)
				: (23 + (bytes + EAlign512 - 257) / EAlign512);
		}
		return bytes <= 2048
			? bytes <= 1024
			? (31 + (bytes + EAlign1024 - 513) / EAlign1024)
			: (39 + (bytes + EAlign2048 - 1025) / EAlign2048)
			: (47 + (bytes + EAlign4096 - 2049) / EAlign4096);
	}
	void* alloc::M_refill(size_t n)
	{
		size_t nobjs = 10;
		char* chunk = M_chunk_alloc(n, nobjs);
		FreeList * my_free_list;
		FreeList * result;
		FreeList * cur, *next;
		// 如果只有一个区块，就把这个区块返回给调用者，free list 没有增加新节点
		if (nobjs == 1) return(chunk);
		//找到对应freelist
		my_free_list = free_list[M_freelist_index(n)];
		result = (FreeList*)chunk;
		my_free_list = next = (FreeList*)(chunk + n);
		for (size_t i = 1; ; i++)
		{
			cur = next;
			next = (FreeList*)((char*)next + n);
			if (nobjs - 1 == i)
			{
				cur->next = nullptr;
				break;
			}
			else
			{
				cur->next = next;
			}
		}
		return result;
	}
	//接受两个参数 区块数量、区块大小。从内存池中取空间给 free list 使用，条件不允许时，会调整 nobjs
	char* alloc::M_chunk_alloc(size_t size, size_t &nobjs)
	{
		char* result;
		size_t need_bytes = size*nobjs;
		size_t bytes_left = end_freelist - start_freelist;//内存剩余空间
		if (bytes_left >= need_bytes)
		{
			result = start_freelist;
			start_freelist += need_bytes;//更新内存起始位置
			return result;
		}//剩余空间不足，但又足够提供一个或以上的区块
		else if (bytes_left >= size)
		{
			nobjs = bytes_left / size;//提供的区块数目
			need_bytes = nobjs*size;//重新设定总大小
			result = start_freelist;
			start_freelist += need_bytes;
			return start_freelist;
		}
		//一个区块大小都不够时
		else
		{
			if (bytes_left >= 0)
			{
				//内存池内还有一部分零头，先分配给适当的free list
				//寻找合适的free list
				FreeList* my_free_list = free_list[M_freelist_index(bytes_left)];
				((FreeList*)start_freelist)->next = my_free_list;
				my_free_list = (FreeList*)start_freelist;
			}
			//配置heap空间，补充内存池
			size_t bytes_to_get = 2 * need_bytes + M_round_up(heap_size >> 4);
			start_freelist = (char*)std::malloc(bytes_to_get);
			//在heap空间也不足的情况下,在freelist中寻找足够大的可用区块
			if (!start_freelist)
			{
				size_t i;
				FreeList *my_free_list, *p;
				for (i = size; i <= ESmallObjectBytes; i += M_align(i))
				{
					my_free_list = free_list[M_freelist_index(i)];
					p = my_free_list;
					if (p != nullptr)
					{
						my_free_list = p->next;
						start_freelist = (char*)p;
						end_freelist = start_freelist + i;
						//修正nobjs
						return (M_chunk_alloc(size, nobjs));
					}
				}
				end_freelist = nullptr;
				throw std::bad_alloc();

			}
			end_freelist = start_freelist + bytes_to_get;
			heap_size += bytes_to_get;
			return M_chunk_alloc(size, nobjs);
		}
	}
	
}
#endif // !MYSTL_ALLOC_H
