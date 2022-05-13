#ifndef PROJECT1_ALLOCATOR_H
#define	PROJECT1_ALLOCATOR_H
#include"construct.h"
#include"util.h"
//���ļ�ʵ�ֵ���STL�е�һ���ռ���������ֱ��ʹ��construct��
//������ֱ��ʹ��new��delete��new��delete������malloc��free
namespace mySTL
{
	template<class T>
	class allocator
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef ptrdiff_t difference_type;
		typedef size_t size_type;
		typedef const T* const_pointer;
		typedef const T& consg_reference;
		static T* allocate();
		static T* allocate(size_type n);
		static void dellocate(T* ptr);
		static void dellocate(T* ptr size_type n);
		static void construct(T* ptr);
		static void construct(T* prt, const T& value);
		static void construct(T* ptr, T&& value);
		template<class...Args>
		static void construct(T* ptr, Args&&...args);
		static void destory(T* ptr);
		static void destory(T* first, T* last);
	};
	//�ռ�����������ģ��
	template<class T>
	T* allocator<T>::allocate()
	{
		return static_cast<T*>(::operator new(sizeof(T)));
	}
	//allocator ���ذ汾 ����n��T�Ŀռ�
	template <class T> 
	T* allocator<T>::allocate(size_type n)
	{
		if (n == 0)
			return nullptr;
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}
	//�ͷſռ�
	template<class T>
	void allocator<T>::deallocate(T* ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}
		::operator delete(ptr);
	}
	template <class T>
	void allocator<T>::deallocate(T* ptr, size_type /*size*/)
	{
		if (ptr == nullptr)
			return;
		::operator delete(ptr);
	}
	//ʹ��construct.h�еĹ���
	template <class T>
	void allocator<T>::construct(T* ptr)
	{
		mystl::construct(ptr);
	}
	template <class T>
	void allocator<T>::construct(T* ptr, const T& value)
	{
		mystl::construct(ptr, value);
	}

	template <class T>
	void allocator<T>::construct(T* ptr, T&& value)
	{
		mystl::construct(ptr, mystl::move(value));
	}

	template <class T>
	template <class ...Args>
	void allocator<T>::construct(T* ptr, Args&& ...args)
	{
		mystl::construct(ptr, mystl::forward<Args>(args)...);
	}

	template <class T>
	void allocator<T>::destroy(T* ptr)
	{
		mystl::destroy(ptr);
	}

	template <class T>
	void allocator<T>::destroy(T* first, T* last)
	{
		mystl::destroy(first, last);
	}

}
#endif // !PROJECT1_ALLOCATOR_H
