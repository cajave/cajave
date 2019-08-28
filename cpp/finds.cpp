
#include <iterator> //std::iterator_traits

namespace finds{

template <class ITER>
ITER find1(ITER begin, ITER end, const typename std::iterator_traits<ITER>::value_type& value){
	while (begin != end && *begin != value)
		++begin;
	return begin;
}

template <class ITER, class T>
ITER find2(ITER begin, ITER end, const T& value) {
	while (begin != end && *begin != value)
		++begin;
	return begin;
}

} // end of finds namespace

#include <iostream> //std::cout
#include <initializer_list> // std::initializer_list
int main(){
	std::initializer_list<double> list1{0.0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6}; 
	std::initializer_list<int> list2{0, 1, 2, 3, 4, 5, 6}; 
	
	auto it1 = finds::find1(list1.begin(), list1.end(), 3.3);
	if (it1 != list1.end()){
		std::cout << "3.3 : it1 = " << *it1 << std::endl;
	} else {
		std::cout << "it1 not find!" << std::endl;
	}
	
	auto it2 = finds::find1(list2.begin(), list2.end(), 4.6); // 依然找到了4，4.6被截断为4
	if (it2 != list2.end()){
		std::cout << "4.6 : it2 = " << *it2 << std::endl;
	} else {
		std::cout << "it2 not find!" << std::endl;
	}
	
	auto it3 = finds::find2(list1.begin(), list1.end(), 3.3);
	if (it3 != list1.end()){
		std::cout << "3.3 : it3 = " << *it3 << std::endl;
	} else {
		std::cout << "it3 not find!" << std::endl;
	}
	
	auto it4 = finds::find2(list2.begin(), list2.end(), 4.6);
	if (it4 != list2.end()){
		std::cout << "4.6 : it4 = " << *it4 << std::endl;
	} else {
		std::cout << "it4 not find!" << std::endl;
	}
	
	return 0;
}
