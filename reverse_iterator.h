#pragma once



/*希望老师在批改之前可以看一下代码的文档 -- README.md -- 谢谢！*/
/*代码已按照要求实现了题目中所有功能，详情请见代码的文档*/



//复用，迭代器适配器
template<class Iterator, class Ref, class Ptr>
struct __reverse_iterator {
	Iterator _cur;
	typedef __reverse_iterator<Iterator, Ref, Ptr> RIterator;
	__reverse_iterator(Iterator it)
		:_cur(it) {}
	//为了对称，stl源码进行了一些操作
	RIterator operator++() { //迭代器++，返回的还是迭代器
		--_cur;//反向迭代器++，就是封装的正向迭代器--
		return *this;
	}
	RIterator operator--() {
		++_cur;//反向迭代器++，就是封装的正向迭代器--
		return *this;
	}
	Ref operator*() {
		//return *_cur;
		//为什么这里需要拷贝一下对象呢？
		//因为解引用只是取一下数据，迭代器位置肯定是不能变的 -- 变了肯定会出问题的
		auto tmp = _cur;
		--tmp;
		return *tmp;
	}
	Ptr operator->() {
		return &(operator*());
	}
	bool operator!=(const RIterator& it) {
		return _cur != it._cur;
	}
};