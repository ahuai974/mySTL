#ifndef PROJECT1_UTIL_H
#define PROJECT1_UTIL_H

#include<cstddef>//for ptrdifff_t类型
#include"type_traits.h"
//此文件定义move，forward，swap函数与其重载版本，定义pair结构，重载其操作符。
namespace mySTL
{
	template<class T>
	//move函数将参数强制转换为右值，先取消引用，再强制转换
	//remove_reference 引用解除，对左值与右值引用都可行
	typename std::remove_reference<T>::type&& move(T&& arg) noexcept//不抛出异常
	{
		return static_cast<typename std::remove_reference<T>::type&&>(arg);
	}
	// 使用move（）作为参数时会无法调用报错，所以使用forward（）作为过渡使用
	template <class T>
	T&& forward(typename std::remove_reference<T>::type& arg) noexcept
	{
		return static_cast<T&&>(arg);
	}
	template<class T>
	T&& forward(typename std::remove_reference<T>::type&& arg) noexcept//检查 T 是否为左值引用类型。
	//若 T 是左值引用类型，则提供等于 true 的成员常量 value ，否则， value 等于 false 
	{
		static_assert(!std::is_lvalue_reference<T>::value, "bad forward");//静态断言
		return static_cast<T&&>(arg);
	}
	//交换,这里使用右值引用（c++11新特性）减少复制操作，提高效率。
	template<class Tp>
	void swap(Tp& lhs, Tp& rhs)
	{
		auto tmp(mySTL::move(lhs));
		lhs = mySTL::move(rhs);
		rhs = mySTL::move(tmp);
	}
	//范围性swap
	template <class ForwardIter1, class ForwardIter2>
	ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2)
	{
		for (; first1 != last1; ++first1, (void) ++first2)
			mystl::swap(*first1, *first2);
		return first2;
	}
	//对于传入的是[]下标形式的特化版本
	template <class Tp, size_t N>
	void swap(Tp(&a)[N], Tp(&b)[N])
	{
		mystl::swap_range(a, a + N, b);
	}
	// 结构体模板 : pair
	// 两个模板参数分别表示两个数据的类型
	// 用 first 和 second 来分别取出第一个数据和第二个数据
	template<class Ty1,class Ty2>
	struct pair
	{
		typedef Ty1 first_type;
		typedef Ty2 second_type;
		first_type first;
		second_type second;
		// is_default_constructible 判断是否有默认构造函数
		// enble_if 限定必须为true，即other1，other2都要有默认构造函数
		//enable_if的作用主要用于模板的匹配，偏特化的一个过程。
		//编译器在类型推导的过程中，会尝试推导所有的重载函数，
		//在此过程在过程中，如果enable_if条件不满足，则会在候选函数集合中剔除此函数。
		template<class Other1 = Ty1,class Other2 = Ty2, 
			typename = typename std::enable_if<std::is_default_constructible<Other1>::value&&
			std::is_default_constructible<Other2>::value, void>::type> 
			constexpr pair()
			:first(), second()
		{
		}
		//is_copy_constructible 判断是否有拷贝构造函数
		//is_convertible 判断能否隐式类型转换
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
		// 若std::is_convertible返回false则禁止隐式转换
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
		//自动生成函数体
		pair(const pair& rhs) = default;
		pair(pair && rhs) = default;

		//std::is_constructible第一个参数类型可否由剩余参数（转换或代入参数调用第一个参数的构造函数）
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
		//当is_constructible判断为否时，禁止隐式转换
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
		//拷贝构造
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
		//静止隐式转换的拷贝构造
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
		//右值引用拷贝构造的特化版本
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
		//右值引用拷贝构造的特化版本(禁止拷贝构造)
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
		//重载操作符
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
	// 重载比较操作符 
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

	// 重载 mystl 的 swap
	template <class Ty1, class Ty2>
	void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
	{
		lhs.swap(rhs);
	}

	// 全局函数，让两个数据成为一个 pair
	template <class Ty1, class Ty2>
	pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
	{
		return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
	}


}
#endif //PROJECT1_UTIL_H
