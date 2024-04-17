#pragma once



#include"MyVector.h"
#include"MyList.h"
#include<initializer_list>
#include<vector>
#include<algorithm>

//����һ��
template<class Type, class Cmp>
struct ListQueue;

template<class Type, class Ref, class Ptr, class Cmp>
struct __list_queue_iterator
{
public:
	//Ϊ��ͨ��STL������������ͼ�� -- �����⼸�仰
	//�������ǾͿ��԰�ListQueueҲ����stl�Ĳ��������㷨����
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
	//���ƺ��ĵ�������Ϊ
	//����!=/==
	bool operator!=(const __list_queue_iterator& lqit)
	{
		return __ptr != lqit.__ptr || __pos != lqit.__pos;
	}
	bool operator==(const __list_queue_iterator& lqit)
	{
		return __ptr == lqit.__ptr && __pos == lqit.__pos;
	}
	//���ؽ�����
	Ref operator*()
	{
		return __ptr->operator[](__pos);
	}
	Ptr operator->()
	{
		return &(operator*());
	}
	//����ǰ��++/--
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
	//���غ���++/--
	__list_queue_iterator operator++(int)
	{
		//�ȱ���һ��֮ǰ��ֵ
		__list_queue_iterator tmp(*this);
		__pos++;
		return tmp;
	}
	__list_queue_iterator operator--(int)
	{
		//�ȱ���һ��֮ǰ��ֵ
		__list_queue_iterator tmp(*this);
		__pos--;
		return tmp;
	}
	//����+/-
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


//��װListQueue��
#define VECTOR_MAX_CAPACITY 3 //�涨ÿ��vectorֻ�ܴ� VECTOR_MAX_CAPACITY ��Ԫ��
template<class Type = int, class Cmp = less<Type>>
struct ListQueue
{
private:
	Mylist<MyVector<Type>> __inner_list;
	size_t __nodes_count = 0;
	size_t __size = 0;
public:
	//���������?
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
	ListQueue() = default;//�Զ������ͻ�ȥ�����Լ��Ĺ��캯�������������Լ�����
	ListQueue(const ListQueue& lq)
	{
		__nodes_count = lq.__nodes_count;
		__size = lq.__size;
		__inner_list = lq.__inner_list;
	}
	~ListQueue() = default;//�Զ������ͻ�ȥ�����Լ����������������������Լ�����
public:
	//֧�ֳ�ʼ���б����͵Ĺ���
	ListQueue(const initializer_list<Type>& list)
	{
		for (const auto& e : list)
		{
			this->push_back(e);
		}
	}
	//֧�ֵ���������Ĺ���?
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
	//β��
	void push_back(const Type& x)
	{
		if (__nodes_count == 0 || __inner_list[__nodes_count - 1].size() == VECTOR_MAX_CAPACITY)
		{
			//��Ҫ�����һ���ڵ�����? -- һ��ʼû�нڵ�/���һ���ڵ��vector����
			__nodes_count++;
			__inner_list.push_back(MyVector<Type>());
			//__inner_list[__nodes_count - 1];//��������һ���ڵ�
			__inner_list[__nodes_count - 1].push_back(x);
			__size++;
		}
		else
		{
			__inner_list[__nodes_count - 1].push_back(x);
			__size++;
		}
	}
	//βɾ
	void pop_back()
	{
		assert(__nodes_count);
		assert(__size);
		assert(__inner_list[__nodes_count - 1].size() != 0);//�����ܳ��ֿսڵ�����
		__size--;
		__inner_list[__nodes_count - 1].pop_back();//���һ��vector��pop
		//����pop֮�������Ҫɾ���ڵ�?
		if (__inner_list[__nodes_count - 1].size() == 0)
		{
			//��ʱ��Ҫɾ���������һ���ڵ�?
			__inner_list.pop_back();
			__nodes_count--;
		}
	}
	//ͷ��
	void push_front(const Type& x)
	{
		//����insert
		this->insert(0, x);
	}
	//ͷɾ
	void pop_front()
	{
		this->erase(0);
	}
	//����λ�ò���
	void insert(size_t pos, const Type& x)
	{
		if (pos == __size)
		{
			this->push_back(x);
			return;
		}
		assert(pos >= 0 && pos < __size);
		//���±�Ϊpos��Ԫ��֮ǰ����
		//���ҵ�pos��������һ��vector��
		size_t cur_sum = 0;
		size_t i = 0;
		for (i = 0; i < __inner_list.size(); i++)
		{
			if (cur_sum + __inner_list[i].size() > pos)break;
			cur_sum += __inner_list[i].size();
		}
		//��ʱ��i�������ҵ�Ҫ�����vector��λ��
		if (__inner_list[i].size() == VECTOR_MAX_CAPACITY)
		{
			//��ʱҪ�����һ���ڵ�?(����ϸ���?)
			size_t res_idx = pos - cur_sum;//ʣ�µ��±�
			__inner_list[i].insert(__inner_list[i].begin() + res_idx, x);
			__size++;
			//��ʱ��λ��i�µ�vectorӦ���Ѿ������޶�������
			//���iλ�ú����vector���� -- �����½ڵ㣬���û�������뵽�����vector��ȥ
			//���iλ�ú���û��vector�� -- �����½ڵ�
			if (i == __inner_list.size() - 1 || __inner_list[i + 1].size() == VECTOR_MAX_CAPACITY)
			{
				//��iλ�õĺ�������һ�������Ľڵ� -- ��i�±��vector�����һ��Ԫ�طŹ��?
				__inner_list.insert(__inner_list.begin() + i + 1, MyVector<Type>());
				__nodes_count++;
				__inner_list[i + 1].push_back(__inner_list[i][VECTOR_MAX_CAPACITY]);
				__inner_list[i].pop_back();
			}
			else
			{
				//ͷ�嵽�����vector��ȥ
				__inner_list[i + 1].insert(__inner_list[i + 1].begin(),
					__inner_list[i][VECTOR_MAX_CAPACITY]);//�ѳ������Ǹ�vector�����һ��Ԫ��ͷ�嵽�����vector��ȥ
				__inner_list[i].pop_back();//ͬ�����������Ǹ�vector popһ��
			}
		}
		else
		{
			size_t res_idx = pos - cur_sum;//ʣ�µ��±�
			__inner_list[i].insert(__inner_list[i].begin() + res_idx, x);
			__size++;
		}
	}
	//����λ��ɾ��
	void erase(size_t pos)
	{
		assert(pos >= 0 && pos < __size);
		__size--;
		//ɾ���±�Ϊposλ�õ�ֵ
		//���ҵ�pos��������һ��vector��
		size_t cur_sum = 0;
		size_t i = 0;
		for (i = 0; i < __inner_list.size(); i++)
		{
			if (cur_sum + __inner_list[i].size() > pos)break;
			cur_sum += __inner_list[i].size();
		}
		//��ʱ��i�������ҵ�Ҫɾ��vector��λ��
		//��ʱ�������±�Ϊi��λ�õ�vector��size��Ӧ��Ϊ0������?0��˵��ǰ����߼���������?
		assert(__inner_list[i].size());
		size_t res_idx = pos - cur_sum;//��vector�У�Ŀ��Ԫ�ص��±�
		__inner_list[i].erase(__inner_list[i].begin() + res_idx);
		//��ʱ����±�Ϊi��vector��ɾ���� -- ��ʱ��Ҫɾ��һ�������Ľڵ�
		if (__inner_list[i].size() == 0)
		{
			__inner_list.erase(__inner_list.begin() + i);
			__nodes_count--;
		}
	}
	//[]����
	Type& operator[](size_t pos)
	{
		assert(pos >= 0 && pos < __size);
		//�ҵ��±�Ϊposλ�õ�ֵ
		//���ҵ�pos��������һ��vector��
		size_t cur_sum = 0;
		size_t i = 0;
		for (i = 0; i < __inner_list.size(); i++)
		{
			if (cur_sum + __inner_list[i].size() > pos)break;
			cur_sum += __inner_list[i].size();
		}
		size_t res_idx = pos - cur_sum;//��vector�У�Ŀ��Ԫ�ص��±�
		return __inner_list[i][res_idx];
	}
	//���� -- �����Ǳ����? -- �������eraseʹ�� -- ɾ����һ�γ��ֵ�x
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
	//����
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
	//����
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
	void print() // չʾ�ڲ���Ϣ
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
	//�����ýӿ� -- ���Բ���װ���֮��m����Ϊ˽��
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