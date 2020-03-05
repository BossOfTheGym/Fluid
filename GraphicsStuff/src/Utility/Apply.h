#pragma once

namespace detail
{
	template<class Callable, class ... Dummies>
	struct Apply;

	template<class Callable, class Dummy, class ... Dummies>
	struct Apply<Callable, Dummy, Dummies...>
	{
		static void apply(Callable&& callable, Dummy&& dummy, Dummies&& ... dummies)
		{
			callable(std::forward<Dummy>(dummy));

			Apply<Callable, Dummies...>::apply(
				std::forward<Callable>(callable)
				, std::forward<Dummies>(dummies)...
			);
		}
	};

	template<class Callable, class Dummy>
	struct Apply<Callable, Dummy>
	{
		static void apply(Callable&& callable, Dummy&& dummy)
		{
			callable(std::forward<Dummy>(dummy));
		}
	};
}

template<class Callable, class ... Dummies>
void apply(Callable&& callable, Dummies&& ... dummies)
{
	detail::Apply<Callable, Dummies...>::apply(
		std::forward<Callable>(callable)
		, std::forward<Dummies>(dummies)...
	);
}
