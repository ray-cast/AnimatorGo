#ifndef OCTOON_ARCHIVE_H_
#define OCTOON_ARCHIVE_H_

#include <octoon/io/iosbase.h>
#include <octoon/io/archive_buf.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT archive : public ios_base
		{
		public:
			using type_t = archivebuf::type_t;
			using boolean_t = archivebuf::boolean_t;
			using number_integer_t = archivebuf::number_integer_t;
			using number_unsigned_t = archivebuf::number_unsigned_t;
			using number_float_t = archivebuf::number_float_t;
			using number_float2_t = archivebuf::number_float2_t;
			using number_float3_t = archivebuf::number_float3_t;
			using number_float4_t = archivebuf::number_float4_t;
			using number_quaternion_t = archivebuf::number_quaternion_t;
			using string_t = archivebuf::string_t;
			using object_t = archivebuf::object_t;
			using array_t = archivebuf::array_t;
			using iterator = archivebuf::iterator;
			using reverse_iterator = archivebuf::reverse_iterator;
			using const_iterator = archivebuf::const_iterator;
			using const_reverse_iterator = archivebuf::const_reverse_iterator;

		public:
			archive() noexcept;
			virtual ~archive() noexcept;

			archivebuf* rdbuf() const noexcept;
			void set_rdbuf(archivebuf* buf) noexcept;

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
			void _init(archivebuf* _buf, ios_base::openmode mode) noexcept;

		private:
			archive & operator=(const archive&) = delete;
			archive(const archive&) = delete;

		private:
			archivebuf * _strbuf;
			ios_base::openmode _mode;
		};
	}
}

#endif