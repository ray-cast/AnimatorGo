#ifndef OCTOON_DESERIALIZATION_H_
#define OCTOON_DESERIALIZATION_H_

#include <octoon/io/iosbase.h>
#include <octoon/io/serialization_buf.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT deserialization : public ios_base
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
			deserialization() noexcept;
			virtual ~deserialization() noexcept;

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

		protected:
			void _init(serialization_buf* _buf, ios_base::openmode mode) noexcept;

		private:
			deserialization & operator=(const deserialization&) = delete;
			deserialization(const deserialization&) = delete;

		private:
			serialization_buf * _strbuf;
			ios_base::openmode _mode;
		};
	}
}

#endif