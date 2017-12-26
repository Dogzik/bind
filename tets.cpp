#include <iostream>
#include <string>
#include <iostream>
#include <utility>
#include <tuple>
#include "bind.h"
using std::string;
using std::cin;
using std::cout;

void f(string a, string b, string c, string d) {
	cout << a << " " << b << " " << c << " " << d << "\n";
	d = "AZAZAA";
}

string concat(string x, string y) {
	return x + y;
}

void g(string s) {
	cout << s << "\n";
}

int main()
{
	//auto ww = call_once_bind(f, "keke", "lol", placeholder<1>(), "aza");
	//ww("saaa");
	string s = "lol";
	string a = "kek";
	string b = "wow";
	auto w = call_once_bind(f, "YOU", placeholder<2>(), bind(concat, string("TROLL+"), _1), _3);
	w(std::move(a), std::move(b), s);
	cout << a << "!" << b << "\n";
	return 0;
}