#ifndef PROJECT1_ALGOBASE_H
#define PROJECT1_ALGOBASE_H
#include<cstring>
#include<type_traits>
#include"iterator.h"
#include"util.h"
//此文件定义了min、max、swap、copy、move、fill、eaqul、mismatch函数。针对不同的迭代器有不同的重载版本
namespace mySTL
{
#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min
	template<class T>
	//返回较大者
	const T& max(const T& lhs, const T& rhs)
	{
		return lhs < rhs ? rhs : lhs;
	}
	//重载版本，使用compare函数代替比较操作
	template<class T,class Compare>
	const T& max(const T& lhs, const T& rhs, Compare copmare)
	{
		return compare(lhs, rhs) ? rhs : lhs;
	}
	//返回较小者
	template<class T>
	const T& min(const T& lhs, const T&rhs)
	{
		return lhs < rhs ? lhs : rhs;
	}
	//重载版本，使用compare函数代替比较操作
	template<class T, class Compare>
	const T& min(const T& lhs, const T& rhs, Compare copmare)
	{
		return compare(lhs, rhs) ? lhs : rhs;
	}
	//传入参数为迭代器
	template<class FIter1,class FIter2>
	void iter_swap(FIter1 lhs, FIter2 rhs)
	{
		return mySTL::swap(*lhs,*rhs)
	} 
	//copy
	//把[first,last]区间的元素拷贝道[result,result+(last-first)]中
	//input_iterator_tag 版本
	template<class InputIter,class OutputIter>
	OutputIter unchecked_copy_cat(InputIter first, InputIter last, OutputIter result,
		mySTL::input_iterator_tag)
	{
		for (; first != last; first++, result++)
		{
			*result = *first;
		}
		return result;
	}
	// unchecked_copy_backward_cat 的 random_access_iterator_tag 的特化版本
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result, mySTL::random_access_iterator_tag)
	{
		for (; first != last; first++, result++)
		{
			*result = *first;
		}
		return result;
	}
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result)
	{
		return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
	}
	// 为 trivially_copy_assignable 类型提供特化版本
	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
		std::is_trivially_copy_assignable<Up>::value,
		Up*>::type
		unchecked_copy_backward(Tp* first, Tp* last, Up* result)
	{
		const auto n = static_cast<size_t>(last - first);
		if (n != 0)
		{
			result -= n;
			std::memmove(result, first, n * sizeof(Up));
		}
		return result;
	}
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
	{
		return unchecked_copy_backward(first, last, result);
	}

	// copy_if
	// 把[first, last)内满足一元操作 unary_pred 的元素拷贝到以 result 为起始的位置上
	template <class InputIter, class OutputIter, class UnaryPredicate>
	OutputIter
		copy_if(InputIter first, InputIter last, OutputIter result, UnaryPredicate unary_pred)
	{
		for (; first != last; ++first)
		{
			if (unary_pred(*first))
				*result++ = *first;
		}
		return result;
	}
	// copy_n
	// 把 [first, first + n)区间上的元素拷贝到 [result, result + n)上
	// 返回一个 pair 分别指向拷贝结束的尾部
	template <class InputIter, class Size, class OutputIter>
	mySTL::pair<InputIter, OutputIter>
		unchecked_copy_n(InputIter first, Size n, OutputIter result, mySTL::input_iterator_tag)
	{
		for (; n > 0; --n, ++first, ++result)
		{
			*result = *first;
		}
		return mystl::pair<InputIter, OutputIter>(first, result);
	}
	//random_access_iterator_tag 的特化版本
	template <class RandomIter, class Size, class OutputIter>
	mySTL::pair<RandomIter, OutputIter>
		unchecked_copy_n(RandomIter first, Size n, OutputIter result, mySTL::random_access_iterator_tag)
	{
		auto last = first + n;
		return mySTL::pair<RandomIter, OutputIter>(last, mySTL::copy(first, last, result));
	}
	template <class InputIter, class Size, class OutputIter>
	mySTL::pair<InputIter, OutputIter>
		copy_n(InputIter first, Size n, OutputIter result)
	{
		return unchecked_copy_n(first, n, result, iterator_category(first));
	}
	//************************************************************************
	// move
	// 把 [first, last)区间内的元素移动到 [result, result + (last - first))内
	// input_iterator_tag 版本 ，使用move函数提高效率，变相的删除操作
	template <class InputIter, class OutputIter>
	OutputIter
		unchecked_move_cat(InputIter first, InputIter last, OutputIter result,
			mySTL::input_iterator_tag)
	{
		for (; first != last; ++first, ++result)
		{
			*result = mySTL::move(*first);
		}
		return result;
	}
	// ramdom_access_iterator_tag 特化版本
	template <class RandomIter, class OutputIter>
	OutputIter
		unchecked_move_cat(RandomIter first, RandomIter last, OutputIter result,
			mySTL::random_access_iterator_tag)
	{
		for (auto n = last - first; n > 0; --n, ++first, ++result)
		{
			*result = mySTL::move(*first);
		}
		return result;
	}
	//move 的模板函数,调用move_cat函数
	template <class InputIter, class OutputIter>
	OutputIter
		unchecked_move(InputIter first, InputIter last, OutputIter result)
	{
		return unchecked_move_cat(first, last, result, iterator_category(first));
	}
	// 为 trivially_copy_assignable 类型提供特化版本
	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
		std::is_trivially_move_assignable<Up>::value,
		Up*>::type
		unchecked_move(Tp* first, Tp* last, Up* result)
	{
		const size_t n = static_cast<size_t>(last - first);
		if (n != 0)
			std::memmove(result, first, n * sizeof(Up));
		return result + n;
	}

	template <class InputIter, class OutputIter>
	OutputIter move(InputIter first, InputIter last, OutputIter result)
	{
		return unchecked_move(first, last, result);
	}
	// move_backward
	// 将 [first, last)区间内的元素移动到 [result - (last - first), result)内

	// bidirectional_iterator_tag 版本
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result, mySTL::bidirectional_iterator_tag)
	{
		while (first != last)
			*--result = mySTL::move(*--last);
		return result;
	}

	// random_access_iterator_tag 版本
	template <class RandomIter1, class RandomIter2>
	RandomIter2
		unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
			RandomIter2 result, mySTL::random_access_iterator_tag)
	{
		for (auto n = last - first; n > 0; --n)
			*--result = mySTL::move(*--last);
		return result;
	}

	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result)
	{
		return unchecked_move_backward_cat(first, last, result, iterator_category(first));
	}
	// 为 trivially_copy_assignable 类型提供特化版本
	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
		std::is_trivially_move_assignable<Up>::value,
		Up*>::type
		unchecked_move_backward(Tp* first, Tp* last, Up* result)
	{
		const size_t n = static_cast<size_t>(last - first);
		if (n != 0)
		{
			result -= n;
			std::memmove(result, first, n * sizeof(Up));
		}
		return result;
	}
	//move_backward 的函数模板
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
	{
		return unchecked_move_backward(first, last, result);
	}
	//************************************************************************
	// equal
	// 比较第一序列在 [first, last)区间上的元素值是否和第二序列相等
	template <class InputIter1, class InputIter2>
	bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2)
	{
		for (; first1 != last1; ++first1, ++first2)
		{
			if (*first1 != *first2)
				return false;
		}
		return true;
	}
	// 重载版本使用函数对象 comp 代替比较操作
	template <class InputIter1, class InputIter2, class Compared>
	bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp)
	{
		for (; first1 != last1; ++first1, ++first2)
		{
			if (!comp(*first1, *first2))
				return false;
		}
		return true;
	}
	//************************************************************************
	// fill_n
	// 从 first 位置开始填充 n 个值,返回结束位置的迭代器
	template <class OutputIter, class Size, class T>
	OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value)
	{
		for (; n > 0; --n, ++first)
		{
			*first = value;
		}
		return first;
	}
	// 为 one-byte 类型提供特化版本，更加高效
	template <class Tp, class Size, class Up>
	typename std::enable_if<
		std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
		std::is_integral<Up>::value && sizeof(Up) == 1 &&
		!std::is_same<Tp, bool>::value,
		Tp*>::type
		unchecked_fill_n(Tp* first, Size n, Up value)
	{
		if (n > 0)
		{
			std::memset(first, (unsigned char)value, (size_t)(n));
		}
		return first + n;
	}
	//fill_n 的函数模板
	template <class OutputIter, class Size, class T>
	OutputIter fill_n(OutputIter first, Size n, const T& value)
	{
		return unchecked_fill_n(first, n, value);
	}
	//************************************************************************
	// fill
	// 为 [first, last)区间内的所有元素填充新值
	template <class ForwardIter, class T>
	void fill_cat(ForwardIter first, ForwardIter last, const T& value,
		mySTL::forward_iterator_tag)
	{
		for (; first != last; ++first)
		{
			*first = value;
		}
	}
	//random_access_iterator_tag的特化版本
	template <class RandomIter, class T>
	void fill_cat(RandomIter first, RandomIter last, const T& value,
		mySTL::random_access_iterator_tag)
	{
		fill_n(first, last - first, value);
	}
	template <class ForwardIter, class T>
	void fill(ForwardIter first, ForwardIter last, const T& value)
	{
		fill_cat(first, last, value, iterator_category(first));
	}
	// 平行比较两个序列，找到第一处不等的元素，返回一对迭代器，分别指向两个序列中不等的元素
	template <class InputIter1, class InputIter2>
	mySTL::pair<InputIter1, InputIter2>
		mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2)
	{
		while (first1 != last1 && *first1 == *first2)
		{
			++first1;
			++first2;
		}
		return mySTL::pair<InputIter1, InputIter2>(first1, first2);
	}
	// 重载版本使用函数对象 comp 代替比较操作
	template <class InputIter1, class InputIter2, class Compred>
	mySTL::pair<InputIter1, InputIter2>
		mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compred comp)
	{
		while (first1 != last1 && comp(*first1, *first2))
		{
			++first1;
			++first2;
		}
		return mySTL::pair<InputIter1, InputIter2>(first1, first2);
	}


























}
#endif // !PROJECT1_ALGOBASE_H
