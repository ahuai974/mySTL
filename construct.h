#ifndef PROJECT1_CONSTRUCT_H
#define	PROJECT1_CONSTRUCT_H
#include<new>;
#include"type_traits.h"
#include"iterator.h"

#ifdef _MSC_VER
#pragma warning(push)//�洢����
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER
namespace mySTL
{
	template<class Ty>
	void construct(Ty* ptr)
	{
		::new((void*)ptr) Ty();//Ĭ�Ϲ���
	}
	template<class Ty1,class Ty2>
	void construct(Ty1*, ptr, const Ty2& value)
	{
		::new((void*)ptr) Ty1(value); //��ֵ����
	}
	template <class Ty, class... Args>
	void construct(Ty* ptr, Args&&... args)//��ֵ����
	{
		::new ((void*)ptr) Ty(mystl::forward<Args>(args)...);
	}

	//��������ӵ�������汾���ֱ��Ӧ�����Ƿ��Դ�������������Ϊfalse������Ϊtrue��

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
	//��Ե������ķ�Χ�����ػ��汾
	template <class ForwardIter>
	void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

	template <class ForwardIter>
	void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
	{
		for (; first != last; ++first)
			destroy(&*first);
	}

	//is_trivially_destructible�ж������Ƿ��������������з���false����֮true��
	template <class Ty>
	void destroy(Ty* pointer)
	{
		destroy_one(pointer, std::is_trivially_destructible<Ty>{});
	}
	//���ڴ���Ϊ����������Ҫ��ȡ����������ָԪ�ص����
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
