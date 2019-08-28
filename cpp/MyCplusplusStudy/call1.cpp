#include "pch.h"
#include <iostream>
#include "the_class_1.hpp"

void call1() {
	the_class<int> c;
	std::cout << c.id << std::endl;
	std::cout << std::hex << &c.id << std::endl;
}