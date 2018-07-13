#ifndef OCTOON_IARCHIVE_H_
#define OCTOON_IARCHIVE_H_

#include <octoon/io/oarchive.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT iarchive : virtual public archive
		{
		protected:
			class isentry final
			{
			public:
				isentry(iarchive* _istr);
				~isentry() noexcept;

				operator bool() const noexcept;

			private:
				isentry(const isentry&) noexcept = delete;
				isentry& operator=(const isentry&) noexcept = delete;

			private:
				bool _ok;
				iarchive* _my_istr;
			};

		public:
			iarchive(archivebuf* buf) noexcept;
			virtual ~iarchive() noexcept;

			const archivebuf& at(const string_t& key) const except;
			const archivebuf& at(const std::size_t n) const except;
			const archivebuf& at(string_t::const_pointer key) const except;

			const archivebuf& operator[](std::size_t n) const except;
			const archivebuf& operator[](const string_t& key) const except;
			const archivebuf& operator[](string_t::const_pointer key) const except;

			const iarchive& operator >> (boolean_t& argv) const except
			{
				const auto& value = *this;
				if (value.is_boolean())
					argv = value.get<boolean_t>();
				return *this;
			}

			template<typename T, std::enable_if_t<std::is_signed<T>::value, int> = 0>
			const iarchive& operator >> (T& argv) const except
			{
				if (this->is_integral())
					argv = static_cast<T>(this->get<number_integer_t>());
				return *this;
			}

			template<typename T, std::enable_if_t<std::is_unsigned<T>::value, int> = 0>
			const iarchive& operator >> (T& argv) const except
			{
				if (this->is_integral())
					argv = static_cast<T>(this->get<number_unsigned_t>());
				return *this;
			}

			const iarchive& operator >> (number_float_t& argv) const except
			{
				const auto& value = *this;
				if (value.is_numeric())
					argv = value.get<number_float_t>();
				return *this;
			}

			const iarchive& operator >> (number_float2_t& argv) const except
			{
				if (this->is_array())
				{
					const auto& values = this->get<array_t>();
					if (values.size() == 2)
					{
						for (std::uint8_t i = 0; i < 2; ++i)
							argv[i] = values[i].get<number_float_t>();
					}
					else
					{
						throw std::runtime_error(std::string("array length mismatch with 2"));
					}
				}

				return *this;
			}

			const iarchive& operator >> (number_float3_t& argv) const except
			{
				if (this->is_array())
				{
					const auto& values = this->get<array_t>();
					if (values.size() == 3)
					{
						for (std::uint8_t i = 0; i < 3; ++i)
							argv[i] = values[i].get<number_float_t>();
					}
					else
					{
						throw std::runtime_error(std::string("array length mismatch with 3"));
					}
				}

				return *this;
			}

			const iarchive& operator >> (number_float4_t& argv) const except
			{
				if (this->is_array())
				{
					const auto& values = this->get<array_t>();
					if (values.size() == 4)
					{
						for (std::uint8_t i = 0; i < 4; ++i)
							argv[i] = values[i].get<number_float_t>();
					}
					else
					{
						throw std::runtime_error(std::string("array length mismatch with 4"));
					}
				}

				return *this;
			}

			const iarchive& operator >> (number_quaternion_t& argv) const except
			{
				if (this->is_array())
				{
					const auto& values = this->get<array_t>();
					if (values.size() == 4)
					{
						for (std::uint8_t i = 0; i < 4; ++i)
							argv[i] = values[i].get<number_float_t>();
					}
					else
					{
						throw std::runtime_error(std::string("array length mismatch with 4"));
					}
				}

				return *this;
			}

			const iarchive& operator >> (string_t& argv) const except
			{
				const auto& value = *this;
				if (value.is_string())
					argv = value.get<string_t>();
				return *this;
			}

		private:
			iarchive(const iarchive&) noexcept = delete;
			iarchive& operator=(const iarchive&) noexcept = delete;
		};
	}
}

#endif