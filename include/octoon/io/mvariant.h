#ifndef OCTOON_VARIANT_H
#define OCTOON_VARIANT_H

#include <string>
#include <cstddef>

namespace octoon
{
	// MaxSizeType
	template <typename T, typename ...TS> struct MaxSizeType;

	template <typename T, typename U>
	struct MaxSizeType<T, U>
	{
		static constexpr std::size_t size = sizeof(T) > sizeof(U) ? sizeof(T) : sizeof(U);
	};

	template <typename T, typename ...TS>
	struct MaxSizeType<T, TS...>
	{
		static constexpr std::size_t size = sizeof(T) > MaxSizeType<TS...>::size ? sizeof(T) : MaxSizeType<TS...>::size;
	};

	// TypeExist
	template <typename U, typename T, typename ...TS> struct TypeExist;

	template <typename U, typename T>
	struct TypeExist<U, T>
	{
		static constexpr bool value = std::is_same<U, T>::value;
	};

	template <typename U, typename T, typename ...TS>
	struct TypeExist<U, T, TS...>
	{
		static constexpr bool value = std::is_same<U, T>::value || TypeExist<U, TS...>::value;
	};

	// Destroy
	template <typename T, typename ...TS> struct Destroy;

	template <typename T>
	struct Destroy<T>
	{
		static constexpr void destroy(std::size_t hash_id, unsigned char *data) noexcept
		{
			if (typeid(T).hash_code() == hash_id)
				reinterpret_cast<T*>(data)->~T();
		}
	};

	template <typename T, typename ...TS>
	struct Destroy<T, TS...>
	{
		static constexpr void destroy(std::size_t hash_id, unsigned char *data) noexcept
		{
			if (typeid(T).hash_code() == hash_id)
				reinterpret_cast<T*>(data)->~T();
			else
				Destroy<TS...>::destroy(hash_id, data);
		}
	};

	// variant
	template<class ...TArgs>
	class variant
	{
		using byte = unsigned char;
	public:
		constexpr variant(void)
		{
			construct<TArgs...>();
		}

		~variant()
		{
			destroy();
		}
	private:
		template <typename T, typename ...Args>
		constexpr void construct(void)
		{
			new(data)T;
			hash_id = typeid(T).hash_code();
		}

		constexpr void destroy(void) noexcept
		{
			Destroy<TArgs...>::destroy(hash_id, data);
		}
	public:
		std::size_t hash_id;
		unsigned char data[MaxSizeType<TArgs...>::size] = { 0 };
	};
}

#endif // OCTOON_VARIANT_H
