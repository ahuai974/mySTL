#ifndef PROJECT1_CONSTRUCT_H
#define	PROJECT1_CONSTRUCT_H
#include<new>;
#include"type_traits.h"
#include"iterator.h"

#ifdef _MSC_VER
#pragma warning(push)//存储警报
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER
namespace mySTL
{
	template<class Ty>
	void construct(Ty* ptr)
	{
		::new((void*)ptr) Ty();//默认构造
	}
	template<class Ty1,class Ty2>
	void construct(Ty1*, ptr, const Ty2& value)
	{
		::new((void*)ptr) Ty1(value); //赋值构造
	}
	template <class Ty, class... Args>
	void construct(Ty* ptr, Args&&... args)//右值引用
	{
		::new ((void*)ptr) Ty(mystl::forward<Args>(args)...);
	}

	//析构对象，拥有两个版本，分别对应对象是否自带析构函数（带为false，不带为true）

	template<class Ty>
	void destory_one(Ty, std::true_type) {}

	template <class Ty>
	void destroy_one(Ty* pointer, std::false_type)
	{
		if (pointer != nullptr)
		{
			pointer->~Ty();
		}
	}
	//针对迭代器的范围析构特化版本
	template <class ForwardIter>
	void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

	template <class ForwardIter>
	void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
	{
		for (; first != last; ++first)
			destroy(&*first);
	}

	//is_trivially_destructible判断类型是否有析构函数，有返回false，反之true。
	template <class Ty>
	void destroy(Ty* pointer)
	{
		destroy_one(pointer, std::is_trivially_destructible<Ty>{});
	}
	//由于传入为迭代器，需要萃取出迭代器所指元素的类别
	template <class ForwardIter>
	void destroy(ForwardIter first, ForwardIter last)
	{
		destroy_cat(first, last, std::is_trivially_destructible<
			typename iterator_traits<ForwardIter>::value_type>{});
	}

}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
#endif // !PROJECT1_CONSTRUCT_H
