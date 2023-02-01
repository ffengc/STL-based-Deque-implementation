#pragma once


/*希望老师在批改之前可以看一下代码的文档 -- README.md -- 谢谢！*/
/*代码已按照要求实现了题目中所有功能，详情请见代码的文档*/


#include<cassert>
#include<string.h>
#include<algorithm>
#include<functional>
#include<initializer_list>
#include<iostream>
using namespace std;
template<class T>
class MyVector {
public:
	typedef T* iterator;//这个要放成共有，不然迭代器外面访问不了 -- 定义迭代器类型是指针类型
	typedef const T* const_iterator;
	//vector的迭代器就是原生指针
	iterator begin() {
		return _start;
	}
	iterator end() {
		return _finish;
	}
	//如果不提供const迭代器 -- 如果传了const的vector是遍历不了的，因为权限不能放大
	const_iterator begin()const { //不能返回普通迭代器，要返回const迭代器
		return _start;
	}
	const_iterator end()const {
		return _finish;
	}
	MyVector() :_start(nullptr), _finish(nullptr), _end_of_storage(nullptr) {}
	~MyVector() {
		delete[] _start;
		_start = _finish = _end_of_storage = nullptr;
	}
	//用初始化列表初始化
	MyVector(const initializer_list<T>& list)
	{
		//用n个值去构造
		reserve(list.size());
		for (const auto& e : list)
		{
			push_back(e);
		}
	}
	//stl的vector还支持使用迭代器区间去构造
	//要支持用其它容器的迭代器的区间构造
	template<class InputIterator>
	MyVector(InputIterator first, InputIterator last)
		:_start(nullptr), _finish(nullptr), _end_of_storage(nullptr)
	{
		while (first != last) {
			//不能直接push_back()，直接push_back()会崩溃 -- 因为没有初始化 -- 野指针
			push_back(*first);
			++first;
		}
	}

	//debug记录：
	//这里的int n不能写成size_t 如果写成uint类型，当(10,5)这样的构造的时候
	//编译器会把10理解成int类型，从而下面这个构造不够匹配 -- 他会去走迭代器区间的构造 -- 造成崩溃
	//所以这里一定要写成int
	MyVector(int n, const T& val = T())
		:_start(nullptr), _finish(nullptr), _end_of_storage(nullptr)
	{
		//用n个值去构造
		reserve(n);
		for (size_t i = 0; i < n; i++) {
			push_back(val);
		}
	}
	//现代写法拷贝构造 -- 复用迭代器区间构造函数
	void swap(MyVector<T>& v) {
		std::swap(_start, v._start);
		std::swap(_finish, v._finish);
		std::swap(_end_of_storage, v._end_of_storage);
	}
#if 1
	MyVector(const MyVector<T>& v)
		:_start(nullptr), _finish(nullptr), _end_of_storage(nullptr)
	{
		MyVector<T>tmp(v.begin(), v.end());
		swap(tmp);//当然自定义类型自己写的swap更高效 -- 我们自己实现一个
	}
#endif
	MyVector<T>& operator=(MyVector<T> v) { //v1=v2
		//v是v2的拷贝，而且是局部的，swap完之后给到v1，v还会自动析构（因为是局部对象）
		swap(v);
		return *this;
	}
	size_t capacity()const {
		return _end_of_storage - _start;
	}
	size_t size()const {
		return _finish - _start;
	}
	void reserve(size_t n) {
		if (n > capacity()) {
			T* tmp = new T[n];
			size_t sz = size();
			if (_start) {
				//memcpy(tmp, _start, sizeof(T) * sz);//拷贝size()个字节过去，先放到tmp里面
				//同样，解决
				//vector<自定义类型>的问题
				for (size_t i = 0; i < sz; i++) {
					tmp[i] = _start[i];//当T是自定义类型时，调用T类型的operator=
				}
				delete[] _start;
			}
			_start = tmp;
			//_finish = _start + size();//这里不要现场去算size()
			//因为size()是用start减出来的，上面那一行start已经变了
			//所以在前面我们最好保留一下size先
			_finish = _start + sz;
			_end_of_storage = _start + n;
		}
	}
	void resize(size_t n, const T& val = T()) {
		//1.扩容+初始化
		//2.初始化
		//3.删除数据
		if (n > capacity()) {
			reserve(n);
		}
		if (n > size()) {
			//初始化填值
			while (_finish < _start+n) {
				*_finish = val;
				++_finish;
			}
		}
		else {
			_finish = _start + n;
		}
	}
	T& operator[](size_t pos) {
		assert(pos < size());
		return _start[pos];
	}
	const T& operator[](size_t pos) const {
		assert(pos < size());
		return _start[pos];
	}
	void push_back(const T& x) {
		//加了const保证传什么类型都行，因为隐式类型转换临时变量具有常性
		if (_finish == _end_of_storage) {
			reserve(capacity() == 0 ? 4 : capacity() * 2);
		}
		*_finish = x;
		++_finish;
	}
	void pop_back() {
		assert(_finish > _start);//为空是不能删的
		--_finish;
	}
	void insert(iterator pos, const T& x) {
		assert(pos >= _start);
		assert(pos <= _finish);
		if (_finish == _end_of_storage) {//扩容
			//记住pos和start的相对位置
			size_t len = pos - _start;
			reserve(capacity() == 0 ? 4 : capacity() * 2);
			pos = _start + len;//更新pos的位置 -- 解决迭代器失效
		}
		//挪动数据
		iterator end = _finish - 1;
		while (end >= pos) { // insert要扩容的时候，这个循环就失效了，停不下来了，为什么？
			*(end + 1) = *end;
			end--;
		}
		//扩容之后，旧空间的数据拷贝到新空间
		//旧空间已经被释放了 -- pos是指向旧空间的一个数字的位置的
		//pos成了野指针！ -- 迭代器失效
		//所以我们要把pos更新一下
		// -- 修改了内部的pos之后，其实问题还没有根本的解决
		*pos = x;
		++_finish;
	}
	iterator erase(iterator pos) {
		assert(pos >= _start);
		assert(pos < _finish);
		//挪动覆盖删除
		iterator begin = pos + 1;
		while (begin < _finish) {
			*(begin - 1) = *begin;
			++begin;
		}
		--_finish;
		//删除了位置的下一个位置 -- 还是pos
		return pos;
	}
	T& front() {
		assert(size() > 0);
		return *_start;
	}
	T& back() {
		assert(size() > 0);
		return *(_finish - 1);
	}
private:
	iterator _start;//start相当于整个数组的开始位置
	iterator _finish;//[_start,_finish)
	iterator _end_of_storage;
};


/* ---------------------- 验证MyVector的一些测试函数，封装ListQueue后屏蔽以下内容！---------------------- */
#if 0
void PrintVector(MyVector<int>& v) {//在没有写拷贝构造的时候，这里一定要引用的，不然浅拷贝，释放两次会出问题
	MyVector<int>::iterator it = v.begin();
	while (it != v.end()) {
		cout << *it << " ";
		it++;
	}
	cout << endl;
}
void PrintVector(const MyVector<int>& v) {
	MyVector<int>::const_iterator it = v.begin();
	while (it != v.end()) {
		cout << *it << " ";
		it++;
	}
	cout << endl;
}
void test_vector1() {
	//测试尾插
	MyVector<int>v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
	v.push_back(6);
	for (int i = 0; i < v.size(); i++) {
		++v[i];
		cout << v[i] << " ";
	}
	cout << endl;
}
void test_vector2() {
	//测试迭代器
	MyVector<int>v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
	v.push_back(6);
	MyVector<int>::iterator it = v.begin();
	while (it != v.end()) {
		(*it)--;
		cout << *it << " ";
		it++;
	}
	cout << endl;

	for (auto& e : v) {
		e++;
		cout << e << " ";
	}
	cout << endl;

	it = v.begin();
	while (it != v.end()) {
		cout << *it << " ";
		it++;
	}
	cout << endl;

	PrintVector(v);
}
void test_vector3() {
	//测试const迭代器
	const MyVector<int>v;
#if 0 //改不了的
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
	v.push_back(6);
#endif
	PrintVector(v);
}
//迭代器失效问题
void test_vector4() {
	MyVector<int>v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	//v.push_back(5); // 我们发现  -- 原来4个数据，insert之后要扩容的时候，就出现问题了！
	//v.push_back(6);
	MyVector<int>::iterator it = v.begin();
	while (it != v.end()) {
		cout << *it << " ";
		it++;
	}
	cout << endl;
	//
	auto p = find(v.begin(), v.end(), 3);
	if (p != v.end()) {
		v.insert(p, 30);

		//在内部初步解决迭代器失效之后，其实问题还是没有根本解决！
		//因为内部的pos更新不会影响p
		//所以我们在使用的时候
		//在p位置插入数据以后，不要访问p，因为p可能失效了
		//因为我们使用STL的时候，不了解底层的扩容机制，所以以后我们insert之后，不要去访问p位置！ -- 可能会有问题

		//那能否把insert第一个参数改成&行吗？ -- 尽量不要这样做 -- 和库保持一致好
		//虽然我们看似解决了问题 -- 但是改了可能会引出其它问题 -- 比如，头插，我们想传v.begin();v.insert(v.begin(), 0);
		//编不过，因为类型不匹配
#if 0
		cout << *p << endl;
		v.insert(p, 40);
#endif
	}
	PrintVector(v);
}
//erase迭代器会失效吗？
//库里面的erase会失效吗？ -- 不知道
//STL并没有规定什么机制
//有没有一种可能，当size()<capacity()/2的时候，缩一下容（缩容：以时间换空间）
//反正用完那个别访问，别动那个p就行了！
void test_vector5() {
	MyVector<int>v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	auto p = find(v.begin(), v.end(), 3);
	if (p != v.end()) {
		v.erase(p);
	}
	PrintVector(v);
}
void test_vector6() {
	MyVector<int>v;
#if 0
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
#endif
	//情况1和2
	//我们发现此时有这个5 -- 程序正常，没有 -- 崩溃
#if 1
	//情况3
	v.push_back(1);
	v.push_back(2);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
#endif
	//要求删除所有的偶数
	auto it = v.begin();
	//情况3的时候，it会跳过end()，直接继续往后了 -- 崩溃
	//所以下面这段算法是不正确的！
	//其实我们看源代码可以发现 -- erase是有返回值的 -- 会更新一下迭代器
	//STL规定erase返回删除位置下一个位置的迭代器！
	//vs这个编译器是查的很严格的 -- erase之后不允许去访问！ -- 访问就报错
	//Linux下就不同
	while (it != v.end()) {
		if (*it % 2 == 0) {
			it = v.erase(it);//我们要返回一个it才行
		}
		else {
			++it;
		}
	}
	PrintVector(v);
}
//总结
//其实迭代器失效，我们在自己做OJ的时候也能体会的，插入或者删除之后，迭代器指向了它不该指向的地方
//在使用vector的迭代器的时候，要多调试！

void test_vector7() {
	//系统默认的拷贝
	//1.自定义类型 -- 调拷贝构造 -- 默认生成的 -- 浅拷贝
	//2.内置类型 -- 值拷贝 -- 浅拷贝
	MyVector<int>arr;
	arr.push_back(1);
	arr.push_back(2);
	arr.push_back(3);
	for (auto i : arr) {
		cout << i << " ";
	}
	cout << endl;
	MyVector<int>arr2 = arr;
	//调用系统默认的话就是浅拷贝，这样程序肯定会崩溃的 -- 析构了两次
	//而且如果是浅拷贝，改一边的值另一边也会被改变
	arr2[0] = 100; //两边都会被改变的 -- 所以我们需要深拷贝！
	for (auto i : arr) {
		cout << i << " ";
	}
	cout << endl;
	for (auto i : arr2) {
		cout << i << " ";
	}
}
void test_vector8() {
	string s("hell");
	MyVector<int>vs(s.begin(), s.end());
	for (auto i : vs) {
		cout << i << " ";
	}
	cout << endl;
	//赋值
	MyVector<int>v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	vs = v;//要实现一个深拷贝
	//当然，如果f11 -- 这里是先进去拷贝的，因为传参要拷贝
	//走完拷贝就进去赋值了
	for (auto i : vs) {
		cout << i << " ";
	}
}
//其实内置类型也有构造
void test_type() {
	int i = 0;
	int j = int();
	int k = int(10);
}
void test_vector9() {
	MyVector<int>v(10, 1);//这样就报错了,如果传了两个参数都是int
	//这里出问题就是匹配问题，它匹配到不该去的地方去了
	//为什么 -- 因为迭代器区间构造那个函数，更适合这样传参 -- 所以进到那里去了
	//解决：
	//1.传参的时候写(10u,1)表示这个是个usigned int类型
	//2.把vector(size_t n,const T&val=T())里面的size_t改成int，也可以解决 -- 但是stl官方给的是size_t
	//3.复制一份，改成int，弄个重载就行 -- stl_3.0是这样解决的
	for (auto i : v) {
		cout << i << " ";
	}
	cout << endl;
}
void test_vector10() {
	MyVector<int>v;
	v.resize(10, 1);
	for (auto e : v) {
		cout << e << " ";
	}
	cout << endl;

	MyVector<int>v1;
	v1.reserve(10);
	v1.push_back(1);
	v1.push_back(2);
	v1.push_back(3);
	v1.push_back(4);
	v1.resize(8, 8);
	for (auto e : v1) {
		cout << e << " ";
	}
	cout << endl;

	v1.resize(20, 20);
	for (auto e : v1) {
		cout << e << " ";
	}
	cout << endl;

	v1.resize(3);
	for (auto e : v1) {
		cout << e << " ";
	}
	cout << endl;
}
#endif