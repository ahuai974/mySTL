#ifndef PROJECT1_ITERATOR_H
#define	PROJECT1_ITERATOR_H
#include <cstddef>
#include "type_traits.h"
namespace mySTL
{	
	//五种迭代器类型
	//迭代器类型的继承关系，方便迭代器类型的推导
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag:public input_iterator_tag{};
	struct bidirectional_iterator_tag :public forward_iterator_tag {};
	struct radom_access_iterator_tag :public bidirectional_iterator_tag{};

	//迭代器模板 
	template<class Category, class T, class Distance = ptrdiff_t,class Point = T*,class Reference = T&>
	struct iterator
	{
		typedef Category                             iterator_category;
		typedef T                                    value_type;
		typedef Pointer                              pointer;
		typedef Reference                            reference;
		typedef Distance                             difference_type;
	};
	//迭代器traits
	template<class I>
	struct iterator_traits {
		typedef typename I::iterator_category	iterator_category;
		typedef typename I::value_type		    value_type;
		typedef typename I::pointer				pointer;
		typedef typename I::reference			reference;
		typedef typename I::difference_type		difference_type;
	};
	//implement 接口
	template<class Iterator,bool>
	struct iterator_traits_impl{};

	template<class Iterator>
	struct iterator_traits_impl<Iterator,true>
	{
		typedef typename Iterator::iterator_category	iterator_category;
		typedef typename Iterator::value_type			value_type;
		typedef typename Iterator::pointer				pointer;
		typedef typename Iterator::reference			reference;
		typedef typename Iterator::difference_type		difference_type;
	};
	//五种迭代器类型
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag :public input_iterator_tag {};
	struct bidirectional_iterator_tag :public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	//判断类型是否含有category，以布尔值has_iterator_cat::value表示
	template <class T>
	struct has_iterator_cat
	{
	private:
		struct two { char a; char b; };
		template <class U> static two test(...) {};
		template <class U> static char test(typename U::iterator_category* = 0) {};
	public:
		static const bool value = sizeof(test<T>(0)) == sizeof(char);
	};

	template <class Iterator, bool>
	struct iterator_traits_helper {};

	//std::is_convertible判断第一个类型可否转换为第二个类型
	template <class Iterator>
	struct iterator_traits_helper<Iterator, true>
		: public iterator_traits_impl<Iterator, std::is_convertible<
		typename Iterator::iterator_category, input_iterator_tag>::value ||
		std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value> {};
	//萃取迭代器的特性
	template<class Iterator>
	struct iterator_traits
		:public iterator_traits_helper<Iterator,has_iterator_cat<Iterator>::value> {};
	//针对原生指针的特化版本(原生指针没有value type属性)
	template<T>
	struct iterator_traits<T*>
	{
		typedef random_access_iterator_tag	iterator_category;//原生指针具有随机访问特性
		typedef T							value_type;
		typedef T*							pointer;
		typedef T&							reference;
		typedef ptrdiff_t					difference_type;
	};
	//针对原生指针指向const对象（返回一个不能修改的const类型是没有意义的）
	template<T>
	struct iterator_traits<const T*>
	{
		typedef random_access_iterator_tag	iterator_category;//原生指针具有随机访问特性
		typedef T							value_type;
		typedef const T*					pointer;
		typedef const T&					reference;
		typedef ptrdiff_t					difference_type;
	};

	//没懂
	template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
	struct has_iterator_cat_of
		: public m_bool_constant<std::is_convertible<
		typename iterator_traits<T>::iterator_category, U>::value> {};

	// 萃取某种迭代器
	template <class T, class U>
	struct has_iterator_cat_of<T, U, false> : public m_false_type {};

	template <class Iter>
	struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

	template <class Iter>
	struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

	template <class Iter>
	struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

	template <class Iter>
	struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

	template <class Iter>
	struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};
	//通过（判断是否为iterator_cagetory）判断是否为迭代器
	template <class Iterator>
	struct is_iterator :
		public m_bool_constant<is_input_iterator<Iterator>::value ||
		is_output_iterator<Iterator>::value> {};

	//萃取某个迭代器的category
	template<class Iterator>
	typename iterator_traits<Iterator>::iterator_category//返回值类型
	inline iterator_category(const Iterator&)
	{
		typedef typename iterator_traits<Iterator>::iterator_category Category;
		return Category();
	}
	// 萃取某个迭代器的 value_type
	template <class Iterator>
	typename iterator_traits<Iterator>::value_type*
		value_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);//静态类型转换，一般用于枚举型这种常量
	}
	// 萃取某个迭代器的 distance_type
	template <class Iterator>
	typename iterator_traits<Iterator>::difference_type*
		distance_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}

	//计算迭代器之间的距离
	//针对input_iterator的版本
	template<class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag)
	{
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (first != last)
		{
			++first;
			++n;
		}
		return n;
	}
	//distance 的random access 版本
	template<class RandomIterator>
	typename iterator_traits<RandomIterator>::difference_type
		distance_dispatch(RandomIterator first, RandomIterator last, random_access_iterator_tag)
	{
		return last-first;
	}
	//distance 的泛化模板
	template <class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		distance(InputIterator first, InputIterator last)
	{
		return distance_dispatch(first, last, iterator_category(first));
	}
	//迭代器前进n个距离
	//advance的input_iterator特化版本
	template<class InputIterator,class Distance>
	void advance_dispatch(InputIterator& pos, Distance n, input_iterator_tag)
	{
		while (n--)
		{
			++pos;
		}
	}
	//advance的 random_access_iterator的特化版本
	template<class InputIterator, class Distance>
	void advance_dispatch(InputIterator& pos, Distance n, input_iterator_tag)
	{
		pos += n;
	}
	//这里调用advance_dispatch
	template <class InputIterator, class Distance>
	void advance(InputIterator& i, Distance n)
	{
		advance_dispatch(i, n, iterator_category(i));
	}

	// 模板类 : reverse_iterator
	// 代表反向迭代器，使前进为后退，后退为前进
	template<class Iterator>
	class reverse_iterator
	{
	private:
		Iterator current;  // 记录对应的正向迭代器

	public:
		// 反向迭代器的五种相应型别
		typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
		typedef typename iterator_traits<Iterator>::value_type        value_type;
		typedef typename iterator_traits<Iterator>::difference_type   difference_type;
		typedef typename iterator_traits<Iterator>::pointer           pointer;
		typedef typename iterator_traits<Iterator>::reference         reference;

		typedef Iterator                                              iterator_type;
		typedef reverse_iterator<Iterator>                            self;
	public:
		//构造函数
		reverse_iterator() {}//默认构造
		explicit reverse_iterator(iterator_type i) : current(i) {}//赋值构造
		reverse_iterator(const self& rhs) :current(rhs.current) {}//拷贝构造
	public:
		//取出对应的迭代器
		iterator_type base() const//不改变成员
		{
			return current;
		}
		//重载操作符
		reference operator*() const
		{	
			
			auto tmp = current;
			return *--tmp;//实际对应正向迭代器的前一个位置
		}
		pointer operator->() const
		{
			return &(operator*());
		}
		//++变为--，前++版本
		self& operator++()
		{
			--current;
			return *this;
		}
		//后++版本，（int）为习惯
		self& operator++(int)
		{	
			self tmp = *this;
			--current;
			return tmp;
		}
		//--变为++，前--版本
		self& operator--()
		{
			++current;
			return *this;
		}
		//--变为++，后--版本
		self& operator--(int)
		{
			self tmp = *this;
			++current;
			return tmp;
		}
		self& operator+=(difference_type n)
		{
			current -=n;
			return *this;
		}
		self operator+(difference_type n) const
		{
			return self(current - n);
		}
		self& operator-=(difference_type n)
		{
			current += n;
			return *this;
		}
		self operator-(difference_type n) const
		{
			return self(current + n);
		}
		reference operator[](difference_type n) const
		{
			return *(*this + n);
		}

	};
	// 重载 operator-
	template <class Iterator>
	typename reverse_iterator<Iterator>::difference_type
		operator-(const reverse_iterator<Iterator>& lhs,
			const reverse_iterator<Iterator>& rhs)
	{
		return rhs.base() - lhs.base();
	}

	// 重载比较操作符
	//实际上只重写了 ==、<其他的操作符可以使用==与<完成
	template <class Iterator>
	bool operator==(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return lhs.base() == rhs.base();
	}

	template <class Iterator>
	bool operator<(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return rhs.base() < lhs.base();
	}

	template <class Iterator>
	bool operator!=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class Iterator>
	bool operator>(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return rhs < lhs;
	}

	template <class Iterator>
	bool operator<=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return !(rhs < lhs);
	}

	template <class Iterator>
	bool operator>=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return !(lhs < rhs);
	}

}
#endif // !PROJECT1_ITERATOR_H
