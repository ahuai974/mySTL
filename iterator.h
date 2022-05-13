#ifndef PROJECT1_ITERATOR_H
#define	PROJECT1_ITERATOR_H
#include <cstddef>
#include "type_traits.h"
namespace mySTL
{	
	//���ֵ���������
	//���������͵ļ̳й�ϵ��������������͵��Ƶ�
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag:public input_iterator_tag{};
	struct bidirectional_iterator_tag :public forward_iterator_tag {};
	struct radom_access_iterator_tag :public bidirectional_iterator_tag{};

	//������ģ�� 
	template<class Category, class T, class Distance = ptrdiff_t,class Point = T*,class Reference = T&>
	struct iterator
	{
		typedef Category                             iterator_category;
		typedef T                                    value_type;
		typedef Pointer                              pointer;
		typedef Reference                            reference;
		typedef Distance                             difference_type;
	};
	//������traits
	template<class I>
	struct iterator_traits {
		typedef typename I::iterator_category	iterator_category;
		typedef typename I::value_type		    value_type;
		typedef typename I::pointer				pointer;
		typedef typename I::reference			reference;
		typedef typename I::difference_type		difference_type;
	};
	//implement �ӿ�
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
	//���ֵ���������
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag :public input_iterator_tag {};
	struct bidirectional_iterator_tag :public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	//�ж������Ƿ���category���Բ���ֵhas_iterator_cat::value��ʾ
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

	//std::is_convertible�жϵ�һ�����Ϳɷ�ת��Ϊ�ڶ�������
	template <class Iterator>
	struct iterator_traits_helper<Iterator, true>
		: public iterator_traits_impl<Iterator, std::is_convertible<
		typename Iterator::iterator_category, input_iterator_tag>::value ||
		std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value> {};
	//��ȡ������������
	template<class Iterator>
	struct iterator_traits
		:public iterator_traits_helper<Iterator,has_iterator_cat<Iterator>::value> {};
	//���ԭ��ָ����ػ��汾(ԭ��ָ��û��value type����)
	template<T>
	struct iterator_traits<T*>
	{
		typedef random_access_iterator_tag	iterator_category;//ԭ��ָ����������������
		typedef T							value_type;
		typedef T*							pointer;
		typedef T&							reference;
		typedef ptrdiff_t					difference_type;
	};
	//���ԭ��ָ��ָ��const���󣨷���һ�������޸ĵ�const������û������ģ�
	template<T>
	struct iterator_traits<const T*>
	{
		typedef random_access_iterator_tag	iterator_category;//ԭ��ָ����������������
		typedef T							value_type;
		typedef const T*					pointer;
		typedef const T&					reference;
		typedef ptrdiff_t					difference_type;
	};

	//û��
	template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
	struct has_iterator_cat_of
		: public m_bool_constant<std::is_convertible<
		typename iterator_traits<T>::iterator_category, U>::value> {};

	// ��ȡĳ�ֵ�����
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
	//ͨ�����ж��Ƿ�Ϊiterator_cagetory���ж��Ƿ�Ϊ������
	template <class Iterator>
	struct is_iterator :
		public m_bool_constant<is_input_iterator<Iterator>::value ||
		is_output_iterator<Iterator>::value> {};

	//��ȡĳ����������category
	template<class Iterator>
	typename iterator_traits<Iterator>::iterator_category//����ֵ����
	inline iterator_category(const Iterator&)
	{
		typedef typename iterator_traits<Iterator>::iterator_category Category;
		return Category();
	}
	// ��ȡĳ���������� value_type
	template <class Iterator>
	typename iterator_traits<Iterator>::value_type*
		value_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);//��̬����ת����һ������ö�������ֳ���
	}
	// ��ȡĳ���������� distance_type
	template <class Iterator>
	typename iterator_traits<Iterator>::difference_type*
		distance_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}

	//���������֮��ľ���
	//���input_iterator�İ汾
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
	//distance ��random access �汾
	template<class RandomIterator>
	typename iterator_traits<RandomIterator>::difference_type
		distance_dispatch(RandomIterator first, RandomIterator last, random_access_iterator_tag)
	{
		return last-first;
	}
	//distance �ķ���ģ��
	template <class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		distance(InputIterator first, InputIterator last)
	{
		return distance_dispatch(first, last, iterator_category(first));
	}
	//������ǰ��n������
	//advance��input_iterator�ػ��汾
	template<class InputIterator,class Distance>
	void advance_dispatch(InputIterator& pos, Distance n, input_iterator_tag)
	{
		while (n--)
		{
			++pos;
		}
	}
	//advance�� random_access_iterator���ػ��汾
	template<class InputIterator, class Distance>
	void advance_dispatch(InputIterator& pos, Distance n, input_iterator_tag)
	{
		pos += n;
	}
	//�������advance_dispatch
	template <class InputIterator, class Distance>
	void advance(InputIterator& i, Distance n)
	{
		advance_dispatch(i, n, iterator_category(i));
	}

	// ģ���� : reverse_iterator
	// �������������ʹǰ��Ϊ���ˣ�����Ϊǰ��
	template<class Iterator>
	class reverse_iterator
	{
	private:
		Iterator current;  // ��¼��Ӧ�����������

	public:
		// �����������������Ӧ�ͱ�
		typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
		typedef typename iterator_traits<Iterator>::value_type        value_type;
		typedef typename iterator_traits<Iterator>::difference_type   difference_type;
		typedef typename iterator_traits<Iterator>::pointer           pointer;
		typedef typename iterator_traits<Iterator>::reference         reference;

		typedef Iterator                                              iterator_type;
		typedef reverse_iterator<Iterator>                            self;
	public:
		//���캯��
		reverse_iterator() {}//Ĭ�Ϲ���
		explicit reverse_iterator(iterator_type i) : current(i) {}//��ֵ����
		reverse_iterator(const self& rhs) :current(rhs.current) {}//��������
	public:
		//ȡ����Ӧ�ĵ�����
		iterator_type base() const//���ı��Ա
		{
			return current;
		}
		//���ز�����
		reference operator*() const
		{	
			
			auto tmp = current;
			return *--tmp;//ʵ�ʶ�Ӧ�����������ǰһ��λ��
		}
		pointer operator->() const
		{
			return &(operator*());
		}
		//++��Ϊ--��ǰ++�汾
		self& operator++()
		{
			--current;
			return *this;
		}
		//��++�汾����int��Ϊϰ��
		self& operator++(int)
		{	
			self tmp = *this;
			--current;
			return tmp;
		}
		//--��Ϊ++��ǰ--�汾
		self& operator--()
		{
			++current;
			return *this;
		}
		//--��Ϊ++����--�汾
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
	// ���� operator-
	template <class Iterator>
	typename reverse_iterator<Iterator>::difference_type
		operator-(const reverse_iterator<Iterator>& lhs,
			const reverse_iterator<Iterator>& rhs)
	{
		return rhs.base() - lhs.base();
	}

	// ���رȽϲ�����
	//ʵ����ֻ��д�� ==��<�����Ĳ���������ʹ��==��<���
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
