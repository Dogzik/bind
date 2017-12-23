#include <utility>
#include <tuple>

template <typename T, T ... values>
struct integer_sequence
{};

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
struct make_integer_sequence<T, N, typename std::enable_if_t<N == 0>>
{
	typedef integer_sequence<T> type;
};

template<int N>
using int_sequence = typename make_integer_sequence<int, N>::type;

template <int N>
struct placeholder
{};

template <typename F, typename ... As>
struct bind_t;

constexpr placeholder<1> _1;
constexpr placeholder<2> _2;
constexpr placeholder<3> _3;

template <typename A>
struct G
{
	G(A&& a) : a(std::forward<A>(a)) {}

	template <typename... Bs>
	A operator()(Bs&&...) const
	{
		return a;
	}

private:
	A a;
};

template <int N>
struct G<const placeholder<N>&>
{
	G(placeholder<N>) {}

	template <typename B, typename ... Bs>
	decltype(auto) operator()(B, Bs ... bs) const
	{
		G<placeholder<N - 1>> next{ placeholder<N - 1>() };
		return next(bs...);
	}
};

template <>
struct G<const placeholder<1>&>
{
	G(placeholder<1>) {}

	template <typename B1, typename ... Bs>
	B1 operator()(B1 b1, Bs...) const
	{
		return b1;
	}
};

template <typename F, typename ... As>
struct G<bind_t<F, As...>>
{
	G(bind_t<F, As...>&& fun): fun(fun) {}

	template <typename... Bs>
	decltype(auto) operator()(Bs... bs) const
	{
		return fun(bs...);
	}

private:
	bind_t<F, As...> fun;
};

template <typename F, typename ... As>
struct G<bind_t<F, As...>&&>
{
	G(bind_t<F, As...>&& fun): fun(fun) {}

	template <typename ... Bs>
	decltype(auto) operator()(Bs ... bs) const
	{
		return fun(bs...);
	}

private:
	bind_t<F, As...> fun;
};

template <typename F, typename ... As>
struct bind_t
{
	bind_t(F f, As&& ... as)
		: f(f)
		, gs(std::forward<As>(as)...)
	{}

	template <typename ... Bs>
	decltype(auto) operator()(Bs&& ... bs) const
	{
		return call(int_sequence<sizeof...(As)>(), bs...);
	}

private:
	template <int... ks, typename ... Bs>
	decltype(auto) call(integer_sequence<int, ks...>, Bs&&... bs) const
	{
		return f(std::get<ks>(gs)(bs...)...);
	}

private:
	F f;
	std::tuple<G<As>...> gs;
};

template <typename F, typename ... As>
bind_t<F, As...> bind(F f, As&& ... as)
{
	return bind_t<F, As...>(f, std::forward<As>(as)...);
}

template <typename F, typename ... As>
bind_t<F, As...> call_once_bind(F f, As&& ... as)
{
	return bind_t<F, decltype(std::forward<As>(as))...>(f, std::forward<As>(as)...);
}