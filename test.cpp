
#define _CRT_SECURE_NO_WARNINGS 1

#ifdef __cplusplus
#elif
#endif

/*希望老师在批改之前可以看一下代码的文档 -- README.md -- 谢谢！*/



#include"ListQueue.h"
#include<set>
#include"Person.h"

//第一问
void test_MyVector()
{
	/*实现基本数据类型如int, double,以及自定义类型Student（第一题）的实例化*/
	//测试MyVector
	MyVector<int>v1 = { 1,2,3 };
	for (const auto& e : v1)
	{
		cout << e << " ";
	}
	cout << endl;

	MyVector<double>v2 = { 1.0,2.0,3.0 };
	for (const auto& e : v2)
	{
		cout << e << " ";
	}
	cout << endl;

	Student s1;
	Student s2;
	MyVector<Student>v3 = { s1,s2 };
	for (const auto& e : v3)
	{
		cout << e;
	}
	cout << endl;
}
//第二问
void test0()
{
	Mylist<int> tmp;
	tmp.push_back(1);
	tmp.push_back(2);
	tmp.push_back(3);
	tmp.push_back(4);
	tmp.push_back(5);
	tmp.push_back(6);
	tmp.pop_back();
	tmp.pop_back();
	tmp.pop_back();
	for (size_t i = 0; i < tmp.size(); i++)
	{
		cout << tmp[i] << " ";
	}
	cout << endl;
}
void test1()
{
	//测试push_back和pop_back
	ListQueue<int>l;
	l.push_back(1);
	l.push_back(2);
	l.push_back(3);
	l.push_back(4);
	l.push_back(5);
	l.push_back(6);
	cout << " -------------- 测试insert -------------- " << endl;
	//在3前面插入一个30
	l.insert(2, 30);
	//在30前面插入一个300
	l.insert(2, 300);
	l.output_detail();
	cout << endl;
	//头插一个0
	l.insert(0, 0);
	l.insert(0, -1);
	l.output_detail();
	cout << endl;
	l.output();
	l.output_detail();
	cout << " -------------- 测试erase -------------- " << endl;
	l.erase(4);
	l.output_detail();
	l.erase(3);
	l.output_detail();
	l.pop_front();
	l.pop_front();
	l.output_detail();
	l.pop_front();
	l.output_detail();
	cout << " -------------- 测试operator[] -------------- " << endl;
	for (size_t i = 0; i < l.size(); i++)
	{
		cout << l[i] << " ";
	}
	cout << endl << endl << endl << endl;

	cout << " -------------- 测试print -------------- " << endl;
	l.print();
}
void test2()
{
	//测试拷贝构造
	ListQueue<int>lq = { 1,2,3,4,5,6,7,8,10 };
	lq.output();
	ListQueue<int>lq_copy(lq);
	lq_copy.output();
	cout << endl;

	//测试迭代器区间构造
	set<int>s = { 1,3,5,7,9,2,4,6,8,10 };
	ListQueue<int>lq2(s.begin(), s.end());
	lq2.output();
	cout << endl;
}
void test3()
{
	//测试查找和排序
	ListQueue<int>lq = { 1,2,3,4,5,6,7,1,2,3 };
	lq.output_detail();
	lq.sort();
	lq.output_detail();

	lq.erase(lq.find(3));
	lq.output_detail();
	//经过测试 -- 排序和删除没问题
}
void test4()
{
	//测试迭代器
	ListQueue<int>lq = { 1,2,3,4,5,6,7 };
	ListQueue<int>::iterator it = lq.begin();
	while (it != lq.end())
	{
		(*it)++;
		(*it) -= 3;
		cout << *it++ << " ";
	}
	cout << endl;

	//测试迭代器 -- 范围for也可以用了 -- 范围for底层是迭代器的替换
	for (const auto& e : lq)
	{
		cout << e << " ";
	}
	cout << endl;
}
void test5()
{
	//测试ListQueue和STL部分组件兼容性
	ListQueue<int>lq = { 1,2,3,4,5,6,7,8 };
	auto it = find(lq.begin(), lq.end(), 3);
	cout << *it << endl;
}
//第三问
#define TEST_SIZE 1000000
void req_test()
{
	/*
	1. 在当前的容器中，以[]的方式随机访问一个元素，然后在它之前插入一个新元素，直到容器中达到100K个元素
	2. 对元素进行随机删除，直至ListQueue对象为空
	*/
	//先插入几个元素
	ListQueue<int>test_list;
	test_list.push_back(1);
	test_list.push_back(2);
	test_list.push_back(3);
	//开始在随机位置插入随机元素
	while (test_list.size() < TEST_SIZE)
	{
		//在0~size位置中随机生成一个数字
		size_t idx = rand() % test_list.size();
		cout << test_list[idx] << " ";
		size_t num = rand() % INT_MAX;
		test_list.insert(idx, num);
	}
	//test_list.output();
	//test_list.output_detail();
	while (test_list.size() > 0)
	{
		//在0~size位置中随机生成一个数字
		size_t idx = rand() % test_list.size();
		test_list.erase(idx);
	}
	//在随机位置删除元素
	test_list.output_detail();
	cout << endl;
}
#include<time.h>
int main()
{
	//test0();
	//test1();
	//test2();
	//test3();
	//test4();
	//test5();
	srand((unsigned int)time(NULL));
	//req_test();
	test_MyVector();
	return 0;
}