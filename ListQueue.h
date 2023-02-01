#pragma once



/*希望老师在批改之前可以看一下代码的文档 -- README.md -- 谢谢！*/
/*代码已按照要求实现了题目中所有功能，详情请见代码的文档*/


#include"MyVector.h"
#include"MyList.h"
#include<initializer_list>
#include<vector>
#include<algorithm>

//声明一下
template<class Type, class Cmp>
struct ListQueue;

template<class Type, class Ref, class Ptr, class Cmp>
struct __list_queue_iterator
{
public:
	//为了通过STL其它组件的类型检查 -- 加上这几句话
	//这样我们就可以把ListQueue也用在stl的部分其它算法上了
	typedef bidirectional_iterator_tag iterator_category;
	typedef Type value_type;
	typedef Ptr pointer;
	typedef Ref reference;
	typedef ptrdiff_t difference_type;
private:
	ListQueue<Type, Cmp>* __ptr;
	size_t __pos;
public:
	__list_queue_iterator(ListQueue<Type, Cmp>* ptr, size_t pos)
		:__ptr(ptr), __pos(pos) {}
public:
	//控制核心迭代器行为
	//重载!=/==
	bool operator!=(const __list_queue_iterator& lqit)
	{
		return __ptr != lqit.__ptr || __pos != lqit.__pos;
	}
	bool operator==(const __list_queue_iterator& lqit)
	{
		return __ptr == lqit.__ptr && __pos == lqit.__pos;
	}
	//重载解引用
	Ref operator*()
	{
		return __ptr->operator[](__pos);
	}
	Ptr operator->()
	{
		return &(operator*());
	}
	//重载前置++/--
	__list_queue_iterator& operator++()
	{
		__pos++;
		return *this;
	}
	__list_queue_iterator& operator--()
	{
		__pos--;
		return *this;
	}
	//重载后置++/--
	__list_queue_iterator operator++(int)
	{
		//先保存一下之前的值
		__list_queue_iterator tmp(*this);
		__pos++;
		return tmp;
	}
	__list_queue_iterator operator--(int)
	{
		//先保存一下之前的值
		__list_queue_iterator tmp(*this);
		__pos--;
		return tmp;
	}
	//重载+/-
	__list_queue_iterator operator+(size_t n)
	{
		__list_queue_iterator tmp(*this);
		tmp.__pos += n;
		return tmp;
	}
	__list_queue_iterator operator-(size_t n)
	{
		__list_queue_iterator tmp(*this);
		tmp.__pos -= n;
		return tmp;
	}
};


//封装ListQueue类
#define VECTOR_MAX_CAPACITY 3 //规定每个vector只能存 VECTOR_MAX_CAPACITY 个元素
template<class Type = int, class Cmp = less<Type>>
struct ListQueue
{
private:
	Mylist<MyVector<Type>> __inner_list;
	size_t __nodes_count = 0;
	size_t __size = 0;
public:
	//定义迭代器
	typedef __list_queue_iterator<Type, Type&, Type*, Cmp> iterator;
	typedef __list_queue_iterator<Type, const Type&, const Type*, Cmp> const_iterator;
	iterator begin()
	{
		return iterator(this, 0);
	}
	iterator end()
	{
		return iterator(this, __size);
	}
	const_iterator begin() const
	{
		return const_iterator(this, 0);
	}
	const_iterator end() const
	{
		return const_iterator(this, __size);
	}
public:
	ListQueue() = default;//自定义类型会去调用自己的构造函数，不用我们自己定义
	ListQueue(const ListQueue& lq)
	{
		__nodes_count = lq.__nodes_count;
		__size = lq.__size;
		__inner_list = lq.__inner_list;
	}
	~ListQueue() = default;//自定义类型会去调用自己的析构函数，不用我们自己定义
public:
	//支持初始化列表类型的构造
	ListQueue(const initializer_list<Type>& list)
	{
		for (const auto& e : list)
		{
			this->push_back(e);
		}
	}
	//支持迭代器区间的构造
	template<class InputIterator>
	ListQueue(InputIterator begin, InputIterator end)
	{
		while (begin != end)
		{
			this->push_back(*begin);
			begin++;
		}
	}
public:
	//尾插
	void push_back(const Type& x)
	{
		if (__nodes_count == 0 || __inner_list[__nodes_count - 1].size() == VECTOR_MAX_CAPACITY)
		{
			//需要多插入一个节点的情况 -- 一开始没有节点/最后一个节点的vector满了
			__nodes_count++;
			__inner_list.push_back(MyVector<Type>());
			//__inner_list[__nodes_count - 1];//这个是最后一个节点
			__inner_list[__nodes_count - 1].push_back(x);
			__size++;
		}
		else
		{
			__inner_list[__nodes_count - 1].push_back(x);
			__size++;
		}
	}
	//尾删
	void pop_back()
	{
		assert(__nodes_count);
		assert(__size);
		assert(__inner_list[__nodes_count - 1].size() != 0);//不可能出现空节点的情况
		__size--;
		__inner_list[__nodes_count - 1].pop_back();//最后一个vector作pop
		//做完pop之后可能需要删除节点
		if (__inner_list[__nodes_count - 1].size() == 0)
		{
			//此时需要删除链表最后一个节点
			__inner_list.pop_back();
			__nodes_count--;
		}
	}
	//头插
	void push_front(const Type& x)
	{
		//复用insert
		this->insert(0, x);
	}
	//头删
	void pop_front()
	{
		this->erase(0);
	}
	//任意位置插入
	void insert(size_t pos, const Type& x)
	{
		if (pos == __size)
		{
			this->push_back(x);
			return;
		}
		assert(pos >= 0 && pos < __size);
		//在下标为pos的元素之前插入
		//先找到pos是属于哪一个vector的
		size_t cur_sum = 0;
		size_t i = 0;
		for (i = 0; i < __inner_list.size(); i++)
		{
			if (cur_sum + __inner_list[i].size() > pos)break;
			cur_sum += __inner_list[i].size();
		}
		//此时的i就是所找的要插入的vector的位置
		if (__inner_list[i].size() == VECTOR_MAX_CAPACITY)
		{
			//此时要多插入一个节点(情况较复杂)
			size_t res_idx = pos - cur_sum;//剩下的下标
			__inner_list[i].insert(__inner_list[i].begin() + res_idx, x);
			__size++;
			//此时，位置i下的vector应该已经超过限定容量了
			//如果i位置后面的vector满了 -- 开辟新节点，如果没满，插入到后面的vector中去
			//如果i位置后面没有vector了 -- 开辟新节点
			if (i == __inner_list.size() - 1 || __inner_list[i + 1].size() == VECTOR_MAX_CAPACITY)
			{
				//在i位置的后面多插入一个链表的节点 -- 把i下标的vector的最后一个元素放过去
				__inner_list.insert(__inner_list.begin() + i + 1, MyVector<Type>());
				__nodes_count++;
				__inner_list[i + 1].push_back(__inner_list[i][VECTOR_MAX_CAPACITY]);
				__inner_list[i].pop_back();
			}
			else
			{
				//头插到后面的vector中去
				__inner_list[i + 1].insert(__inner_list[i + 1].begin(),
					__inner_list[i][VECTOR_MAX_CAPACITY]);//把超出的那个vector的最后一个元素头插到后面的vector中去
				__inner_list[i].pop_back();//同样，超出的那个vector pop一下
			}
		}
		else
		{
			size_t res_idx = pos - cur_sum;//剩下的下标
			__inner_list[i].insert(__inner_list[i].begin() + res_idx, x);
			__size++;
		}
	}
	//任意位置删除
	void erase(size_t pos)
	{
		assert(pos >= 0 && pos < __size);
		__size--;
		//删除下标为pos位置的值
		//先找到pos是属于哪一个vector的
		size_t cur_sum = 0;
		size_t i = 0;
		for (i = 0; i < __inner_list.size(); i++)
		{
			if (cur_sum + __inner_list[i].size() > pos)break;
			cur_sum += __inner_list[i].size();
		}
		//此时的i就是所找的要删除vector的位置
		//此时链表中下标为i的位置的vector的size不应该为0，如果为0，说明前面的逻辑出现问题
		assert(__inner_list[i].size());
		size_t res_idx = pos - cur_sum;//在vector中，目标元素的下标
		__inner_list[i].erase(__inner_list[i].begin() + res_idx);
		//此时如果下标为i的vector被删空了 -- 此时需要删除一个链表的节点
		if (__inner_list[i].size() == 0)
		{
			__inner_list.erase(__inner_list.begin() + i);
			__nodes_count--;
		}
	}
	//[]重载
	Type& operator[](size_t pos)
	{
		assert(pos >= 0 && pos < __size);
		//找到下标为pos位置的值
		//先找到pos是属于哪一个vector的
		size_t cur_sum = 0;
		size_t i = 0;
		for (i = 0; i < __inner_list.size(); i++)
		{
			if (cur_sum + __inner_list[i].size() > pos)break;
			cur_sum += __inner_list[i].size();
		}
		size_t res_idx = pos - cur_sum;//在vector中，目标元素的下标
		return __inner_list[i][res_idx];
	}
	//查找 -- 查找是必须的 -- 可以配合erase使用 -- 删除第一次出现的x
	size_t find(const Type& x)
	{
		for (size_t i = 0; i < __size; i++)
		{
			if (this->operator[](i) == x)
			{
				return i;
			}
		}
	}
	//排序
	void sort()
	{
		vector<Type>arr;
		for (size_t i = 0; i < __size; i++)
		{
			arr.push_back(this->operator[](i));
		}
		std::sort(arr.begin(), arr.end(), Cmp());
		this->clear();
		for (const auto& e : arr)
		{
			this->push_back(e);
		}
	}
	//清理
	void clear()
	{
		__inner_list.clear();
		__nodes_count = __size = 0;
	}
public:
	size_t size()
	{
		return __size;
	}
public:
	void print() // 展示内部信息
	{
		cout << "curent list info: ";
		this->output(); cout << endl;
		cout << "curent detail info: " << endl;
		for (size_t i = 0; i < __inner_list.size(); i++)
		{
			cout << "Nodes[" << i << "] " << "Size: " << __inner_list[i].size() << " --> ";
			for (size_t j = 0; j < __inner_list[i].size(); j++)
			{
				cout << __inner_list[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl << "other info: " << endl;
		cout << "NodesNumber: " << __inner_list.size() << endl;
		cout << "Element Size: " << __size << endl;
	}
public:
	//测试用接口 -- 测试并封装完成之后m可设为私有
	void output()
	{
		for (size_t i = 0; i < __inner_list.size(); i++)
		{
			for (size_t j = 0; j < __inner_list[i].size(); j++)
			{
				cout << __inner_list[i][j] << " ";
			}
		}
		cout << endl;
	}
	void output_detail()
	{
		for (size_t i = 0; i < __inner_list.size(); i++)
		{
			for (size_t j = 0; j < __inner_list[i].size(); j++)
			{
				cout << __inner_list[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
};