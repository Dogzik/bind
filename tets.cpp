#include <iostream>
#include <string>
#include <iostream>
#include <utility>
#include <tuple>
#include "bind.h"
using std::string;
using std::cin;
using std::cout;

void f(string a, string b, string c, string& d) {
	cout << a << " " << b << " " << c << " " << d << "\n";
	//d = "AZAZAA";
}

void ff(int a, int b, int c, int&& d) {
	cout << a << " " << b << " " << c << " " << d << "\n";
	//d = "AZAZAA";
}

string concat(string x, string y) {
	return x + y;
}

int add(int x, int y) {
	return x + y;
}

int main()
{
	//string s = "lol";
	//string a = "kek";
	//string b = "wow";
	//auto w = bind(f, "YOU", placeholder<2>(), bind(concat, string("TROLL+"), _3), _3);
	////w(std::move(a), std::move(b), s);
	//w("a", "b", std::move(s));
	//cout << a << "!" << b << "\n";

	auto ww = bind(ff, 1, placeholder<2>(), bind(add, 7, _1), _3);
	ww(10, 11, 12);
	return 0;
}