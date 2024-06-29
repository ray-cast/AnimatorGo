#ifndef OCTOON_MATH_TRAIT_H_
#define OCTOON_MATH_TRAIT_H_

#include <string>

namespace octoon
{
	namespace math
	{
		namespace trait
		{
			template<typename T>
			struct type_addition
			{
				typedef T           value_type;
				typedef T*			pointer;
				typedef T&			reference;
				typedef const T		const_type;
				typedef const T*	const_pointer;
				typedef const T&	const_reference;
			};

			template<>
			struct type_addition<void>
			{
				typedef void        value_type;
				typedef void*		pointer;
				typedef const void*	const_pointer;
			};

			template<typename T>
			struct type_addition<T*>
			{
				typedef T			value_type;
				typedef T*			pointer;
				typedef T&			reference;
				typedef const T*	const_type;
				typedef const T*	const_pointer;
				typedef const T&	const_reference;
			};

			template<typename T>
			struct type_addition<T&>
			{
				typedef T			value_type;
				typedef T*			pointer;
				typedef T&			reference;
				typedef const T&	const_type;
				typedef const T*	const_pointer;
				typedef const T&	const_reference;
			};

			template<typename T>
			struct type_addition<const T*>
			{
				typedef T			value_type;
				typedef T*			pointer;
				typedef T&			reference;
				typedef const T*	const_type;
				typedef const T*	const_pointer;
				typedef const T&	const_reference;
			};

			template<class T, class U>
			struct _Has_left_shift
			{
				template<class _Tx, class _Ty>
				static auto _test(int) -> decltype(std::declval<_Tx>() << std::declval<_Ty>(), std::true_type());

				template<class _Tx, class _Ty>
				static auto _test(...)->std::false_type;

				typedef decltype(_test<T, U>(0)) type_t;
			};

			template<class T>
			struct _Has_left_shift<T, void>
			{
				template<class _T>
				static auto _test(int) -> decltype(std::declval<_T>() << std::declval<_T>(), std::true_type());

				template<class _T>
				static auto _test(...)->std::false_type;

				typedef decltype(_test<T>(0)) type_t;
			};

			template<class T, class U = void>
			struct has_left_shift : _Has_left_shift<T, U>::type_t
			{
			};

			template<class T>
			struct has_left_shift<T, void> : _Has_left_shift<T, void>::type_t
			{
			};

			template<class T, class U>
			struct _Has_right_shift
			{
				template<class _Tx, class _Ty>
				static auto _test(int) -> decltype(std::declval<_Tx>() >> std::declval<_Ty>(), std::true_type());

				template<class _Tx, class _Ty>
				static auto _test(...)->std::false_type;

				typedef decltype(_test<T, U>(0)) type_t;
			};

			template<class T>
			struct _Has_right_shift<T, void>
			{
				template<class _T>
				static auto _test(int) -> decltype(std::declval<_T>() >> std::declval<_T>(), std::true_type());

				template<class _T>
				static auto _test(...)->std::false_type;

				typedef decltype(_test<T>(0)) type_t;
			};

			template<class T, class U = void>
			struct has_right_shift : _Has_right_shift<T, U>::type_t
			{
			};

			template<class T>
			struct has_right_shift<T, void> : _Has_right_shift<T, void>::type_t
			{
			};

			template<class _Ty> constexpr bool is_empty_v = std::is_empty<_Ty>::value;
			template<class _Ty> constexpr bool is_integral_v = std::is_integral<_Ty>::value;
			template<class _Ty> constexpr bool is_floating_point_v = std::is_floating_point<_Ty>::value;

			template<class _Tx, class _Ty> constexpr bool is_same_v = std::is_same<_Tx, _Ty>::value;
		}
	}
}

#endif