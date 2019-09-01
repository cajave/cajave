
#include <initializer_list>
#include <vector>
#include <memory>
#include<iostream>

template <class T>
class BlobPtr;

template <typename T>
class Blob{
	friend class BlobPtr<T>;
public:
	// typedef T value_type;
	using value_type = T;
	// typedef typename std::vector<T>::size_type size_type;
	using size_type = typename std::vector<T>::size_type;
	using iterator = typename std::vector<T>::iterator;
	
	Blob();
	Blob(std::initializer_list<T> il);
	template <class I>
	Blob(I beg, I end);
	size_type size() const {return data->size();}
	bool empty() const {retrun data->empty();}
	void push_back(const T& t){data->push_back(t);}
	void pop_back();
	T& back();
	T& operator[](size_type i);
	const T& operator[](size_type i) const;
	iterator begin(){return data->begin();}
	iterator end(){return data->end();}
	
private:
	std::shared_ptr<std::vector<T>> data;
	void check(size_type i, const std::string& msg) const;
};

template <class T>
void Blob<T>::check(size_type i, const std::string & msg) const{
	if (i >= data->size()){
		throw std::out_of_range(msg);
	}
}

template <class T>
T& Blob<T>::back(){
	check(0, "back on empty Blob");
	return data->back();
}

template <class T>
T& Blob<T>::operator[](size_type i){
	std::cout << "non-const" << std::endl;
	// check(i, "out of range");
	// return (*data)[i];
/* 	using type = const T& (Blob<T>::*)(size_type) const;
	type func = &Blob<T>::operator[];
	return const_cast<T&>((this->*func)(i));	 */
	return const_cast<T&>((this->*static_cast<const T&(Blob<T>::*)(size_type)const>(&Blob<T>::operator[]))(i));
}

template <class T>
void Blob<T>::pop_back(){
	check(0, "pop_back on empty Blob");
	data->pop_back();
}

template <class T>
const T& Blob<T>::operator[](size_type i) const {
	std::cout << "const" << std::endl;
	check(i, "out of range");
	return (*data)[i];
}

template <class T>
Blob<T>::Blob()
: data(std::make_shared<std::vector<T>>()){
	
}

template <class T>
Blob<T>::Blob(std::initializer_list<T> il)
: data(std::make_shared<std::vector<T>>(il.begin(), il.end())){
	std::cout << "Blob<T>::Blob(std::initializer_list<T> il)" << std::endl;
}

template<class T>
template<class I>
Blob<T>::Blob(I beg, I end)
: data(std::make_shared<std::vector<T>>(beg, end)){
	std::cout << "Blob<T>::Blob(I beg, I end)" << std::endl;
}

template <typename T>
class BlobPtr{
public:
	BlobPtr():curr(0){}
	BlobPtr(Blob<T> &a, size_t sz = 0):wptr(a.data), curr(sz){}
	T& operator*()const{
		auto p = check(curr, "dereference past end");
		return (*p)[curr];
	}
	BlobPtr& operator++(); // BlobPtr<T>& operator++();
	BlobPtr& operator--(); // BlobPtr<T>& operator--();
private:
	std::shared_ptr<std::vector<T>> check(std::size_t, const std::string&) const;
	std::weak_ptr<std::vector<T>> wptr;
	std::size_t curr;
};

template <class T>
std::shared_ptr<std::vector<T>> BlobPtr<T>::check(std::size_t i, const std::string& s) const{
	if (!wptr.expired()){
		if (i >= wptr.lock()->size()){ //size_t 为无符号类型，不可能小于0
			throw std::out_of_range(s);
		}
		return wptr.lock();
	}
	return nullptr;
}

template <class T>
BlobPtr<T>& BlobPtr<T>::operator++(){
	++curr;
	return *this;
}

template <class T>
BlobPtr<T>& BlobPtr<T>::operator--(){
	--curr;
	return *this;
}

#include<string>
int main(){
	int nums[] = {0, 2, 5, 6, 7, 8, 20};
	const Blob<int> blob(nums, nums + sizeof(nums)/sizeof(nums[0]));
	std::cout << blob[4] << std::endl;
	// std::cout << blob[7] << std::endl;
	std::cout << "======" << std::endl;
	Blob<std::string> b{"hello", "good", "again", "end"};
	std::cout << b[2] << std::endl;
	// std::cout << b[7] << std::endl;
	
	for(const auto& i : b){
		std::cout << i << " ";
	}
	
	std::cout << std::endl;
	BlobPtr<std::string> p(b, 1);
	std::cout << *++p << std::endl;
}