#ifndef OCTOON_SERIALIZATION_H_
#define OCTOON_SERIALIZATION_H_

#include <octoon/io/iosbase.h>
#include <octoon/io/serialization_buf.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT serialization : public ios_base
		{
		public:
			using type_t = serialization_buf::type_t;
			using boolean_t = serialization_buf::boolean_t;
			using number_integer_t = serialization_buf::number_integer_t;
			using number_unsigned_t = serialization_buf::number_unsigned_t;
			using number_float_t = serialization_buf::number_float_t;
			using number_float2_t = serialization_buf::number_float2_t;
			using number_float3_t = serialization_buf::number_float3_t;
			using number_float4_t = serialization_buf::number_float4_t;
			using number_quaternion_t = serialization_buf::number_quaternion_t;
			using string_t = serialization_buf::string_t;
			using object_t = serialization_buf::object_t;
			using array_t = serialization_buf::array_t;
			using iterator = serialization_buf::iterator;
			using reverse_iterator = serialization_buf::reverse_iterator;
			using const_iterator = serialization_buf::const_iterator;
			using const_reverse_iterator = serialization_buf::const_reverse_iterator;

		public:
			serialization() noexcept;
			virtual ~serialization() noexcept;

			void set_open_mode(ios_base::openmode mode) noexcept;
			ios_base::openmode get_open_mode() const noexcept;

			serialization_buf* rdbuf() const noexcept;
			void set_rdbuf(serialization_buf* buf) noexcept;

			std::size_t size() const noexcept;

			const char* type_name() const noexcept;
			const char* type_name(type_t type) const noexcept;

			const type_t type() const noexcept;

			bool is_null() const noexcept;
			bool is_boolean() const noexcept;
			bool is_integral() const noexcept;
			bool is_float() const noexcept;
			bool is_string() const noexcept;
			bool is_numeric() const noexcept;
			bool is_array() const noexcept;
			bool is_object() const noexcept;

			template<type_t type, typename = std::enable_if_t<type == type_t::boolean>>
			constexpr boolean_t get() const except { return this->rdbuf()->get<type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::number_integer>>
			constexpr number_integer_t get() const except { return this->rdbuf()->get<type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::number_unsigned>>
			constexpr number_unsigned_t get() const except { return this->rdbuf()->get<type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::number_float>>
			constexpr number_float_t get() const except { return this->rdbuf()->get<number_float_t, type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::string>>
			constexpr string_t& get() const except { return *this->rdbuf()->get<type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::array>>
			constexpr array_t& get() const except { return *this->rdbuf()->get<type>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, boolean_t>::value>>
			constexpr boolean_t get() const except { return this->rdbuf()->get<type_t::boolean>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, number_integer_t>::value>>
			constexpr number_integer_t get() const except { return this->rdbuf()->get<type_t::number_integer>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, number_unsigned_t>::value>>
			constexpr number_unsigned_t get() const except { return this->rdbuf()->get<type_t::number_unsigned>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, number_float_t>::value>>
			constexpr number_float_t get() const except { return this->rdbuf()->get<type_t::number_float>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, string_t>::value>>
			constexpr string_t& get() const except { return this->rdbuf()->get<type_t::string>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, array_t>::value>>
			constexpr array_t& get() const except { return this->rdbuf()->get<type_t::array>(); }

		protected:
			void _init(serialization_buf* _buf, ios_base::openmode mode) noexcept;

		private:
			serialization & operator=(const serialization&) = delete;
			serialization(const serialization&) = delete;

		private:
			serialization_buf * _strbuf;
			ios_base::openmode _mode;
		};
	}
}

#endif