#ifndef	PROJECT1_ALLOC_H
#define PROJECT1_ALLOC_H

#include<new> //Ϊ�����ÿռ�
#include<cstddef> //for ptrdiff_t,size_t
#include<cstdlib> //for exit()
//��������ΪSGI�еĶ�����������ʹ����������������Ƭ���ڴ�Ŀ�֧�����ե�ɿռ�
//�ڴ�����ԽС�����⿪����ռ�ı���Խ��
//������ռ����4096�ֽ�ʱʹ��malloc

namespace mySTL
{
	union  FreeList
	{
		union FreeList*  next;//ָ����һ��FreeList��Ҳ������һ������
		char data[1];//�����ڴ��׵�ַ
	};
	enum
	{
		EAlign128 = 8,
		EAlign256 = 16,
		EAlign512 = 32,
		EAlign1024 = 64,
		EAlign2048 = 128,
		EAlign4096 = 256
	};//��ͬ���鲻ͬ�ϵ���С
	enum { ESmallObjectBytes = 4096 }; // С������ڴ��С
	enum { EFreeListsNumber = 56 };// free lists ����
	//�����ռ�������alloc
	//�ڴ泬��4096bytesʱ��ֱ�ӵ���std����malloc��std����free��ɿռ�����
	//�ڴ��Сʱ��ʹ���ڴ�ع���ά����ÿ����һ���ڴ棬ά����Ӧ����������
	class alloc// ���ڲ��漰���̣߳�����δʹ��ģ����
	{
	private:
		static size_t heap_size;//����heap�ռ��С
		static char* start_freelist;//�ڴ����ʼλ�ã�ֻ��M_chunk_alloc()�仯
		static char* end_freelist;//�ڴ�ؽ���λ�ã�ֻ��M_chunk_alloc()�仯
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
	//��̬��Ա������ʼ��
	char* alloc::start_freelist = nullptr;
	char* alloc::end_freelist = nullptr;
	size_t alloc::heap_size = 0;
	//��56��������г�ʼ��
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
		my_free_list = free_list[M_freelist_index(n)];//�ҵ���Ӧfreelist
		result = my_free_list;
		if (result == nullptr)
		{
			void *r = M_refill(M_round_up(n));
			return r;
		}
		//����free list ,������һ���ڵ�
		my_free_list = result->next;
		return(result);

	}
	// �ͷ� p ָ��Ĵ�СΪ n �Ŀռ�, p ����Ϊ 0
	inline  void alloc::deallocate(void* p, size_t n)
	{
		FreeList* q = reinterpret_cast<FreeList*>(p);
		FreeList* my_free_list;
		if (n > static_cast<size_t>(ESmallObjectBytes))
		{
			std::free(p);
			return;
		}
		//����union ���ԣ�ʵ���������
		my_free_list = free_list[M_freelist_index(n)];
		q->next = my_free_list;
		my_free_list = q;
	}
	// ���·���ռ䣬������������������һΪָ���¿ռ��ָ�룬������Ϊԭ���ռ�Ĵ�С��������Ϊ����ռ�Ĵ�С
	inline void* alloc::reallocate(void *p, size_t old_size, size_t new_size)
	{
		deallocate(p, old_size);
		p = allocate(new_size);
		return p;
	}
	//��bytes�ϵ�����
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
	// �� bytes �ϵ�����Ӧ�����С
	inline size_t alloc::M_round_up(size_t n)
	{
		return ((n + M_align(n) - 1) & ~(M_align(n) - 1));
	}
	// ���������С��ѡ��� n �� free lists
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
		// ���ֻ��һ�����飬�Ͱ�������鷵�ظ������ߣ�free list û�������½ڵ�
		if (nobjs == 1) return(chunk);
		//�ҵ���Ӧfreelist
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
	//������������ ���������������С�����ڴ����ȡ�ռ�� free list ʹ�ã�����������ʱ������� nobjs
	char* alloc::M_chunk_alloc(size_t size, size_t &nobjs)
	{
		char* result;
		size_t need_bytes = size*nobjs;
		size_t bytes_left = end_freelist - start_freelist;//�ڴ�ʣ��ռ�
		if (bytes_left >= need_bytes)
		{
			result = start_freelist;
			start_freelist += need_bytes;//�����ڴ���ʼλ��
			return result;
		}//ʣ��ռ䲻�㣬�����㹻�ṩһ�������ϵ�����
		else if (bytes_left >= size)
		{
			nobjs = bytes_left / size;//�ṩ��������Ŀ
			need_bytes = nobjs*size;//�����趨�ܴ�С
			result = start_freelist;
			start_freelist += need_bytes;
			return start_freelist;
		}
		//һ�������С������ʱ
		else
		{
			if (bytes_left >= 0)
			{
				//�ڴ���ڻ���һ������ͷ���ȷ�����ʵ���free list
				//Ѱ�Һ��ʵ�free list
				FreeList* my_free_list = free_list[M_freelist_index(bytes_left)];
				((FreeList*)start_freelist)->next = my_free_list;
				my_free_list = (FreeList*)start_freelist;
			}
			//����heap�ռ䣬�����ڴ��
			size_t bytes_to_get = 2 * need_bytes + M_round_up(heap_size >> 4);
			start_freelist = (char*)std::malloc(bytes_to_get);
			//��heap�ռ�Ҳ����������,��freelist��Ѱ���㹻��Ŀ�������
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
						//����nobjs
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
