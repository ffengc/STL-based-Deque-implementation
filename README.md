
C++封装类如果涉及到模板 -- 不支持直接分文件(.h和.cpp)
	如果分声明和定义分离 -- 需要在很多地方加上typename等关键字才能正常运行
	否则会报出链接错误
因此本次大作业的理论课题2，声明和定义均在.h文件中实现
理论课题1和实验课题声明和定义均分离

测试结果均在result.txt中，在test.cpp里面已经标好了第一问，第二问，第三问的测试结果

## ListQueue的实现
内部结构：
	Mylist<MyVector<Type>> __inner_list;
	size_t __nodes_count = 0;
	size_t __size = 0;

### 支持的功能:（以下所有功能都在result.txt中的测试结果中体现）
#### 题目要求的功能:
	尾插、尾删、头插、头删、任意位置的删除的插入、[]重载、以及打印内部信息print接口
#### 额外实现的功能：
	1. 通过实现 __list_queue_iterator 类
		支持stl迭代器的行为（包括const迭代器）
		支持迭代器迭代
		支持范围for迭代
		兼容stl算法中部分组件，如find,find_if等
			调用方式和stl其它组件调用方式一致，如：
			auto it = find(lq.begin(),lq.end(),3);
	2. 提供clear(),size()等其它stl容器也有的接口
	3. 支持ListQueue<T>::sort()
	4. 支持初始化列表类型的构造，如：
		ListQueue<int>lq = {1,2,3,4,5};
	5. 支持迭代器区间的构造，如：
		std::set<int>s = {1,2,3};
		ListQueue<int>lq(s.begin(),s.end());

### 内部结构的功能：
	MyVector和Mylist
	均模仿stl::vector,stl::list的功能实现
	其中MyVector用顺序表实现，list采用双向链表实现
#### MyVector
	1. 支持stl::vector中大部分核心功能，除了基本的push_back/pop_back/erase/insert
		还支持resize,reverse,swap,capacity,size等功能
	2. 支持与stl一样的多种构造方式，包括迭代器区间构造，初始化列表构造等
	3. 支持迭代器（包括const迭代器）行为 -- 支持的功能和ListQueue一样
#### Mylist
	1. 支持stl::list中大部分核心功能，除了基本的push_back/pop_back/push_front/pop_front/erase/insert
		**为了ListQueue的封装更加容易，我还添加了[]重载，以便保证上层封装代码的简洁**
	2. 支持与stl一样的多种构造方式，包括迭代器区间构造，初始化列表构造等
	3. 通过实现 __list__iterator 类
		支持迭代器行为（包括const迭代器） -- 支持的功能和ListQueue一样
	4. 通过实现	reverse_iterator.h 支持反向迭代器功能（该功能同样可以用在MyVector上）

### 内部结构的实现细节
	MyVector和Mylist 均按照stl的方式，进行了一定程度上的简化实现 -- 相比于stl的vector和list，已经实现了大部分核心功能
#### ListQueue实现细节
	ListQueue整体结构按照题目要求进行实现 -- 类似于std::deque
#### ListQueue结构的内部的规则
	push_back -- 直接往双向链表最后一个节点的vector插入
		如果这个vector满了 -- 开新节点
	pop_back -- 删除双向链表最后一个节点的vector的最后一个元素
		如果删完之后，该vector空了 -- 链表pop_back()一下，删除该节点
	insert -- 通过计算元素个数的方式 -- 找到要插入的位置是属于哪一个vector的
		如果此时这个vector满了 -- 先把东西插入进去，此时，该vector的大小超出规定容量一个单位
		1. 此时，如果该节点的下一节点上的vector还未满 -- 把当前超容量的vector的最后一个元素insert到后一个节点的vector的头，即头插
		2. 此时，如果该节点的下一节点的vector已经满了 -- 调用Mylist的insert，在当前节点的后面插入一个新的节点
			然后把超容量的vector（即当前节点的vector）的最后一个元素插入到新节点的新vector上
	push_front -- 复用insert
	erase -- 通过计算元素个数的方式 -- 找到要删除的元素是属于哪一个vector的
		通过计算位置的方式 -- 找到这个元素在vector中的位置 -- 直接调用MyVector的erase删除即可
		如果删完之后，vector空了 -- 调用Mylist的erase删除该节点即可
	[]重载 -- 计算pos位置的元素属于哪个vector -- 再通过计算位置即可得到结果
#### 其它功能实现细节
	析构函数和clear -- clear直接调用Mylist的clear即可 -- 不用担心vector的释放，析构直接调用默认析构即可
		自定义类型的默认析构 -- 会调用自己的析构函数
		我们设置结构的时候是 Mylist<MyVector<Type>> __inner_list;
		而不是 Mylist<MyVector<Type>*> __inner_list;
		因此不会造成内存泄漏
	sort -- 先把元素拷贝到一个std::vector上，排好序再拷贝回去