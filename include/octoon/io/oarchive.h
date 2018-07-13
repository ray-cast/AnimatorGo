#ifndef OCTOON_OARCHIVE_H_
#define OCTOON_OARCHIVE_H_

#include <octoon/io/archive.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT oarchive : virtual public archive
		{
		protected:
			class osentry final
			{
			public:
				osentry(oarchive* _istr);
				~osentry() noexcept;

				operator bool() const noexcept;

			private:
				osentry(const osentry&) noexcept = delete;
				osentry& operator=(const osentry&) noexcept = delete;

			private:
				bool _ok;
				oarchive* _my_istr;
			};

		public:
			oarchive(archivebuf* buf) noexcept;
			virtual ~oarchive() noexcept;

			void emplace(type_t type) noexcept;
			void clear() noexcept;

			archivebuf& at(const string_t& key) except;
			archivebuf& at(const string_t::value_type* key) except;
			archivebuf& at(const std::size_t n) except;

			const archivebuf& at(const string_t& key) const except;
			const archivebuf& at(const string_t::value_type* key) const except;
			const archivebuf& at(const std::size_t n) const except;

			oarchive& operator=(boolean_t value) except;
			oarchive& operator=(number_integer_t value) except;
			oarchive& operator=(number_unsigned_t value) except;
			oarchive& operator=(number_float_t value) except;
			oarchive& operator=(string_t&& value) except;
			oarchive& operator=(const string_t& value) except;
			oarchive& operator=(string_t::const_pointer& value) except;

			oarchive& operator[](std::size_t n) except;
			oarchive& operator[](const string_t& key) except;
			oarchive& operator[](string_t::const_pointer key) except;

			const oarchive& operator[](std::size_t n) const except;
			const oarchive& operator[](const string_t& key) const except;
			const oarchive& operator[](string_t::const_pointer key) const except;

			void push_back(const string_t& key, boolean_t value);
			void push_back(const string_t& key, const number_integer_t& value);
			void push_back(const string_t& key, const number_unsigned_t& value);
			void push_back(const string_t& key, const number_float_t& value);
			void push_back(const string_t& key, const string_t& value);
			void push_back(const string_t& key, const string_t::value_type* value);
			void push_back(archivebuf&& value);

			oarchive& operator << (archivebuf::boolean_t& argv)
			{
				this->operator=(argv);
				return *this;
			}

			template<typename T, std::enable_if_t<std::is_integral<T>::value || std::is_unsigned<T>::value || std::is_floating_point<T>::value, int> = 0>
			oarchive& operator << (T argv)
			{
				this->operator=(argv);
				return *this;
			}

			template<typename T, std::enable_if_t<std::is_same<T, string_t>::value || std::is_same<T, string_t::const_pointer>::value, int> = 0>
			oarchive& operator << (const T& argv)
			{
				this->operator=(argv);
				return *this;
			}

			oarchive& operator << (const archivebuf::number_float2_t& argv)
			{
				if (!this->is_array())
					this->emplace(archivebuf::array);

				if (this->is_array())
				{
					for (std::uint8_t i = 0; i < 2; ++i)
						this->operator[](i) = argv[i];
				}
				else
				{
					throw std::runtime_error(std::string("cannot use operator << with ") + this->type_name());
				}

				return *this;
			}

			oarchive& operator << (const archivebuf::number_float3_t& argv)
			{
				if (!this->is_array())
					this->emplace(archivebuf::array);

				if (this->is_array())
				{
					for (std::uint8_t i = 0; i < 3; ++i)
						this->operator[](i) = argv[i];
				}
				else
				{
					throw std::runtime_error(std::string("cannot use operator << with ") + this->type_name());
				}

				return *this;
			}

			oarchive& operator << (const archivebuf::number_float4_t& argv)
			{
				if (!this->is_array())
					this->emplace(archivebuf::array);

				if (this->is_array())
				{
					for (std::uint8_t i = 0; i < 4; ++i)
						this->operator[](i) = argv[i];
				}
				else
				{
					throw std::runtime_error(std::string("cannot use operator << with ") + this->type_name());
				}

				return *this;
			}

			oarchive& operator << (const archivebuf::number_quaternion_t& argv)
			{
				if (!this->is_array())
					this->emplace(archivebuf::array);

				if (this->is_array())
				{
					for (std::uint8_t i = 0; i < 4; ++i)
						this->operator[](i) = argv[i];
				}
				else
				{
					throw std::runtime_error(std::string("cannot use operator << with ") + this->type_name());
				}

				return *this;
			}

		private:

			oarchive(const oarchive&) noexcept = delete;
			oarchive& operator=(const oarchive&) noexcept = delete;
		};
	}
}

#endif