#ifndef OCTOON_ARCHIVEBUF_H_
#define OCTOON_ARCHIVEBUF_H_

#include <octoon/math/math.h>
#include <octoon/runtime/except.h>

#include <map>
#include <list>
#include <memory>
#include <variant>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT serialization_buf final
		{
		public:
			using boolean_t = bool;
			using number_integer_t = std::int32_t;
			using number_unsigned_t = std::uint32_t;
			using number_float_t = float;
			using number_float2_t = math::detail::Vector2<number_float_t>;
			using number_float3_t = math::detail::Vector3<number_float_t>;
			using number_float4_t = math::detail::Vector4<number_float_t>;
			using number_quaternion_t = math::detail::Quaternion<number_float_t>;
			using string_t = std::string;
			using object_t = serialization_buf;;
			using array_t = std::vector<serialization_buf>;
			using map_t = std::list<std::pair<std::string, object_t>>;
			using iterator = map_t::iterator;
			using reverse_iterator = map_t::reverse_iterator;
			using const_iterator = map_t::const_iterator;
			using const_reverse_iterator = map_t::const_reverse_iterator;

			using variant_t = std::variant<
				void*,
				boolean_t,
				number_integer_t,
				number_unsigned_t,
				number_float_t,
				std::unique_ptr<string_t>,
				std::unique_ptr<array_t>,
				std::unique_ptr<map_t>
			>;

			static const serialization_buf& nil;
			static const serialization_buf& nilRef;

			enum type_t
			{
				null,
				boolean,
				number_integer,
				number_unsigned,
				number_float,
				string,
				array,
				object
			};
		public:
			serialization_buf() noexcept;
			serialization_buf(type_t value);
			serialization_buf(boolean_t value);
			serialization_buf(number_integer_t value);
			serialization_buf(number_unsigned_t value);
			serialization_buf(number_float_t value);
			serialization_buf(string_t&& value);
			serialization_buf(const string_t& value);
			serialization_buf(const string_t::value_type* value);
			serialization_buf(serialization_buf&& value);
			~serialization_buf() noexcept;

			serialization_buf& at(const string_t& key);
			serialization_buf& at(const string_t::value_type* key);
			serialization_buf& at(const std::size_t n);

			const serialization_buf& at(const string_t& key) const;
			const serialization_buf& at(const string_t::value_type* key) const;
			const serialization_buf& at(const std::size_t n) const;

			template<type_t type, typename = std::enable_if_t<type == type_t::boolean>>
			constexpr boolean_t get() const { return this->_get<type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::number_integer>>
			constexpr number_integer_t get() const { return this->_get<type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::number_unsigned>>
			constexpr number_unsigned_t get() const { return this->_get<type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::number_float>>
			constexpr number_float_t get() const { return this->get<number_float_t, type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::string>>
			constexpr string_t& get() const { return *this->_get<type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::array>>
			constexpr array_t& get() const { return *this->_get<type>(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::object>>
			constexpr object_t& get() const { return this->_get<type>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, boolean_t>::value>>
			constexpr boolean_t get() const { return this->get<serialization_buf::type_t::boolean>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, number_integer_t>::value>>
			constexpr number_integer_t get() const { return this->get<serialization_buf::type_t::number_integer>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, number_unsigned_t>::value>>
			constexpr number_unsigned_t get() const { return this->get<serialization_buf::type_t::number_unsigned>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, number_float_t>::value>>
			constexpr number_float_t get() const { return this->get<float, serialization_buf::type_t::number_float>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, string_t>::value>>
			constexpr string_t& get() const { return this->get<serialization_buf::type_t::string>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, array_t>::value>>
			constexpr array_t& get() const { return this->get<serialization_buf::type_t::array>(); }

			template<typename T, typename = std::enable_if_t<std::is_same<T, object_t>::value>>
			constexpr object_t& get() const { return this->get<serialization_buf::type_t::object>(); }

			void push_back(const string_t& key, boolean_t value);
			void push_back(const string_t& key, const number_integer_t& value);
			void push_back(const string_t& key, const number_unsigned_t& value);
			void push_back(const string_t& key, const number_float_t& value);
			void push_back(const string_t& key, const string_t& value);
			void push_back(const string_t& key, const string_t::value_type* value);
			void push_back(const string_t& key, serialization_buf&& value);

			iterator begin() noexcept;
			iterator end() noexcept;

			const_iterator begin() const noexcept;
			const_iterator end() const noexcept;

			reverse_iterator rbegin() noexcept;
			reverse_iterator rend() noexcept;

			const_reverse_iterator rbegin() const noexcept;
			const_reverse_iterator rend() const noexcept;

			serialization_buf& front() noexcept;
			const serialization_buf& front() const noexcept;

			serialization_buf& back() noexcept;
			const serialization_buf& back() const noexcept;

			void emplace(type_t type) noexcept;
			void clear() noexcept;

			std::size_t size() const noexcept;

			char* type_name() const noexcept;
			char* type_name(type_t type) const noexcept;

			type_t type() const noexcept;

			bool is_null() const noexcept;
			bool is_boolean() const noexcept;
			bool is_integral() const noexcept;
			bool is_float() const noexcept;
			bool is_string() const noexcept;
			bool is_numeric() const noexcept;
			bool is_array() const noexcept;
			bool is_object() const noexcept;

			serialization_buf& operator=(boolean_t value);
			serialization_buf& operator=(number_integer_t value);
			serialization_buf& operator=(number_unsigned_t value);
			serialization_buf& operator=(number_float_t value);
			serialization_buf& operator=(string_t&& value);
			serialization_buf& operator=(const string_t& value);
			serialization_buf& operator=(serialization_buf&& value);

			serialization_buf& operator[](const char* key);
			serialization_buf& operator[](const string_t& key);
			serialization_buf& operator[](std::size_t n);

			const serialization_buf& operator[](const char* key) const;
			const serialization_buf& operator[](const string_t& key) const;
			const serialization_buf& operator[](std::size_t n) const;

			virtual void lock() noexcept;
			virtual void unlock() noexcept;

			const serialization_buf& operator >> (serialization_buf::boolean_t& argv) const
			{
				const auto& value = *this;
				if (value.is_boolean())
					argv = value.get<serialization_buf::boolean_t>();
				return *this;
			}

			template<typename T, std::enable_if_t<std::is_signed<T>::value, int> = 0>
			const serialization_buf& operator >> (T& argv) const
			{
				if (this->is_integral())
					argv = static_cast<T>(this->get<serialization_buf::number_integer_t>());
				return *this;
			}

			template<typename T, std::enable_if_t<std::is_unsigned<T>::value, int> = 0>
			const serialization_buf& operator >> (T& argv) const
			{
				if (this->is_integral())
					argv = static_cast<T>(this->get<serialization_buf::number_unsigned_t>());
				return *this;
			}

			const serialization_buf& operator >> (serialization_buf::number_float_t& argv) const
			{
				const auto& value = *this;
				if (value.is_numeric())
					argv = value.get<serialization_buf::number_float_t>();
				return *this;
			}

			const serialization_buf& operator >> (serialization_buf::number_float2_t& argv) const
			{
				if (this->is_array())
				{
					const auto& values = this->get<serialization_buf::array_t>();
					if (values.size() == 2)
					{
						for (std::uint8_t i = 0; i < 2; ++i)
							argv[i] = values[i].get<serialization_buf::number_float_t>();
					}
					else
					{
						throw runtime::type_error::create("array length mismatch with 2");
					}
				}

				return *this;
			}

			const serialization_buf& operator >> (serialization_buf::number_float3_t& argv) const
			{
				if (this->is_array())
				{
					const auto& values = this->get<serialization_buf::array_t>();
					if (values.size() == 3)
					{
						for (std::uint8_t i = 0; i < 3; ++i)
							argv[i] = values[i].get<serialization_buf::number_float_t>();
					}
					else
					{
						throw runtime::type_error::create("array length mismatch with 3");
					}
				}

				return *this;
			}

			const serialization_buf& operator >> (serialization_buf::number_float4_t& argv) const
			{
				if (this->is_array())
				{
					const auto& values = this->get<serialization_buf::array_t>();
					if (values.size() == 4)
					{
						for (std::uint8_t i = 0; i < 4; ++i)
							argv[i] = values[i].get<serialization_buf::number_float_t>();
					}
					else
					{
						throw runtime::type_error::create("array length mismatch with 4");
					}
				}

				return *this;
			}

			const serialization_buf& operator >> (serialization_buf::number_quaternion_t& argv) const
			{
				if (this->is_array())
				{
					const auto& values = this->get<serialization_buf::array_t>();
					if (values.size() == 4)
					{
						for (std::uint8_t i = 0; i < 4; ++i)
							argv[i] = values[i].get<serialization_buf::number_float_t>();
					}
					else
					{
						throw runtime::type_error::create("array length mismatch with 4");
					}
				}

				return *this;
			}

			const serialization_buf& operator >> (serialization_buf::string_t& argv) const
			{
				const auto& value = *this;
				if (value.is_string())
					argv = value.get<serialization_buf::string_t>();
				return *this;
			}

			serialization_buf& operator << (serialization_buf::boolean_t& argv)
			{
				this->operator=(argv);
				return *this;
			}

			template<typename T, std::enable_if_t<std::is_integral<T>::value || std::is_same<T, number_float_t>::value, int> = 0>
			serialization_buf& operator << (const T& argv)
			{
				this->operator=(argv);
				return *this;
			}

			template<typename T, std::enable_if_t<std::is_same<T, string_t>::value || std::is_same<T, string_t::const_pointer>::value, int> = 0>
			serialization_buf& operator << (const T& argv)
			{
				this->operator=(argv);
				return *this;
			}

			serialization_buf& operator << (const serialization_buf::number_float2_t& argv)
			{
				if (!this->is_array())
					this->emplace(serialization_buf::array);

				if (this->is_array())
				{
					for (std::uint8_t i = 0; i < 2; ++i)
						this->operator[](i) = argv[i];
				}
				else
				{
					throw runtime::type_error::create(std::string("cannot use operator << with ") + this->type_name());
				}

				return *this;
			}

			serialization_buf& operator << (const serialization_buf::number_float3_t& argv)
			{
				if (!this->is_array())
					this->emplace(serialization_buf::array);

				if (this->is_array())
				{
					for (std::uint8_t i = 0; i < 3; ++i)
						this->operator[](i) = argv[i];
				}
				else
				{
					throw runtime::type_error::create(std::string("cannot use operator << with ") + this->type_name());
				}

				return *this;
			}

			serialization_buf& operator << (const serialization_buf::number_float4_t& argv)
			{
				if (!this->is_array())
					this->emplace(serialization_buf::array);

				if (this->is_array())
				{
					for (std::uint8_t i = 0; i < 4; ++i)
						this->operator[](i) = argv[i];
				}
				else
				{
					throw runtime::type_error::create(std::string("cannot use operator << with ") + this->type_name());
				}

				return *this;
			}

			serialization_buf& operator << (const serialization_buf::number_quaternion_t& argv)
			{
				if (!this->is_array())
					this->emplace(serialization_buf::array);

				if (this->is_array())
				{
					for (std::uint8_t i = 0; i < 4; ++i)
						this->operator[](i) = argv[i];
				}
				else
				{
					throw runtime::type_error::create(std::string("cannot use operator << with ") + this->type_name());
				}

				return *this;
			}

		private:
			template<typename T, type_t type>
			constexpr T get() const
			{
				switch (_data.index())
				{
				case type_t::boolean:
					return static_cast<T>(std::get<type_t::boolean>(_data));
				case type_t::number_integer:
					return static_cast<T>(std::get<type_t::number_integer>(_data));
				case type_t::number_unsigned:
					return static_cast<T>(std::get<type_t::number_unsigned>(_data));
				case type_t::number_float:
					return static_cast<T>(std::get<type_t::number_float>(_data));
				default:
					throw failure(string_t("type must be number, but is ") + this->type_name());
				}
			}

			template<type_t type, std::enable_if_t<type != object, int> = 0>
			constexpr decltype(auto) _get() const
			{
				if (this->type() != type)
					throw failure(string_t("type must be ") + type_name(type) + " but is " + this->type_name());

				return std::get<type>(_data);
			}

			template<type_t type, std::enable_if_t<type == object, int> = 0>
			constexpr object_t& _get() const
			{
				if (this->type() != type)
					throw failure(string_t("type must be ") + type_name(type) + " but is " + this->type_name());
				return std::get<type>(_data)->front().second;
			}

		private:
			serialization_buf(const serialization_buf& value);
			serialization_buf& operator=(const serialization_buf& value);

		private:
			variant_t _data;
		};
	}
}

#endif