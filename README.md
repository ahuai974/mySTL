# mySTL
此项目为学习《STL源码剖析》、github项目《myTinySTL》的实践，加深对STL与元模板编程的理解，原项目地址https://github.com/Alinshans/MyTinySTL
```
容器（Containers）：
```
各种数据结构，如vector，list，deque，set，map用来存放数据，从实现的角度来看，STL容器时一种class template
```
算法（algorithms）：
```
各种常用算法如sort，search，copy，erase。从实现的角度来看，STL算法是一种 function template
```
迭代器（iterators）：
```
扮演容器和算法之间的胶合剂，从实现的角度看，迭代器是一种将operator*,operator->,operator++,operator--等指针相关操作予以重载的 class template。所有STL容器都附带有自己专属的迭代器，只有容器的设计者才知道如何遍历自己的元素，原生指针也是一种迭代器。
```
仿函数（functor）：
```
行为类似函数，可以作为算法的某种策略，从实现角度来看，仿函数是一种重载了operator（）的class或者是class 的template。一般函数指针可以视为
侠义的仿函数。
```
配接器（adapters）：
```
一种用来修饰容器或仿函数或迭代器接口的东西，STL中的queue和stack，虽然看似容器，其实只能算是一种容器配接器，因为他们的底部完全借助deque，所有操作由底层的deque供应。改变functor接口者，称为function adapter；改变container接口者，称为container adapter；改变iterator接口者，称为iterator adapter。
```
配置器（allocators）：
```
负责空间配置与管理，从实现的角度，配置器是一个实现了动态空间配置、空间管理、空间释放的class template
1.空间配置器
```
construct.h
```
对象的构造与析构
```
alloc.h
```
STL中的二级配置器，使用内存池申请空间，维护一个自由链表，实现小块内存的空间配置。当申请空间小于4096bytes时使用二级配置器，但这里只定义却没有使用它
```
alloator.h
```
对内存空间的分配与释放，使用简单的new与delete
```
uninitialized.h 
```
move、copy、fill函数的实现。依据对象是否可简单的复制分配，重载两个特化版本。
```
memory.h
```
2.迭代器
```
type_traits.h
```
萃取类别信息
```
iterator.h
```
定义了五种迭代器类型，分别是只读、只写、读写、双向、随机访问迭代器。继承关系从左至右
3.序列式容器
```
vector.h
```
vector采用线性连续空间的数据结构，它以两个迭代器start和finish分别指向配置得来的连续空间中目前已被使用的首和尾，并以迭代器cap_指向整块连续空间(含备用空间)的尾端。支持随机数据访问。vector和array不同，采用和静态空间不同的动态空间，书中所述SGI空间配置策略为空间满载时将空间扩充为二倍甚至更多，但此文件中分配空间大小由get_new_cap（）函数决定。
```
deque.h
```
deque和vector的最大差异，一在于deque允许常数时间内对起头端进行插入或移除操作，二在于deque没有所谓容量(capacity)概念，因为它是以分段连续空间组合而成，随时可以增加一段新的空间连接起来。 deque由一段一段连续空间组成，一旦有必要在deque的前端或尾端增加新空间，便配置一段连续空间，串接在整个deque 的前端或尾端。deque的最大任务，便是在这些分段的连续空间上，维护其整体连续的假象，并提供随机存取的接口，避开了“重新配置、复制、释放”的轮回，代价是复杂的迭代器结构。 每个缓冲区（buffer）的大小为4096，缓冲区的个数至少为8个，由四个迭代器表示deque，begin_指向第一个节点，end_ 指向最后一个节点，map_指向一块 map，map 中的每个元素都是一个指针，指向一个缓冲区，size_type map内指针的数目。
```
stack.h
```
stack是一种先进后出FILO的数据结构，它只有一个出口。stack允许增加元素、移除元素、取得最顶端元素。但除了最顶端外，没有任何其他方法可以存取，stack的其他元素，换言之，stack不允许有遍历行为。stack默认以deque为底层容器。
4.关联性容器
```
rb_tree.h
```
红黑树是平衡树的一种，保证最坏情况下操作时间复杂度为O(lgo(n))。红黑树的应用比较广泛，比如作为C++中STL的set和map的底层数据结构。 rb树必须满足以下规则: (1)每个节点是红色的，或者是黑色的 (2)根节点是黑色的 (3)如果一个节点是红色的，则它的两个子节点都是黑色的 (4)对每个节点，从该节点到其后代叶节点的简单路径上，均包含相同数目的黑色节点
```
map.h
```
map的所有元素都会根据元素的键值自动排序。map的所有元素都是pair，同时拥有实值(value)和键值(key)。pair的第一元素为键值，第二元素为实值。 map不允许有两个相同的键值。如果通过map的迭代器改变元素的键值，这样是不行的，因为map元素的键值关系到map元素的排列规则。任意改变map元素键值都会破坏map组织。如果修改元素的实值，这是可以的，因为map元素的实值不影响map元素的排列规则。因此，map iterator既不是一种constant iterators，也不是一种mutable iterators。 
```
set.h
```
set的所有元素都会根据元素的键值自动排序。set的元素不像map那样可以同时拥有实值(value)和键值(key)，set元素的键值就是实值，实值就是键值，set不允许有两个相同的元素。Set元素不能改变，在set源码中，set::iterator被定义为底层TB-tree的const_iterator，杜绝写入操作，也就是说，set iterator是一种constant iterators。
