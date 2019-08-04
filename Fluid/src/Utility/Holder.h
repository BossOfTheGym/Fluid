#ifndef HOLDER_H
#define HOLDER_H

template<class ... Callables>
struct Holder : public Callables...
{
	Holder(Callables&& ... callables) : Callables(std::forward<Callables>(callables))...
	{}

	using Callables::operator()...;
};

template<class ... Callables>
auto make_holder(Callables&& ... callables)
{
	return Holder<Callables...>(std::forward<Callables>(callables)...);
}

#endif