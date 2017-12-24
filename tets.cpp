#include <iostream>
#include <string>
#include <iostream>
#include <utility>
#include <tuple>
#include "bind.h"
using std::string;
using std::cin;
using std::cout;


void string_print(string&& a, string b) {
	std::cout << a << " " << b << "\n";
	a = "azaza";
}

int main()
{
	char* q = "kek";
	//string_print("lllll", q);
	auto w = bind(string_print, "llll", _2);
	w(1, string("keke"));
	w(2, string("lol"));
	return 0;
}