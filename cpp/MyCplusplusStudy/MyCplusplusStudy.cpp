#include "pch.h"
#include <iostream>


/* test 2 */
struct Bar {
	using fr_t = void(*)(void);
	static void func(void) {
		std::cout << "Bar" << std::endl;
	}
	operator fr_t(void) {
		return func;
	}
};


/* test 3 */
template <class T>
class Singleton {
public:
	template <class... Args>
	static T* Instance(Args&&... args) {
		if (!m_pInstance) {
			m_pInstance = new T(std::forward<Args>(args)...);
		}
		return m_pInstance;
	}
	static T* GetInstance() {
		if (m_pInstance) {
			throw std::logic_error("the instance is not init, \
				please initialize the instance first");
		}
		return m_pInstance;
	}
	static void DestroyInstance() {
		delete m_pInstance;
		m_pInstance = nullptr;
	}

private:
	Singleton(void);
	virtual ~Singleton(void);
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);

private:
	static T* m_pInstance;
};

template<class T>T* Singleton<T>::m_pInstance = nullptr;

template<class T>
class String {
	T* m_pContent;
	size_t m_size;
public:
	String(T* content) : m_pContent(nullptr){
		if (m_pContent)
			return;

		T* t = content;
		m_size = 0;
		while (*t) {
			t++;
			m_size++;
		}
		m_pContent = new T(m_size + 1);
		memcpy(m_pContent, content, m_size);
		m_pContent[m_size] = 0;
	}
	virtual ~String() {
		if (m_pContent) {
			delete m_pContent;
			m_pContent = nullptr;
		}
	}
	size_t size() {
		return m_size;
	}
};


//template <class T>
//struct StringBuffer {
//	const T* GetString() const { return m_buf; }
//
//private:
//	T* m_buf;
//};
//
//template <class T>
//struct Writer {
//	Writer(const T& t) : m_buf(t) {}
//	void StartArray() {}
//	void EndArray() {}
//	void StartObject() {}
//	void EndObject() {}
//private:
//	T m_buf;
//};
//
//struct Document {
//
//};
//
//
///* test 5 */
//class JsonCpp {
//	typedef Writer<StringBuffer<char>> JsonWriter;
//	StringBuffer<char> m_buf;
//	JsonWriter m_writer;
//	Document m_doc;
//public:
//	JsonCpp() :m_writer(m_buf) {}
//	~JsonCpp(){}
//	void StartArray() { m_writer.StartArray(); }
//	void EndArray() { m_writer.EndArray(); }
//	void StartObject() { m_writer.StartObject(); }
//	void EndObject() { m_writer.EndObject(); }
//	template <class T>
//	void WriteJson(const std::string& key, T&& value) {
//		m_writer.String(key.c_str());
//		WriteValue(std::forward<T>(value));
//	}
//	template <class T>
//	void WriteJson(const char* key, T&& value) {
//		m_writer.String(key);
//		WriteValue(std::forward<T>(value));
//	}
//	const char* GetString() const { return m_buf.GetString(); }
//private:
//	template <class T>
//	typename std::enable_if<std::is_same<T, int>::value>::type WriteValue(T value) {
//		m_writer.Int(value);
//	}
//	template <class T>
//	typename std::enable_if<std::is_same<T, unsigned int>::value>::type WriteValue(T value) {
//		m_writer.Uint(value);
//	}
//	template <class T>
//	typename std::enable_if<std::is_same<T, int64_t>::value>::type WriteValue(T value) {
//		m_writer.Int64(value);
//	}
//	template <class T>
//	typename std::enable_if<std::is_floating_point<T>::value>::type WriteValue(T value) {
//		m_writer.Double(value);
//	}
//	template <class T>
//	typename std::enable_if<std::is_same<T, bool>::value>::type WriteValue(T value) {
//		m_writer.Bool(value);
//	}
//	template <class T>
//	typename std::enable_if<std::is_pointer<T>::value>::type WriteValue(T value) {
//		m_writer.String(value);
//	}
//	template <class T>
//	typename std::enable_if<std::is_array<T>::value>::type WriteValue(T value) {
//		m_writer.String(value);
//	}
//	template <class T>
//	typename std::enable_if<std::is_same<T, std::nullptr_t>::value>::type WriteValue(T value) {
//		m_writer.Null();
//	}
//};

#include<fstream>
#include<iostream>
std::ostream& get_out_stream(int type) {
	switch (type) {
	case 0: {
		static std::ofstream out("log.log");
		return out; }
	default:
		return std::cout; // << 并不是virtual函数
	}
}



/*creator policy classes   Create Interface*/
template <class T>
struct OpNewCreator {
	static T* Create() {
		return new T;//假如T为右值引用呢
	}
};

struct OpNewCreator1 {
	template <class T>
	static T* Create() {
		return new T;
	}
};


template <class T>
struct MallocCreator {
	static T* Create() {
		void* buf = std::malloc(sizeof(T));
		if (!buf) return 0;
		return new(buf) T; // placement new
	}
};

template <class T>
struct PrototypeCreator {
	PrototypeCreator(T* pObj = 0)
		: pPrototype_(pObj) {}
	T* Create() {
		return pPrototype_ ? pPrototype_->clone() : 0;
	}
	T* GetPrototype() { return pPrototype_; }
	void SetPrototype(T* pObj) { pPrototype_ = pObj; }
private:
	T* pPrototype_;
};

 //host class
template <class CreationPolicy>
class WidgetManager1 : public CreationPolicy {

};

class widget{};
class uidget{};

typedef WidgetManager1<OpNewCreator<widget>> MyWidgetManager1;

template <template <class Created> 
		  class CreationPolicy = MallocCreator>
class WidgetManager : public CreationPolicy<widget> {
public:
	uidget* get_uidget() {
		return CreationPolicy<uidget>().Create();
	}
};

typedef WidgetManager<> MyDefWidgetManager;
typedef WidgetManager<OpNewCreator> MyWidgetManager;

void test() {
	typedef WidgetManager<PrototypeCreator> MyWidgetManager;
	widget* pPrototype = new widget;
	MyWidgetManager mgr;
	mgr.SetPrototype(pPrototype);
}

#include <stdexcept>
// 友元模板前置声明不可省略
template <typename T> class my_stack;
template <typename T> class list_node {
	T value;
	list_node *next;
	list_node(T const &v, list_node* n)
		: value(v), next(n){}
	friend class my_stack<T>;
};
template <typename T = int>class my_stack {
	typedef list_node<T> node_type;
	node_type *head;
	my_stack operator=(my_stack const&) {}
	my_stack(my_stack const&) {}
public:
	my_stack() : head(0) {}
	~my_stack() { while (!empty()) pop(); }
	bool empty() { return !head; }
	T const& top() const throw (std::runtime_error) {
		if (empty())
			throw std::runtime_error("stack is empty!");
		return head->vlaue;
	}
	void push(T const& v) {head = new node_type(v, head);}
	void pop();
};
template <typename T>void my_stack<T>::pop() {
	if (head) {
		node_type *tmp = head;
		head = head->next;
		delete tmp;
	}
}
template <typename T = int>class count_stack : public my_stack<T> {
	typedef my_stack<T> base_type;
	unsigned size;
public:
	count_stack() : base_type(), size(0){}
	void push(T const& v) {
		base_type::push(v);
		size++;
	}
	void pop() {
		if (size > 0) {
			base_type::pop();
			size--;
		}
	}
	unsigned getSize() const { return size; }
};


void call();
void call1();

//typedef unsigned long long size_t;

#if 0
template<class T, void (*f)(T& v)>
void foreach(T arrs[], size_t size) {
	for (size_t i = 0; i < size; i++) {
		f(arrs[i]);
	}
}
#elif 1
template<class T, template<class R, class...Args>class Func>
void foreach(T arrs[], size_t size) {
	for (size_t i = 0; i < size; i++) Func<T, T>()(arrs[i]);
}

template<class T, template<class R, class...Args>class Func>
void foreach1(T arrs[], size_t size) {
	for (size_t i = 0; i < size; i++) Func<T>()();
}
#endif

template<class R, class...Args>
class inc1 {
public:
	void operator()(Args... args) {
		std::cout << sizeof...(args) << std::endl;
	}
};

template<class T>
void inc(T& v) {
	++v;
}
template<class T>
void dec(T& v) {
	--v;
}
template<class T>
void print(T& v) {
	std::cout << v << " ";
}

template<class R, class T>
R printarg(T t) {
	std::cout << t << std::endl;
	return R(t);
}

#include <algorithm>
template<class R, class...Args>
void expand(Args...args) {
	R arr[] = { printarg<R>(args)... };
	std::for_each(arr, arr + sizeof...(args), [=](R r) {
		std::cout << r << " ";
	});
	std::cout << std::endl;
}


template <class T>T split(T t) {return t;}

//#include <initializer_list>
//template <class T, class...Args>
//T& get(T& t, Args&...args) {
//	if (sizeof...(args) > 0)
//		get(args...);
//	return t;
//}
//template <class T,class...Args>
//const T& get(const T& t, const Args&...args) {
//	return sizeof...(args) > 1 ? get(args...) : t;
//}

template <class T>
T const& max1(T const& t, T const& x) { return t > x ? t : x; }

template <class T>
struct Num {
	struct Type {
		T m_t;
	} m_t;
};

template <class T>
struct Type {
	typedef /*typename*/ T type;
};
//template <class T>
//T const& max(typename Type<T>::type const& t, T const& x) { return t > x ? t : x; }
template <class T>
T const& max(T const& x, typename Type<T>::type const& t) { return t > x ? t : x; }

#include <unordered_map>
std::unordered_map<int, int>::iterator it = std::unordered_map<int, int>().begin();

#include "the_class.hpp"
int main() {
	List<int> lst;
	lst.push_front(0);
	lst.push_front(5);
	lst.push_front(7);
	List<int>::iterator it = lst.begin();
	for (; it != lst.end(); ++it) {
		std::cout << *it << " ";
	}

	auto in = max(1.7, 1);
	Num<double> n{ {1.9} };
	auto is = max(n.m_t.m_t, 1);

	//expand<int>(1, 2, 3, 4, 5);
	//expand<double>(1, 2, 3, 4, 5);
	//get(1);
	//get(1, 2, 3.5, "qwe", 5);//(int, int, double, const char[4], int)

	/* test 1 */
	//__asm{
	//	mov eax, 0xffffffff
	//	neg eax
	//	mov eax, 0xffffffff
	//	not eax
	//}

	/* test 2 */
	Bar bar;
	bar();

	/* test 3 */
	std::cout << "size = " << Singleton<String<char>>::Instance((char*)"Hello")->size() << std::endl;

	/* test 4 */
	auto i = 5;
	const auto* pi = &i, u  = 6;

	/* test 5 */
	//JsonCpp jcp;
	//jcp.StartArray();
	//for (size_t i = 0; i < 100; i++) {
	//	jcp.StartObject();
	//	jcp.WriteJson("ID", i);
	//	jcp.WriteJson("Name", "Peter");
	//	jcp.WriteJson("Address", "Zhuhai");
	//	jcp.EndObject();
	//}
	//jcp.EndArray();

	/* test 6 */
	get_out_stream(0) << "again" << " 1" << std::endl;
	get_out_stream(1) << "again" << " 2" << std::endl; // 没有输出

	MyWidgetManager mwm;
	uidget* uiPointer = mwm.get_uidget();

	my_stack<int> mystack;
	count_stack<char> cstack;
	int nums[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
	for (auto i : nums) {
		mystack.push(i);
		cstack.push(i);
	}

	call();
	call1();

	foreach<int, inc1>(nums, sizeof(nums) / sizeof(nums[0]));
	foreach1<int, inc1>(nums, sizeof(nums) / sizeof(nums[0]));
	//foreach<int, print<int> >(nums, sizeof(nums) / sizeof(nums[0]));
	//foreach<int, dec<int> >(nums, sizeof(nums) / sizeof(nums[0]));
	//foreach<int, print<int> >(nums, sizeof(nums) / sizeof(nums[0]));

	int wait;
	std::cin >> wait;
}