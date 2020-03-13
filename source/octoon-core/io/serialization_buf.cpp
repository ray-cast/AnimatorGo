#include <octoon/io/serialization_buf.h>

namespace octoon
{
	namespace io
	{
		const serialization_buf& serialization_buf::nil = serialization_buf();
		const serialization_buf& serialization_buf::nilRef = serialization_buf();

		serialization_buf::serialization_buf() noexcept
		{
		}

		serialization_buf::serialization_buf(type_t value)
		{
			this->emplace(value);
		}

		serialization_buf::serialization_buf(boolean_t value)
			: _data(value)
		{
		}

		serialization_buf::serialization_buf(number_integer_t value)
			: _data(value)
		{
		}

		serialization_buf::serialization_buf(number_unsigned_t value)
			: _data(value)
		{
		}

		serialization_buf::serialization_buf(number_float_t value)
			: _data(value)
		{
		}

		serialization_buf::serialization_buf(string_t&& value)
			: _data(std::make_unique<string_t>(std::move(value)))
		{
		}

		serialization_buf::serialization_buf(const string_t& value)
			: _data(std::make_unique<string_t>(value))
		{
		}

		serialization_buf::serialization_buf(const string_t::value_type* value)
			: _data(std::make_unique<string_t>(value))
		{
		}

		serialization_buf::serialization_buf(serialization_buf&& node)
			: _data(std::move(node._data))
		{
		}

		serialization_buf::~serialization_buf() noexcept
		{
		}

		serialization_buf&
		serialization_buf::at(const string_t& key)
		{
			if (this->is_null())
				_data.emplace<std::unique_ptr<map_t>>(std::make_unique<map_t>());

			if (this->is_object())
			{
				auto& data = std::get<serialization_buf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				data->push_back(std::make_pair(key, serialization_buf::null));
				return data->back().second;
			}
			else
			{
				throw failure(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		serialization_buf&
		serialization_buf::at(const string_t::value_type* key)
		{
			if (this->is_null())
				_data.emplace<std::unique_ptr<map_t>>(std::make_unique<map_t>());

			if (this->is_object())
			{
				auto& data = std::get<serialization_buf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				data->push_back(std::make_pair(key, serialization_buf::null));
				return data->back().second;
			}
			else
			{
				throw failure(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		serialization_buf&
		serialization_buf::at(const std::size_t n)
		{
			if (this->is_array())
			{
				auto& data = std::get<serialization_buf::type_t::array>(_data);
				assert(data->size() > n);

				return (*data)[n];
			}
			else
			{
				throw failure(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		const serialization_buf&
		serialization_buf::at(const string_t& key) const
		{
			if (this->is_object())
			{
				auto& data = std::get<serialization_buf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				return serialization_buf::nil;
			}
			else
			{
				throw failure(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		const serialization_buf&
		serialization_buf::at(const string_t::value_type* key) const
		{
			if (this->is_object())
			{
				auto& data = std::get<serialization_buf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				return serialization_buf::nil;
			}
			else
			{
				throw failure(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		const serialization_buf&
		serialization_buf::at(const std::size_t n) const
		{
			if (this->is_array())
			{
				auto& data = std::get<serialization_buf::type_t::array>(_data);
				assert(data->size() > n);

				return (*data)[n];
			}
			else
			{
				throw failure(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		void
		serialization_buf::push_back(const string_t& key, boolean_t value)
		{
			if (this->is_null())
				this->emplace(serialization_buf::type_t::object);

			auto& data = std::get<serialization_buf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		serialization_buf::push_back(const string_t& key, const number_integer_t& value)
		{
			if (this->is_null())
				this->emplace(serialization_buf::type_t::object);

			auto& data = std::get<serialization_buf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		serialization_buf::push_back(const string_t& key, const number_unsigned_t& value)
		{
			if (this->is_null())
				this->emplace(serialization_buf::type_t::object);

			auto& data = std::get<serialization_buf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		serialization_buf::push_back(const string_t& key, const number_float_t& value)
		{
			if (this->is_null())
				this->emplace(serialization_buf::type_t::object);

			auto& data = std::get<serialization_buf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		serialization_buf::push_back(const string_t& key, const string_t& value)
		{
			if (this->is_null())
				this->emplace(serialization_buf::type_t::object);

			auto& data = std::get<serialization_buf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		serialization_buf::push_back(const string_t& key, const string_t::value_type* value)
		{
			if (this->is_null())
				this->emplace(serialization_buf::type_t::object);

			auto& data = std::get<serialization_buf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		serialization_buf::push_back(const string_t& key, serialization_buf&& value)
		{
			if (this->is_null())
				this->emplace(serialization_buf::type_t::object);

			auto& data = std::get<serialization_buf::type_t::object>(_data);
			data->push_back(std::make_pair(key, std::move(value)));
		}

		serialization_buf::iterator
		serialization_buf::begin() noexcept
		{
			switch (this->type())
			{
			case serialization_buf::type_t::object:
				if (std::get<serialization_buf::type_t::object>(_data))
					return std::get<serialization_buf::type_t::object>(_data)->begin();
				break;
			default:
				break;
			}

			return serialization_buf::iterator();
		}

		serialization_buf::iterator
		serialization_buf::end() noexcept
		{
			switch (this->type())
			{
			case serialization_buf::type_t::object:
				if (std::get<serialization_buf::type_t::object>(_data))
					return std::get<serialization_buf::type_t::object>(_data)->end();
				break;
			default:
				break;
			}

			return serialization_buf::iterator();
		}

		serialization_buf::const_iterator
		serialization_buf::begin() const noexcept
		{
			switch (this->type())
			{
			case serialization_buf::type_t::object:
				if (std::get<serialization_buf::type_t::object>(_data))
					return std::get<serialization_buf::type_t::object>(_data)->begin();
				break;
			default:
				break;
			}

			return serialization_buf::iterator();
		}

		serialization_buf::const_iterator
		serialization_buf::end() const noexcept
		{
			switch (this->type())
			{
			case serialization_buf::type_t::object:
				if (std::get<serialization_buf::type_t::object>(_data))
					return std::get<serialization_buf::type_t::object>(_data)->end();
				break;
			default:
				break;
			}

			return serialization_buf::iterator();
		}

		serialization_buf::reverse_iterator
		serialization_buf::rbegin() noexcept
		{
			switch (this->type())
			{
			case serialization_buf::type_t::object:
				if (std::get<serialization_buf::type_t::object>(_data))
					return std::get<serialization_buf::type_t::object>(_data)->rbegin();
				break;
			default:
				break;
			}

			return serialization_buf::reverse_iterator();
		}

		serialization_buf::reverse_iterator
		serialization_buf::rend() noexcept
		{
			switch (this->type())
			{
			case serialization_buf::type_t::object:
				if (std::get<serialization_buf::type_t::object>(_data))
					return std::get<serialization_buf::type_t::object>(_data)->rend();
				break;
			default:
				break;
			}

			return serialization_buf::reverse_iterator();
		}

		serialization_buf::const_reverse_iterator
		serialization_buf::rbegin() const noexcept
		{
			switch (this->type())
			{
			case serialization_buf::type_t::object:
				if (std::get<serialization_buf::type_t::object>(_data))
					return std::get<serialization_buf::type_t::object>(_data)->rbegin();
				break;
			default:
				break;
			}

			return serialization_buf::const_reverse_iterator();
		}

		serialization_buf::const_reverse_iterator
		serialization_buf::rend() const noexcept
		{
			switch (this->type())
			{
			case serialization_buf::type_t::object:
				if (std::get<serialization_buf::type_t::object>(_data))
					return std::get<serialization_buf::type_t::object>(_data)->rend();
				break;
			default:
				break;
			}

			return serialization_buf::const_reverse_iterator();
		}

		serialization_buf&
		serialization_buf::front() noexcept
		{
			assert(this->type() == serialization_buf::type_t::object);
			return std::get<serialization_buf::type_t::object>(_data)->front().second;
		}

		const serialization_buf&
		serialization_buf::front() const noexcept
		{
			assert(this->type() == serialization_buf::type_t::object);
			return std::get<serialization_buf::type_t::object>(_data)->front().second;
		}

		serialization_buf&
		serialization_buf::back() noexcept
		{
			assert(this->type() == serialization_buf::type_t::object);
			return std::get<serialization_buf::type_t::object>(_data)->back().second;
		}

		const serialization_buf&
		serialization_buf::back() const noexcept
		{
			assert(this->type() == serialization_buf::type_t::object);
			return std::get<serialization_buf::type_t::object>(_data)->back().second;
		}

		serialization_buf::type_t
		serialization_buf::type() const noexcept
		{
			return (type_t)_data.index();
		}

		char*
		serialization_buf::type_name() const noexcept
		{
			return this->type_name(this->type());
		}

		char*
		serialization_buf::type_name(type_t type) const noexcept
		{
			switch (type)
			{
			case serialization_buf::type_t::null:
				return "null";
			case serialization_buf::type_t::boolean:
				return "boolean";
			case serialization_buf::type_t::number_integer:
				return "interger";
			case serialization_buf::type_t::number_unsigned:
				return "unsigned interger";
			case serialization_buf::type_t::number_float:
				return "float point";
			case serialization_buf::type_t::string:
				return "string";
			case serialization_buf::type_t::array:
				return "array";
			case serialization_buf::type_t::object:
				return "object";
			default:
				return "unknow";
			}
		}

		bool
		serialization_buf::is_null() const noexcept
		{
			return this->type() == serialization_buf::type_t::null;
		}

		bool
		serialization_buf::is_boolean() const noexcept
		{
			return this->type() == serialization_buf::type_t::boolean;
		}

		bool
		serialization_buf::is_integral() const noexcept
		{
			return this->type() == serialization_buf::type_t::number_integer || this->type() == serialization_buf::type_t::number_unsigned;
		}

		bool
		serialization_buf::is_float() const noexcept
		{
			return this->type() == serialization_buf::type_t::number_float;
		}

		bool
		serialization_buf::is_string() const noexcept
		{
			return this->type() == serialization_buf::type_t::string;
		}

		bool
		serialization_buf::is_numeric() const noexcept
		{
			return this->is_integral() || this->is_float();
		}

		bool
		serialization_buf::is_array() const noexcept
		{
			return this->type() == serialization_buf::type_t::array;
		}

		bool
		serialization_buf::is_object() const noexcept
		{
			return this->type() == serialization_buf::type_t::object;
		}

		void
		serialization_buf::emplace(type_t type) noexcept
		{
			switch (type)
			{
			case serialization_buf::type_t::null:
				_data.emplace<void*>();
				break;
			case serialization_buf::type_t::boolean:
				_data.emplace<bool>(false);
				break;
			case serialization_buf::type_t::number_integer:
				_data.emplace<number_integer_t>(0);
				break;
			case serialization_buf::type_t::number_unsigned:
				_data.emplace<number_unsigned_t>(0);
				break;
			case serialization_buf::type_t::number_float:
				_data.emplace<number_float_t>(number_float_t(0.0f));
				break;
			case serialization_buf::type_t::string:
				_data.emplace<std::unique_ptr<string_t>>(std::make_unique<string_t>());
				break;
			case serialization_buf::type_t::array:
				_data.emplace<std::unique_ptr<array_t>>(std::make_unique<array_t>());
				break;
			case serialization_buf::type_t::object:
				_data.emplace<std::unique_ptr<map_t>>(std::make_unique<map_t>());
				break;
			default:
				break;
			}
		}

		void
		serialization_buf::clear() noexcept
		{
			_data.emplace<void*>();
		}

		std::size_t
		serialization_buf::size() const noexcept
		{
			if (this->is_null())
				return 0;

			if (this->is_array())
				return this->get<serialization_buf::array_t>().size();
			else
				return 1;
		}

		serialization_buf&
		serialization_buf::operator=(boolean_t value)
		{
			_data = value;
			return *this;
		}

		serialization_buf&
		serialization_buf::operator=(number_integer_t value)
		{
			_data = value;
			return *this;
		}

		serialization_buf&
		serialization_buf::operator=(number_unsigned_t value)
		{
			_data = value;
			return *this;
		}

		serialization_buf&
		serialization_buf::operator=(number_float_t value)
		{
			_data = value;
			return *this;
		}

		serialization_buf&
		serialization_buf::operator=(string_t&& value)
		{
			_data = std::make_unique<string_t>(std::move(value));
			return *this;
		}

		serialization_buf&
		serialization_buf::operator=(const string_t& value)
		{
			_data = std::make_unique<string_t>(value);
			return *this;
		}

		serialization_buf&
		serialization_buf::operator=(serialization_buf&& value)
		{
			_data = std::move(value._data);
			return *this;
		}

		serialization_buf&
		serialization_buf::operator[](const char* key)
		{
			if (this->is_null())
				_data.emplace<std::unique_ptr<map_t>>(std::make_unique<map_t>());

			if (this->is_object())
			{
				auto& data = std::get<serialization_buf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				data->push_back(std::make_pair(key, serialization_buf::null));
				return data->back().second;
			}
			else
			{
				throw failure(std::string("cannot use operator[] with ") + this->type_name());
			}
		}

		serialization_buf&
		serialization_buf::operator[](const string_t& key)
		{
			if (this->is_null())
				_data.emplace<std::unique_ptr<map_t>>(std::make_unique<map_t>());

			if (this->is_object())
			{
				auto& data = std::get<serialization_buf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				data->push_back(std::make_pair(key, null));
				return data->back().second;
			}
			else
			{
				throw failure(std::string("cannot use operator[] with ") + this->type_name());
			}
		}

		serialization_buf&
		serialization_buf::operator[](std::size_t n)
		{
			if (this->is_null())
				_data.emplace<std::unique_ptr<array_t>>(std::make_unique<array_t>());

			if (this->is_array())
			{
				if (n >= std::get<serialization_buf::type_t::array>(_data)->size())
				{
					auto end = std::get<serialization_buf::type_t::array>(_data)->end();
					auto size = std::get<serialization_buf::type_t::array>(_data)->size();
					std::get<serialization_buf::type_t::array>(_data)->resize(n + 1);
				}

				return std::get<serialization_buf::type_t::array>(_data)->operator[](n);
			}
			else
			{
				throw failure(std::string("cannot use operator[] with ") + this->type_name());
			}
		}

		const serialization_buf&
		serialization_buf::operator[](const char* key) const
		{
			return this->at(key);
		}

		const serialization_buf&
		serialization_buf::operator[](const string_t& key) const
		{
			return this->at(key);
		}

		const serialization_buf&
		serialization_buf::operator[](std::size_t n) const
		{
			return this->at(n);
		}

		void
		serialization_buf::lock() noexcept
		{
		}

		void
		serialization_buf::unlock() noexcept
		{
		}
	}
}