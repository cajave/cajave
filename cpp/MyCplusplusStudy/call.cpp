#include "pch.h"
#include <iostream>
#include "the_class.hpp"

void call() {
	the_class<int> c;
	std::cout << c.id << std::endl;
	std::cout << std::hex << &c.id << std::endl;
}