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
			struct functor {};

			template<typename Result>
			struct functor<Result(*)()>
			{
				typedef Result result_t;
				typedef void arg_t;
			};

			template<typename Result, typename Args>
			struct functor<Result(*)(Args)>
			{
				typedef Result result_t;
				typedef Args arg_t;
			};

			template<typename Result, typename Class>
			struct functor<Result(Class::*)()>
			{
				typedef Result result_t;
				typedef Class class_t;
				typedef void arg_t;
			};

			template<typename Result, typename Class, typename Args>
			struct functor<Result(Class::*)(Args)>
			{
				typedef Result result_t;
				typedef Class class_t;
				typedef Args arg_t;
			};

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

			template<typename _Elem, typename _Int_type>
			struct _Char_traits
			{
				typedef _Elem			char_type;
				typedef _Elem*			pointer;
				typedef _Elem&			reference;
				typedef const _Elem		const_type;
				typedef const _Elem*	const_pointer;
				typedef const _Elem&	const_reference;
				typedef _Int_type		int_type;

				static int compare(const char_type* str1, const char_type* str2, std::size_t cnt) noexcept
				{
					for (; 0 < cnt; --cnt, ++str1, ++str2)
					{
						if (!eq(*str1, *str2))
							return (lt(*str1, *str2) ? -1 : 1);
					}

					return (0);
				}

				static bool eq(const char_type& lhs, const char_type& rhs) noexcept
				{
					return (lhs == rhs);
				}

				static bool lt(const char_type& lhs, const char_type& rhs) noexcept
				{
					return (lhs < rhs);
				}
			};

			template<typename T>
			struct erase_const
			{
				typedef T value_type;
			};

			template<typename T>
			struct erase_const<const T>
			{
				typedef T value_type;
			};

			template<typename T>
			struct erase_const<const T*>
			{
				typedef T* value_type;
			};

			template<typename T>
			struct erase_const<const T&>
			{
				typedef T& value_type;
			};

			template<typename _Ty>
			class char_traits;

			template<>
			class char_traits<char> : public _Char_traits<char, long>
			{
			public:
				static const std::string towcs(const wchar_t* str) noexcept
				{
					std::string _format;
					_format.resize(wcslen(str));
					wcstombs((char*)_format.c_str(), str, _format.size());
					return _format;
				}

				static const_pointer towcs(const char* str) noexcept
				{
					return (str);
				}

				static pointer get_pointer(pointer name) noexcept
				{
					return name;
				}

				static const_pointer get_pointer(const_pointer name) noexcept
				{
					return name;
				}

				static void set_pointer(const_pointer*& str, const_pointer* value) noexcept
				{
					str = value;
				}

				static bool empty(const_pointer* str)
				{
					return nullptr == str;
				}
			};

			template<>
			class char_traits<wchar_t> : public _Char_traits<wchar_t, long>
			{
			public:
				static const std::wstring towcs(const char* str) noexcept
				{
					std::wstring _format;
					_format.resize(strlen(str));
					mbstowcs((wchar_t*)_format.c_str(), str, _format.size());
					return _format;
				}

				static const_pointer towcs(const wchar_t* str) noexcept
				{
					return str;
				}

				static pointer get_pointer(pointer str) noexcept
				{
					return str;
				}

				static const_pointer get_pointer(const_pointer str) noexcept
				{
					return str;
				}

				static void set_pointer(const_pointer*& str, const_pointer* value) noexcept
				{
					str = value;
				}

				static bool empty(const_pointer* str)
				{
					return nullptr == str;
				}
			};

			template<>
			class char_traits<std::string> : public _Char_traits<char, long>
			{
			public:
				static const_pointer get_pointer(const std::string& name) noexcept
				{
					return name.c_str();
				}

				static void set_pointer(std::string& str, const_pointer value) noexcept
				{
					if (nullptr == value)
						str.clear();
					else
						str = value;
				}

				static void set_poineter(std::string& s1, const std::string& s2) noexcept
				{
					s1 = s2;
				}

				static bool empty(const std::string& str)
				{
					return str.empty();
				}
			};

			template<>
			class char_traits<std::wstring> : public _Char_traits<wchar_t, long>
			{
			public:
				static const_pointer get_pointer(const std::wstring& str) noexcept
				{
					return str.c_str();
				}

				static void set_pointer(std::wstring& str, const_pointer value) noexcept
				{
					if (nullptr == value)
						str.clear();
					else
						str = value;
				}

				static void set_poineter(std::wstring& s1, const std::wstring& s2) noexcept
				{
					s1 = s2;
				}

				static bool empty(const std::wstring& str)
				{
					return str.empty();
				}
			};

			template<class T, class U>
			struct _Has_left_shift
			{
				template<class _Tx, class _Ty>
				static auto _test(int) -> decltype(std::declval<_Tx>() << std::declval<_Ty>(), std::true_type());

				template<class _Tx, class _Ty>
				static auto _test(...)->std::false_type;

				typedef decltype(_test<T, U>(0)) type;
			};

			template<class T>
			struct _Has_left_shift<T, void>
			{
				template<class _T>
				static auto _test(int) -> decltype(std::declval<_T>() << std::declval<_T>(), std::true_type());

				template<class _T>
				static auto _test(...)->std::false_type;

				typedef decltype(_test<T>(0)) type;
			};

			template<class T, class U = void>
			struct has_left_shift : _Has_left_shift<T, U>::type
			{
			};

			template<class T>
			struct has_left_shift<T, void> : _Has_left_shift<T, void>::type
			{
			};

			template<class T, class U>
			struct _Has_right_shift
			{
				template<class _Tx, class _Ty>
				static auto _test(int) -> decltype(std::declval<_Tx>() >> std::declval<_Ty>(), std::true_type());

				template<class _Tx, class _Ty>
				static auto _test(...)->std::false_type;

				typedef decltype(_test<T, U>(0)) type;
			};

			template<class T>
			struct _Has_right_shift<T, void>
			{
				template<class _T>
				static auto _test(int) -> decltype(std::declval<_T>() >> std::declval<_T>(), std::true_type());

				template<class _T>
				static auto _test(...)->std::false_type;

				typedef decltype(_test<T>(0)) type;
			};

			template<class T, class U = void>
			struct has_right_shift : _Has_right_shift<T, U>::type
			{
			};

			template<class T>
			struct has_right_shift<T, void> : _Has_right_shift<T, void>::type
			{
			};
		}
	}
}

#endif