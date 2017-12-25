#include <iostream>
#include <string>
#include <iostream>
#include <utility>
#include <tuple>
#include "bind.h"
using std::string;
using std::cin;
using std::cout;


template<typename Seq>
struct printer;

template<int X, int ... tail>
struct printer<integer_sequence<int, X, tail...>>
{
	static void f() {
		cout << X << " ";
		printer<integer_sequence<int, tail...>>::f();
	}
};

template<>
struct printer<integer_sequence<int>>
{
	static void f() {
		cout << "kek";
	}
};

void f(string a, string b, string c, string& d) {
	cout << a << " " << b << " " << c << " " << d << "\n";
	d = "AZAZAA";
}

template<typename T, typename ... Q>
struct my_type;

int main()
{
	//printer<typename many_types<bind_t<int, bind_t<char, placeholder<5>, placeholder<1>, double>, char, placeholder<4>>, int, placeholder<2>, double, placeholder<2>, placeholder<3> >::value>::f();
	string s = "lol";
	string a = "kek";
	string b = "wow";
	auto w = bind(f, _1, _1, _2, _3);
	w(std::move(a), std::move(b), s);
	cout << a << "!" << b << "\n";
	return 0;
}