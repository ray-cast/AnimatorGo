#ifndef OCTOON_VARIANT_H_
#define OCTOON_VARIANT_H_

#include "variant/variant.hpp"

namespace octoon
{
	namespace runtime
	{
		template<typename ...T>
		using variant = mpark::variant<T...>;
	}
}

namespace std
{
	template <std::size_t I, typename... Ts>
	inline constexpr mpark::variant_alternative_t<I, mpark::variant<Ts...>>& get(mpark::variant<Ts...> &v)
	{
		return mpark::detail::generic_get<I>(v);
	}

	template <std::size_t I, typename... Ts>
	inline constexpr mpark::variant_alternative_t<I, mpark::variant<Ts...>> &&get(mpark::variant<Ts...> &&v)
	{
		return mpark::detail::generic_get<I>(mpark::lib::move(v));
	}

	template <std::size_t I, typename... Ts>
	inline constexpr const mpark::variant_alternative_t<I, mpark::variant<Ts...>> &get(const mpark::variant<Ts...> &v)
	{
		return mpark::detail::generic_get<I>(v);
	}

	template <std::size_t I, typename... Ts>
	inline constexpr const mpark::variant_alternative_t<I, mpark::variant<Ts...>> &&get(const mpark::variant<Ts...> &&v)
	{
		return mpark::detail::generic_get<I>(mpark::lib::move(v));
	}

	template <typename T, typename... Ts>
	inline constexpr T& get(mpark::variant<Ts...>& v)
	{
		return mpark::get<mpark::detail::find_index_checked<T, Ts...>::value>(v);
	}

	template <typename T, typename... Ts>
	inline constexpr T&& get(mpark::variant<Ts...>&& v)
	{
		return mpark::get<mpark::detail::find_index_checked<T, Ts...>::value>(mpark::lib::move(v));
	}

	template <typename T, typename... Ts>
	inline constexpr const T& get(const mpark::variant<Ts...>& v)
	{
		return mpark::get<mpark::detail::find_index_checked<T, Ts...>::value>(v);
	}

	template <typename T, typename... Ts>
	inline constexpr const T&& get(const mpark::variant<Ts...>&& v)
	{
		return mpark::get<mpark::detail::find_index_checked<T, Ts...>::value>(mpark::lib::move(v));
	}
}

#endif