#include <utility>
#include <tuple>
#include <type_traits>
using std::forward;
using std::get;
using std::move;
using std::tuple;
using std::remove_reference_t;
using std::remove_const_t;
using std::enable_if_t;
using std::tuple_element_t;


template <typename T, T ... values>
struct integer_sequence
{};

template <int N>
struct placeholder
{};

template <typename F, typename ... As>
struct bind_t;

template <typename T, typename Seq, T arg>
struct append;

template <typename T, T ... values, T arg>
struct append<T, integer_sequence<T, values...>, arg>
{
	typedef integer_sequence<T, values..., arg> type;
};

template <typename T, T N, typename Q = void>
struct make_integer_sequence
{
	typedef typename append<T, typename make_integer_sequence<T, N - 1>::type, N - 1>::type type;
};

template <typename T, T N>
struct make_integer_sequence<T, N, enable_if_t<N == 0>>
{
	typedef integer_sequence<T> type;
};

template<int N>
using int_sequence = typename make_integer_sequence<int, N>::type;

template<typename S1, typename S2>
struct combine;

template<int ... seq1, int ... seq2>
struct combine<integer_sequence<int, seq1...>, integer_sequence<int, seq2...>>
{
	typedef integer_sequence<int, seq1..., seq2...> value;
};

template<typename T>
struct array_killer
{
	typedef T value;
};

template<typename T, int N>
struct array_killer<const T (&)[N]>
{
	typedef const T* value;
};

template <typename T>
using array_killer_t = typename array_killer<T>::value;

template<typename ... TAIL>
struct many_types;

template<typename T>
struct single_type {
	typedef int_sequence<0> value;
};

template<int N>
struct single_type<placeholder<N>>
{
	typedef integer_sequence<int, N> value;
};

template <typename F, typename ... As>
struct single_type<bind_t<F, As...>>
{
	typedef typename many_types<remove_const_t<remove_reference_t<As>>...>::value value;
};

template<typename T, typename ... TAIL>
struct many_types<T, TAIL...>
{
	typedef typename combine<typename single_type<T>::value, typename many_types<TAIL...>::value>::value value;
};

template<>
struct many_types<>
{
	typedef int_sequence<0> value;
};

template<typename ... T>
using many_types_t = typename many_types<T...>::value;

template<int X, typename Seq>
struct get_cnt;

template<int X, int N, int ... seq>
struct get_cnt<X, integer_sequence<int, N, seq...>>
{
	static constexpr int cnt = static_cast<int>(X == N) + get_cnt<X, integer_sequence<int, seq...>>::cnt;
};

template<int X>
struct get_cnt<X, integer_sequence<int>>
{
	static constexpr int cnt = 0;
};

template<int X, typename Seq>
constexpr bool good = get_cnt<X, Seq>::cnt <= 1;

template<typename T, bool f>
struct get_type
{
	static T g(T a);
};

template<typename T>
struct get_type<T, 1>
{
	static auto g(T a) -> decltype(std::forward<T>(a));
};

template<typename T, bool f>
auto get_type_t(T a) -> decltype(get_type<T, f>::g(a));


template<typename T>
struct my_forwarder
{
	static constexpr T&& forward(T& t) noexcept {
		return static_cast<T&&>(t);
	}
	//static constexpr T&& forward(T&& t) noexcept {
	//	return static_cast<T&&>(t);
	//}
};

template<int N>
struct my_forwarder<const placeholder<N>&>
{
	static constexpr placeholder<N> forward(placeholder<N> t) noexcept {
		return t;
	}
};

template<int N>
struct my_forwarder<placeholder<N>&>
{
	static constexpr placeholder<N> forward(placeholder<N> t) noexcept {
		return t;
	}
};

template<int N>
struct my_forwarder<placeholder<N>>
{
	static constexpr placeholder<N> forward(placeholder<N> t) noexcept {
		return t;
	}
};

template<int N>
struct my_forwarder<placeholder<N>&&>
{
	static constexpr placeholder<N> forward(placeholder<N> t) noexcept {
		return t;
	}
};

template<typename T>
auto my_forward(T& t) -> decltype(my_forwarder<T>::forward(t)) {
	return my_forwarder<T>::forward(t);
}

//template<typename T>
//auto my_forward(T&& t) -> decltype(my_forwarder<T>::forward(t)) {
//	return my_forwarder<T>::forward(t);
//}

template<typename T>
struct cleaner
{
	typedef T type;
};

template<int N>
struct cleaner<const placeholder<N>&>
{
	typedef placeholder<N> type;
};

template<int N>
struct cleaner<placeholder<N>&>
{
	typedef placeholder<N> type;
};

template<int N>
struct cleaner<placeholder<N>&&>
{
	typedef placeholder<N> type;
};

template<typename T>
using cleaner_t = typename cleaner<T>::type;

template<typename T>
struct bind_cleaner
{
	typedef T value;
};

template<typename F, typename ... As>
struct bind_cleaner<bind_t<F, As...>&&>
{
	typedef bind_t<F, As...> value;
};

template<typename F, typename ... As>
struct bind_cleaner<bind_t<F, As...>&>
{
	typedef bind_t<F, As...> value;
};

template<typename F, typename ... As>
struct bind_cleaner<const bind_t<F, As...>&>
{
	typedef bind_t<F, As...> value;
};

template<typename T>
using bind_cleaner_t = typename bind_cleaner<T>::value;

constexpr placeholder<1> _1;
constexpr placeholder<2> _2;
constexpr placeholder<3> _3;

template <typename A>
struct G
{
	G(A&& a) : a(forward<A>(a)) {}

	template <typename... Bs>
	A operator()(Bs&&...)
	{
		return static_cast<A>(a);
	}
private:
	remove_reference_t<A> a;
};

template <int N>
struct G<placeholder<N>>
{
	G(placeholder<N>) {}

	template <typename B, typename ... Bs>
	decltype(auto) operator()(B&& t, Bs&& ... bs)
	{
		G<placeholder<N - 1>> next{ placeholder<N - 1>() };
		return next(forward<Bs>(bs)...);
	}
};

template <>
struct G<placeholder<1>>
{
	G(placeholder<1>) {}

	template <typename B1, typename ... Bs>
	decltype(auto) operator()(B1&& b1, Bs&&...)
	{
		return std::forward<B1>(b1);
	}
};

template <typename F, typename ... As>
struct G<bind_t<F, As...>>
{
	G(bind_t<F, As...> fun): fun(move(fun)) {}

	template <typename ... Bs>
	decltype(auto) operator()(Bs&& ... bs)
	{
		return fun(forward<Bs>(bs)...);
	}
private:
	bind_t<F, As...> fun;
};

template <typename F, typename ... As>
struct bind_t
{
	bind_t(F f, As&& ... as)
		: f(f)
		, gs(forward<As>(as)...)
	{}

	template <typename ... Bs>
	decltype(auto) operator()(Bs&& ... bs) 
	{
		return call(int_sequence<sizeof...(As)>(), int_sequence<sizeof...(Bs)>(), many_types_t<As...>(),  forward<Bs>(bs)...);
	}
private:
	template <int ... ks, int ... inds, int ... holds, typename ... Bs>
	decltype(auto) call(integer_sequence<int, ks...> a, integer_sequence<int, inds...> b, integer_sequence<int, holds...> c,  Bs&& ... bs)
	{
		tuple<decltype(get_type_t<Bs, good<inds + 1, decltype(c)>>(bs))...> args(forward<Bs>(bs)...);
		return f(get<ks>(gs)(get_arg< inds, decltype(get_type_t<Bs, good<inds + 1, decltype(c)>>(bs))... >(args)...)...);
	}

	template<int ind, typename ... Ts>
	decltype(auto) get_arg(tuple<Ts...>& source) {
		return static_cast<tuple_element_t<ind, tuple<Ts...>>>(get<ind>(source));
	}

	F f;
	tuple<G<bind_cleaner_t<As>>...> gs;
};

template <typename F, typename ... As>
decltype(auto) bind(F f, As&& ... as)
{
	return bind_t<F, cleaner_t<array_killer_t<As>>...>(f, my_forward<As>(as)...);
}

template <typename F, typename ... As>
decltype(auto) call_once_bind(F f, As&& ... as) 
{
	return bind_t<F, cleaner_t<array_killer_t<As>&&>...>(f, my_forward<As>(as)...);
}
