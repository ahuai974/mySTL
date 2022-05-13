#ifndef PROJECT1_UTIL_H
#define PROJECT1_UTIL_H

#include<cstddef>//for ptrdifff_t����
#include"type_traits.h"
//���ļ�����move��forward��swap�����������ذ汾������pair�ṹ���������������
namespace mySTL
{
	template<class T>
	//move����������ǿ��ת��Ϊ��ֵ����ȡ�����ã���ǿ��ת��
	//remove_reference ���ý��������ֵ����ֵ���ö�����
	typename std::remove_reference<T>::type&& move(T&& arg) noexcept//���׳��쳣
	{
		return static_cast<typename std::remove_reference<T>::type&&>(arg);
	}
	// ʹ��move������Ϊ����ʱ���޷����ñ�������ʹ��forward������Ϊ����ʹ��
	template <class T>
	T&& forward(typename std::remove_reference<T>::type& arg) noexcept
	{
		return static_cast<T&&>(arg);
	}
	template<class T>
	T&& forward(typename std::remove_reference<T>::type&& arg) noexcept//��� T �Ƿ�Ϊ��ֵ�������͡�
	//�� T ����ֵ�������ͣ����ṩ���� true �ĳ�Ա���� value ������ value ���� false 
	{
		static_assert(!std::is_lvalue_reference<T>::value, "bad forward");//��̬����
		return static_cast<T&&>(arg);
	}
	//����,����ʹ����ֵ���ã�c++11�����ԣ����ٸ��Ʋ��������Ч�ʡ�
	template<class Tp>
	void swap(Tp& lhs, Tp& rhs)
	{
		auto tmp(mySTL::move(lhs));
		lhs = mySTL::move(rhs);
		rhs = mySTL::move(tmp);
	}
	//��Χ��swap
	template <class ForwardIter1, class ForwardIter2>
	ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2)
	{
		for (; first1 != last1; ++first1, (void) ++first2)
			mystl::swap(*first1, *first2);
		return first2;
	}
	//���ڴ������[]�±���ʽ���ػ��汾
	template <class Tp, size_t N>
	void swap(Tp(&a)[N], Tp(&b)[N])
	{
		mystl::swap_range(a, a + N, b);
	}
	// �ṹ��ģ�� : pair
	// ����ģ������ֱ��ʾ�������ݵ�����
	// �� first �� second ���ֱ�ȡ����һ�����ݺ͵ڶ�������
	template<class Ty1,class Ty2>
	struct pair
	{
		typedef Ty1 first_type;
		typedef Ty2 second_type;
		first_type first;
		second_type second;
		// is_default_constructible �ж��Ƿ���Ĭ�Ϲ��캯��
		// enble_if �޶�����Ϊtrue����other1��other2��Ҫ��Ĭ�Ϲ��캯��
		//enable_if��������Ҫ����ģ���ƥ�䣬ƫ�ػ���һ�����̡�
		//�������������Ƶ��Ĺ����У��᳢���Ƶ����е����غ�����
		//�ڴ˹����ڹ����У����enable_if���������㣬����ں�ѡ�����������޳��˺�����
		template<class Other1 = Ty1,class Other2 = Ty2, 
			typename = typename std::enable_if<std::is_default_constructible<Other1>::value&&
			std::is_default_constructible<Other2>::value, void>::type> 
			constexpr pair()
			:first(), second()
		{
		}
		//is_copy_constructible �ж��Ƿ��п������캯��
		//is_convertible �ж��ܷ���ʽ����ת��
		template<class U1 = Ty1, class U2 = Ty2 ,
			typename std::enable_if<
			std::is_copy_constructible<U1>::value&&
			std::is_copy_constructible<U2>::value&&
			std::is_convertible<const U1&, Ty1>::value&&
			std::is_convertible<const U2&, Ty2>::value, int>::type = 0>
			constexpr pair(const Ty1& a, const Ty2& b)
			: first(a), second(b)
		{
		}
		// ��std::is_convertible����false���ֹ��ʽת��
		template <class U1 = Ty1, class U2 = Ty2,
			typename std::enable_if<
			std::is_copy_constructible<U1>::value&&
			std::is_copy_constructible<U2>::value &&
			(!std::is_convertible<const U1&, Ty1>::value ||
				!std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
			explicit constexpr pair(const Ty1& a, const Ty2& b)
			: first(a), second(b)
		{
		}
		//�Զ����ɺ�����
		pair(const pair& rhs) = default;
		pair(pair && rhs) = default;

		//std::is_constructible��һ���������Ϳɷ���ʣ�������ת�������������õ�һ�������Ĺ��캯����
		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, Other1>::value&&
			std::is_constructible<Ty2, Other2>::value&&
			std::is_convertible<Other1&&, Ty1>::value&&
			std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
			constexpr pair(Other1&& a, Other2&& b)
			: first(mystl::forward<Other1>(a)),
			second(mystl::forward<Other2>(b))
		{
		}
		//��is_constructible�ж�Ϊ��ʱ����ֹ��ʽת��
		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, Other1>::value&&
			std::is_constructible<Ty2, Other2>::value &&
			(!std::is_convertible<Other1, Ty1>::value ||
				!std::is_convertible<Other2, Ty2>::value), int>::type = 0>
			explicit constexpr pair(Other1&& a, Other2&& b)
			: first(mystl::forward<Other1>(a)),
			second(mystl::forward<Other2>(b))
		{
		}
		//��������
		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, const Other1&>::value&&
			std::is_constructible<Ty2, const Other2&>::value&&
			std::is_convertible<const Other1&, Ty1>::value&&
			std::is_convertible<const Other2&, Ty2>::value, int>::type = 0>
			constexpr pair(const pair<Other1, Other2>& other)
			: first(other.first),
			second(other.second)
		{
		}
		//��ֹ��ʽת���Ŀ�������
		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, const Other1&>::value&&
			std::is_constructible<Ty2, const Other2&>::value &&
			(!std::is_convertible<const Other1&, Ty1>::value ||
				!std::is_convertible<const Other2&, Ty2>::value), int>::type = 0>
			explicit constexpr pair(const pair<Other1, Other2>& other)
			: first(other.first),
			second(other.second)
		{
		}
		//��ֵ���ÿ���������ػ��汾
		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, Other1>::value&&
			std::is_constructible<Ty2, Other2>::value&&
			std::is_convertible<Other1, Ty1>::value&&
			std::is_convertible<Other2, Ty2>::value, int>::type = 0>
			constexpr pair(pair<Other1, Other2>&& other)
			: first(mystl::forward<Other1>(other.first)),
			second(mystl::forward<Other2>(other.second))
		{
		}
		//��ֵ���ÿ���������ػ��汾(��ֹ��������)
		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, Other1>::value&&
			std::is_constructible<Ty2, Other2>::value &&
			(!std::is_convertible<Other1, Ty1>::value ||
				!std::is_convertible<Other2, Ty2>::value), int>::type = 0>
			explicit constexpr pair(pair<Other1, Other2>&& other)
			: first(mystl::forward<Other1>(other.first)),
			second(mystl::forward<Other2>(other.second))
		{
		}
		//���ز�����
		pair& operator=(const pair& rhs)
		{
			if (rhs != this)
			{
				first = rhs.first;
				second = rhs.second;
			}
			return *this;
		}

		pair& operator=(pair&& rhs)
		{
			if (this != &rhs)
			{
				first = mystl::move(rhs.first);
				second = mystl::move(rhs.second);
			}
			return *this;
		}

		template <class Other1, class Other2>
		pair& operator=(const pair<Other1, Other2>& other)
		{
			first = other.first;
			second = other.second;
			return *this;
		}

		template <class Other1, class Other2>
		pair& operator=(pair<Other1, Other2>&& other)
		{
			first = mystl::forward<Other1>(other.first);
			second = mystl::forward<Other2>(other.second);
			return *this;
		}

		~pair() = default;

		void swap(pair & other)
		{
			if (this != &other)
			{
				mystl::swap(first, other.first);
				mystl::swap(second, other.second);
			}
		}
	};
	// ���رȽϲ����� 
	template <class Ty1, class Ty2>
	bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
	{
		return lhs.first == rhs.first && lhs.second == rhs.second;
	}

	template <class Ty1, class Ty2>
	bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
	{
		return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
	}

	template <class Ty1, class Ty2>
	bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class Ty1, class Ty2>
	bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
	{
		return rhs < lhs;
	}

	template <class Ty1, class Ty2>
	bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
	{
		return !(rhs < lhs);
	}

	template <class Ty1, class Ty2>
	bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
	{
		return !(lhs < rhs);
	}

	// ���� mystl �� swap
	template <class Ty1, class Ty2>
	void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
	{
		lhs.swap(rhs);
	}

	// ȫ�ֺ��������������ݳ�Ϊһ�� pair
	template <class Ty1, class Ty2>
	pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
	{
		return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
	}


}
#endif //PROJECT1_UTIL_H
