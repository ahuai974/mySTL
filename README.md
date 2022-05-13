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
空间配置器（）：
```
负责空间配置与管理，从实现的角度，配置器是一个实现了动态空间配置、空间管理、空间释放的class template
```
```

```
```

```
```

```
```

```
```

```
```

```
```

```
```

```
```

```
```
