
//#include <type_traits>
namespace type_select{
	
template <class T>
class disable_inference{
public:
	using type = T;
};

} // end of type_select namespace


namespace rge{
	
using size_t = unsigned long long;
	
template<class T>
class range{
public:
	range(T b, T e, T s)
	: begin_(b), step_(s), size_(calc_size(e)){
	}
	
	size_t size() const {
		return size_;
	}
	bool empty() const {
		return size_ == 0;
	}
	
	class iterator;
	iterator begin(){
		return iterator(this, 0);
	}
	iterator end(){
		return iterator(this, size_);
	}
	
public:
	class iterator {
	public:
		using item = iterator;
		iterator(range* p, size_t i)
		: ptr_(p), index_(i){
		}
		T operator*(){
			return ptr_->begin_ + ptr_->step_ * index_;
		}
		item& operator++(){
			++index_;
			return *this;
		}
		bool operator!=(const item& i) const {
			return index_ != i.index_;
		}
	private:
		range* ptr_;
		size_t index_;
	};

private:
	size_t calc_size(T end) {
		if (step_ == 0 || end == begin_ || (end - begin_) / step_ < 0){
			return 0;
		} else {
			size_t tmp = (end - begin_) / step_;
			return step_ > 0
				? begin_ + tmp * step_ < end ? tmp + 1 : tmp
				: begin_ + tmp * step_ > end ? tmp + 1 : tmp;
		}
	}

private:
	const T begin_;
	const T step_;
	const size_t size_;
};

} // end of rge namespace

namespace std{

template<class B, class E, class S>
auto range(B begin, E end, S step)->rge::range<decltype(end-step+begin)>{
	using R = decltype(end-step+begin);
	return rge::range<R>(begin, end, step);
}

template<class E, class S>
auto range(E end, S step)->decltype(range(E(), end, step)){
	return range(E(), end, end < E() && step > 0 ? -step : step);
}

template<class E>
auto range(E end)->decltype(range(end, 0)){
	return range(end, end > 0 ? 1 : -1);
}

} // end of std namespace

#define RANGE_OUTPUT(out, nums) do {\
	(out) << "	empty : " << ((nums).empty() ? "true" : "false") << std::endl;\
	(out) << "	size : " << (nums).size() << std::endl;\
	(out) << "	list : ";\
	for (auto i : (nums)) (out) << i << " ";\
} while(0)

#define RANGE_FOR_3(out, begin, end, step) do {\
	auto&& nums = std::move(std::range((begin), (end), (step)));\
	(out) << "range(" << #begin << ", " << #end << ", " << #step << ") : " << std::endl;\
	RANGE_OUTPUT(out, nums);\
} while(0)

#define RANGE_FOR_2(out, end, step) do {\
	auto&& nums = std::move(std::range((end), (step)));\
	(out) << "range(" << #end << ", " << #step << ") : " << std::endl;\
	RANGE_OUTPUT(out, nums);\
} while(0)

#define RANGE_FOR_1(out, end) do {\
	auto&& nums = std::move(std::range((end)));\
	(out) << "range(" << #end << ") : " << std::endl;\
	RANGE_OUTPUT(out, nums);\
} while(0)

#include <iostream> //std::boolalpha
//#include <iomanip> std::boolalpha
int main(){
	/*
	(8)
	(8.1)
	(8, 2)
	(8, 2.1)
	(8.1, 2)
	(8.1, 2.1)
	取负值的情况也要考虑。
	*/
	decltype((std::cout)) OUT = std::cout;
	
	auto&& nums = std::move(std::range(1, 8, 3)); // 1, 4, 7
	OUT << "range(1, 8, 3) : " <<std::endl; 
	OUT << "	empty : " << std::boolalpha << nums.empty() << std::endl;
	OUT << "	size : " << nums.size() << std::endl;
	OUT << "	list : ";
	for (auto i : (nums)) OUT << i << " ";

	OUT << std::endl;
	RANGE_FOR_1(OUT, 0);
	OUT << std::endl;
	RANGE_FOR_1(OUT, 8);
	OUT << std::endl;
	RANGE_FOR_1(OUT, -1);
	OUT << std::endl;
	RANGE_FOR_1(OUT, 8.1);
	OUT << std::endl;
	RANGE_FOR_2(OUT, 8, 2.1);
	OUT << std::endl;
	RANGE_FOR_3(OUT, 1, 8, 3);
	OUT << std::endl;
	RANGE_FOR_3(OUT, -1, -8, -2.4);
	OUT << std::endl;
	return 0;
}
