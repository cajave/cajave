#pragma once

template <typename T>
struct the_class {
	static int id;
	the_class() { ++id; }
};

template <typename T> int the_class<T>::id = 0;

#include <stdexcept>

template<class T>
struct type_traits {
	using value_type = typename T::value_type;
	using reference_type = typename T::reference_type;
	using const_reference_type = typename T::const_reference_type;
	using pointer_type = typename T::pointer_type;
	using const_pointer_type = typename T::const_pointer_type;
};
template<>
struct type_traits<int> {
	using value_type = int;
	using reference_type = int&;
	using const_reference_type = const int&;
	using pointer_type = int*;
	using const_pointer_type = const int*;
};


template <class T>
class List;

template <class T>
class ListIterator {
	template <class X>
	friend class List;

	T* pos;
public:
	using value_type = typename type_traits<T>::value_type;
	using reference_type = typename type_traits<T>::reference_type;
	using const_reference_type = typename type_traits<T>::const_reference_type;
	using pointer_type = typename type_traits<T>::pointer_type;
	using const_pointer_type = typename type_traits<T>::const_pointer_type;
	//using value_type = typename T::value_type;
	//using reference_type = typename T::reference_type;
	//using const_reference_type = typename T::const_reference_type;
	//using pointer_type = typename T::pointer_type;
	//using const_pointer_type = typename T::const_pointer_type;
	using self_type = ListIterator<T>;

	ListIterator() : pos(0){}
	ListIterator(T* p) : pos(p){}
	bool operator!=(const self_type& it) {
		return pos != it.pos;
	}
	bool operator==(const self_type& it) {
		return pos == it.pos;
	}
	self_type& operator++() {
		if (pos)
			pos = pos->next;
		return *this;
	}
	self_type operator++(int) {
		self_type tmp = *this;
		if (pos)
			pos = pos->next;
		return tmp;
	}
	reference_type operator*() {
		if (pos)
			return pos->value;
		else
			throw std::runtime_error("dereference null iterator!");
	}
	pointer_type operator->() {
		return &pos->value;
	}
};


template <class T>
struct ListNode {
	using value_type = T;
	using reference_type = T & ;
	using const_reference_type = const T&;
	using pointer_type = T * ;
	using const_pointer_type = const T*;

	T value;
	ListNode* prev;
	ListNode* next;

	ListNode(T v, ListNode* p, ListNode* n)
		: value(v), prev(p), next(n){}
};

template<class T>
class List {
	using node_type = ListNode<T>;
	node_type* head;
public:
	using value_type = T;
	using iterator = ListIterator<ListNode<T>>;
	List() : head() {}
	~List() {
		while (head) {
			node_type *n = head;
			head = head->next;
			delete n;
		}
	}
	void push_front(T const &v) {
		head = new node_type(v, 0, head);
		if (head->next) head->next->prev = head;
	}
	void pop_front(T const& v) {
		if (head) {
			node_type* n = head;
			head = head->next;
			head->prev = 0;
			delete n;
		}
	}
	void insert(iterator it, T const& v) {
		node_type*n = it.pos;// 插在该位置之后
		if (n) {
			node_type* new_node = new node_type(v, n, n->next);
			new_node->next->prev = new_node;
			n->next = new_node;
		}
	}

	// n->prev         n     n->next(it)
	//     \            \          \
	// prev|next    prev|next   prev|next
	void erase(iterator &it) {
		node_type* n = it.pos;// 删除该位置节点
		++it;
		if (n) {
			if (n->next) n->next->prev = n->prev;
			if (n->prev) n->prev->next = n->next;
			if (head == n) head = it.pos;
		}
	}

	bool empty() const { return head == 0; }
	iterator begin() { return iterator(head); }
	iterator end() { return iterator(); }
};