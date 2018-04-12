#ifndef OCTOON_VARIANT_H
#define OCTOON_VARIANT_H

#include <string>
#include <iostream>
#include <utility>
#include <exception>
#include <variant>
#include <cstddef>

namespace octoon
{
	// exception
	class bad_variant_access : public std::exception
	{

	};

	template <typename ...TArgs>
	class variant;

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
		static constexpr bool is_exist = std::is_same<U, T>::value;
		static constexpr std::size_t index = 0;
	};

	template <typename U, typename T, typename ...TS>
	struct TypeExist<U, T, TS...>
	{
		static constexpr bool is_exist = std::is_same<U, T>::value ? true : TypeExist<U, TS...>::is_exist;
		static constexpr std::size_t index = std::is_same<U, T>::value ? 0 : TypeExist<U, TS...>::index + 1;
	};

	// FindTypeByIndex
	template <std::size_t I, std::size_t idx, bool equal, typename T, typename ...TS> struct FindTypeByIndexImpl;

	template <std::size_t I, std::size_t idx, bool equal, typename T>
	struct FindTypeByIndexImpl<I, idx, equal, T>
	{
		using type = T;
	};

	template <std::size_t I, std::size_t idx, typename T, typename ...TS>
	struct FindTypeByIndexImpl<I, idx, true, T, TS...>
	{
		using type = T;
	};

	template <std::size_t I, std::size_t idx, typename T, typename ...TS>
	struct FindTypeByIndexImpl<I, idx, false, T, TS...>
	{
		using type = typename FindTypeByIndexImpl <I, idx + 1, I == idx + 1, TS...>::type;
	};

	template <std::size_t I, typename ...TS>
	struct FindTypeByIndex
	{
		using type = typename FindTypeByIndexImpl<I, 0, I == 0, TS...>::type;
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

	// dynamic type op
	template<typename T, typename ...Types> struct DynamicTypeOp;

	template<typename T>
	struct DynamicTypeOp<T>
	{
		template <typename ...TArgs>
		static void assign(variant<TArgs...>& lhs, const variant<TArgs...>& rhs)
		{
			return;
		}

		template <typename ...TArgs>
		static bool nothrow_copy_constructible_or_not_nothrow_move_constructible(const variant<TArgs...>& rhs)
		{
			return false;
		}
	};

	template<typename T, typename ...Types>
	struct DynamicTypeOp
	{
		template <typename ...TArgs>
		static void assign(variant<TArgs...>& lhs, const variant<TArgs...>& rhs)
		{
			if (typeid(T).hash_code() == lhs.hash_id)
			{
				lhs.get<T>() = rhs.get<T>();
			}
			else
			{
				DynamicTypeOp<Types...>::assign(lhs, rhs);
			}
		}

		template <typename ...TArgs>
		static bool nothrow_copy_constructible_or_not_nothrow_move_constructible(const variant<TArgs...>& rhs)
		{
			if (typeid(T).hash_code() == rhs.hash_id)
			{
				if (std::is_nothrow_copy_constructible<T>::value || std::is_nothrow_move_constructible<T>::value)
					return true;
				else
					return false;
			}
			else
			{
				return DynamicTypeOp<Types...>::nothrow_copy_constructible_or_not_nothrow_move_constructible(rhs);
			}
		}
	};
	

	//variant_alternative
	template <size_t I, class... Types>
	struct variant_alternative;

	template <size_t I, class... Types>
	struct variant_alternative<I, variant<Types...>>
	{
		using type = typename FindTypeByIndex<I, Types...>::type;
	};
	template <size_t I, class T> class variant_alternative<I, const T>
	{
		using type = std::size_t;
	};
	template <size_t I, class T> class variant_alternative<I, volatile T>
	{
		using type = std::size_t;
	};
	template <size_t I, class T> class variant_alternative<I, const volatile T>
	{
		using type = std::size_t;
	};

	template <size_t I, class T>
	using variant_alternative_t = typename variant_alternative<I, T>::type;

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

		variant& operator=(const variant& rhs)
		{
			if (valueless_by_exception() && rhs.valueless_by_exception()) return *this;
			if (!valueless_by_exception() && rhs.valueless_by_exception())
			{
				destroy();
				is_valueless = true;
			}
			else if (rhs.hash_id == hash_id)
			{
				DynamicTypeOp<TArgs...>::assign(*this, rhs);
			}
			else if (DynamicTypeOp<TArgs...>::nothrow_copy_constructible_or_not_nothrow_move_constructible(rhs))
			{
				//this->emplace<rhs.index()>(get<rhs.index()>(rhs));
			}
			else
			{
				//this->operator=(variant(rhs));
			}
			return *this;
		}

		variant& operator=(variant& rhs)
		{
			if (valueless_by_exception() && rhs.valueless_by_exception()) return *this;
			if (!valueless_by_exception() && rhs.valueless_by_exception())
			{
				destroy();
				is_valueless = true;
			}
			else if (rhs.hash_id == hash_id)
			{
				DynamicTypeOp<TArgs...>::assign(*this, rhs);
			}
			else if (DynamicTypeOp<TArgs...>::nothrow_copy_constructible_or_not_nothrow_move_constructible(rhs))
			{
				//this->emplace<rhs.index()>(get<rhs.index()>(rhs));
			}
			else
			{
				//this->operator=(variant(rhs));
			}
			return *this;
		}

		variant& operator=(variant&& rhs) noexcept
		{

		}

		template <class T> variant& operator=(T&& t) noexcept
		{
			if (typeid(std::remove_reference<T>::type).hash_code() == hash_id)
			{
				*reinterpret_cast<typename std::remove_reference<T>::type *>(data) = std::forward<T>(t);
			}
			else
			{
				is_valueless = false;
				hash_id = typeid(std::remove_reference<T>::type).hash_code();
				index_id = TypeExist<std::remove_reference<T>::type, TArgs...>::index;
				*reinterpret_cast<typename std::remove_reference<T>::type *>(data) = std::forward<T>(t);
			}
			return *this;
		}

		constexpr std::size_t index() const noexcept { return index_id; }

		constexpr bool valueless_by_exception() const noexcept { return is_valueless; }

		void swap(variant& rhs) noexcept
		{
			if (valueless_by_exception() && rhs.valueless_by_exception()) return;
			if (hash_id == rhs.hash_id)
			{
				//std::swap(octoon::get<typename FindTypeByIndex<I, Types...>::type>(*this),octoon:get<typename FindTypeByIndex<I, Types...>::type>(rhs));
			}
			else
			{
				std::size_t max_size = MaxSizeType<TArgs...>::size;
				unsigned char tmp[MaxSizeType<TArgs...>::size] = { 0 };
				std::swap(is_valueless, rhs.is_valueless);
				std::swap(hash_id, rhs.hash_id);
				std::swap(index_id, rhs.index_id);
				std::copy(data, data + max_size, tmp);
				std::copy(rhs.data, rhs.data + max_size, data);
				std::copy(tmp, tmp + max_size, rhs.data);
			}
		}

		
	private:
		template <typename T, typename ...Args>
		constexpr void construct(void)
		{
			new(data)T;
			is_valueless = true;
			hash_id = typeid(T).hash_code();
			index_id = 0;
		}

		constexpr void destroy(void) noexcept
		{
			Destroy<TArgs...>::destroy(hash_id, data);
		}

		template <typename T>
		T& get(void)
		{
			return *reinterpret_cast<T *>((void *)data);
		}

		template <typename T>
		const T& get(void) const
		{
			return *reinterpret_cast<T *>((void *)data);
		}
		
	private:
		bool is_valueless;
		std::size_t hash_id;
		std::size_t index_id;
		unsigned char data[MaxSizeType<TArgs...>::size] = { 0 };


		// friend get
		template <std::size_t I, class... Types>
		friend constexpr variant_alternative_t<
			I, variant<Types...>
		>& get(variant<Types...>& v);

		template <std::size_t I, class... Types>
		friend constexpr variant_alternative_t<
			I, variant<Types...>
		>&& get(variant<Types...>&& v);

		template <std::size_t I, class... Types>
		friend constexpr variant_alternative_t<
			I, variant<Types...>
		> const& get(const variant<Types...>& v);

		template <std::size_t I, class... Types>
		friend constexpr variant_alternative_t<
			I, variant<Types...>
		>&& get(variant<Types...>&& v);

		template <class T, class... Types>
		friend constexpr T& get(variant<Types...>& v);
		template <class T, class... Types>
		friend constexpr T&& get(variant<Types...>&& v);
		template <class T, class... Types>
		friend constexpr const T& get(const variant<Types...>& v);
		template <class T, class... Types>
		friend constexpr const T&& get(const variant<Types...>&& v);

		// friend class
		template<typename T, typename ...Types> friend struct DynamicTypeOp;
	};

	// get
	template <std::size_t I, class... Types>
	constexpr variant_alternative_t<
		I, variant<Types...>
	>& get(variant<Types...>& v)
	{
		if (I == v.index_id)
			return *reinterpret_cast<typename FindTypeByIndex<I, Types...>::type *>(v.data);
		else
			throw bad_variant_access();
	}

	template <std::size_t I, class... Types>
	constexpr variant_alternative_t<
		I, variant<Types...>
	>&& get(variant<Types...>&& v)
	{
		if (I == v.index_id)
			return *reinterpret_cast<typename FindTypeByIndex<I, Types...>::type *>(v.data);
		else
			throw bad_variant_access();
	}

	template <std::size_t I, class... Types>
	constexpr variant_alternative_t<
		I, variant<Types...>
	> const& get(const std::variant<Types...>& v)
	{
		if (I == v.index_id)
			return *reinterpret_cast<typename FindTypeByIndex<I, Types...>::type *>(v.data);
		else
			throw bad_variant_access();
	}

	template <std::size_t I, class... Types>
	constexpr variant_alternative_t<
		I, variant<Types...>
	> const&& get(const variant<Types...>&& v)
	{
		if (I == v.index_id)
			return *reinterpret_cast<typename FindTypeByIndex<I, Types...>::type *>(v.data);
		else
			throw bad_variant_access();
	}

	template <class T, class... Types>
	constexpr T& get(variant<Types...>& v)
	{
		if (TypeExist<T, Types...>::is_exist && TypeExist<T, Types...>::index == v.index_id)
			return *reinterpret_cast<T*>(v.data);
		else
			throw bad_variant_access();
	}
	template <class T, class... Types>
	constexpr T&& get(variant<Types...>&& v)
	{
		if (TypeExist<T, Types...>::is_exist && TypeExist<T, Types...>::index == v.index_id)
			return *reinterpret_cast<T*>(v.data);
		else
			throw bad_variant_access();
	}
	template <class T, class... Types>
	constexpr const T& get(const variant<Types...>& v)
	{
		if (TypeExist<T, Types...>::is_exist && TypeExist<T, Types...>::index == v.index_id)
			return *reinterpret_cast<T*>(v.data);
		else
			throw bad_variant_access();
	}
	template <class T, class... Types>
	constexpr const T&& get(const variant<Types...>&& v)
	{
		if (TypeExist<T, Types...>::is_exist && TypeExist<T, Types...>::index == v.index_id)
			return *reinterpret_cast<T*>(v.data);
		else
			throw bad_variant_access();
	}
}

#endif // OCTOON_VARIANT_H
