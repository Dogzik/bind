#include <iostream>
#include <string>
#include <iostream>
#include <utility>
#include <tuple>
#include "bind.h"
using std::string;
using std::cin;
using std::cout;

void r(int a, int b) {
	std::cout << a << " " << b << "\n";
}

int main()
{
	int x = 322;
	auto w = bind(r, x, _1);
	w(2);
	return 0;
}