#include <octoon/io/archive_buf.h>

namespace octoon
{
	namespace io
	{
		const archivebuf& archivebuf::nil = archivebuf();
		const archivebuf& archivebuf::nilRef = archivebuf();

		archivebuf::archivebuf() noexcept
		{
		}

		archivebuf::archivebuf(type_t value)
		{
			this->emplace(value);
		}

		archivebuf::archivebuf(boolean_t value)
			: _data(value)
		{
		}

		archivebuf::archivebuf(number_integer_t value)
			: _data(value)
		{
		}

		archivebuf::archivebuf(number_unsigned_t value)
			: _data(value)
		{
		}

		archivebuf::archivebuf(number_float_t value)
			: _data(value)
		{
		}

		archivebuf::archivebuf(string_t&& value)
			: _data(std::make_unique<string_t>(std::move(value)))
		{
		}

		archivebuf::archivebuf(const string_t& value)
			: _data(std::make_unique<string_t>(value))
		{
		}

		archivebuf::archivebuf(const string_t::value_type* value)
			: _data(std::make_unique<string_t>(value))
		{
		}

		archivebuf::archivebuf(archivebuf&& node)
			: _data(std::move(node._data))
		{
		}

		archivebuf::~archivebuf() noexcept
		{
		}

		archivebuf&
		archivebuf::at(const string_t& key)
		{
			if (this->is_null())
				_data.emplace<std::unique_ptr<map_t>>(std::make_unique<map_t>());

			if (this->is_object())
			{
				auto& data = runtime::get<archivebuf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				data->push_back(std::make_pair(key, archivebuf::null));
				return data->back().second;
			}
			else
			{
				throw std::runtime_error(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		archivebuf&
		archivebuf::at(const string_t::value_type* key)
		{
			if (this->is_null())
				_data.emplace<std::unique_ptr<map_t>>(std::make_unique<map_t>());

			if (this->is_object())
			{
				auto& data = runtime::get<archivebuf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				data->push_back(std::make_pair(key, archivebuf::null));
				return data->back().second;
			}
			else
			{
				throw std::runtime_error(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		archivebuf&
		archivebuf::at(const std::size_t n)
		{
			if (this->is_array())
			{
				auto& data = runtime::get<archivebuf::type_t::array>(_data);
				assert(data->size() > n);

				return (*data)[n];
			}
			else
			{
				throw std::runtime_error(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		const archivebuf&
		archivebuf::at(const string_t& key) const
		{
			if (this->is_object())
			{
				auto& data = runtime::get<archivebuf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				return archivebuf::nil;
			}
			else
			{
				throw std::runtime_error(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		const archivebuf&
		archivebuf::at(const string_t::value_type* key) const
		{
			if (this->is_object())
			{
				auto& data = runtime::get<archivebuf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				return archivebuf::nil;
			}
			else
			{
				throw std::runtime_error(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		const archivebuf&
		archivebuf::at(const std::size_t n) const
		{
			if (this->is_array())
			{
				auto& data = runtime::get<archivebuf::type_t::array>(_data);
				assert(data->size() > n);

				return (*data)[n];
			}
			else
			{
				throw std::runtime_error(std::string("cannot use function:at with ") + this->type_name());
			}
		}

		void
		archivebuf::push_back(const string_t& key, boolean_t value)
		{
			if (this->is_null())
				this->emplace(archivebuf::type_t::object);

			auto& data = runtime::get<archivebuf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		archivebuf::push_back(const string_t& key, const number_integer_t& value)
		{
			if (this->is_null())
				this->emplace(archivebuf::type_t::object);

			auto& data = runtime::get<archivebuf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		archivebuf::push_back(const string_t& key, const number_unsigned_t& value)
		{
			if (this->is_null())
				this->emplace(archivebuf::type_t::object);

			auto& data = runtime::get<archivebuf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		archivebuf::push_back(const string_t& key, const number_float_t& value)
		{
			if (this->is_null())
				this->emplace(archivebuf::type_t::object);

			auto& data = runtime::get<archivebuf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		archivebuf::push_back(const string_t& key, const string_t& value)
		{
			if (this->is_null())
				this->emplace(archivebuf::type_t::object);

			auto& data = runtime::get<archivebuf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		archivebuf::push_back(const string_t& key, const string_t::value_type* value)
		{
			if (this->is_null())
				this->emplace(archivebuf::type_t::object);

			auto& data = runtime::get<archivebuf::type_t::object>(_data);
			data->push_back(std::make_pair(key, value));
		}

		void
		archivebuf::push_back(const string_t& key, archivebuf&& value)
		{
			if (this->is_null())
				this->emplace(archivebuf::type_t::object);

			auto& data = runtime::get<archivebuf::type_t::object>(_data);
			data->push_back(std::make_pair(key, std::move(value)));
		}

		archivebuf::iterator
		archivebuf::begin() noexcept
		{
			switch (this->type())
			{
			case archivebuf::type_t::object:
				if (runtime::get<archivebuf::type_t::object>(_data))
					return runtime::get<archivebuf::type_t::object>(_data)->begin();
				break;
			default:
				break;
			}

			return archivebuf::iterator();
		}

		archivebuf::iterator
		archivebuf::end() noexcept
		{
			switch (this->type())
			{
			case archivebuf::type_t::object:
				if (runtime::get<archivebuf::type_t::object>(_data))
					return runtime::get<archivebuf::type_t::object>(_data)->end();
				break;
			default:
				break;
			}

			return archivebuf::iterator();
		}

		archivebuf::const_iterator
		archivebuf::begin() const noexcept
		{
			switch (this->type())
			{
			case archivebuf::type_t::object:
				if (runtime::get<archivebuf::type_t::object>(_data))
					return runtime::get<archivebuf::type_t::object>(_data)->begin();
				break;
			default:
				break;
			}

			return archivebuf::iterator();
		}

		archivebuf::const_iterator
		archivebuf::end() const noexcept
		{
			switch (this->type())
			{
			case archivebuf::type_t::object:
				if (runtime::get<archivebuf::type_t::object>(_data))
					return runtime::get<archivebuf::type_t::object>(_data)->end();
				break;
			default:
				break;
			}

			return archivebuf::iterator();
		}

		archivebuf::reverse_iterator
		archivebuf::rbegin() noexcept
		{
			switch (this->type())
			{
			case archivebuf::type_t::object:
				if (runtime::get<archivebuf::type_t::object>(_data))
					return runtime::get<archivebuf::type_t::object>(_data)->rbegin();
				break;
			default:
				break;
			}

			return archivebuf::reverse_iterator();
		}

		archivebuf::reverse_iterator
		archivebuf::rend() noexcept
		{
			switch (this->type())
			{
			case archivebuf::type_t::object:
				if (runtime::get<archivebuf::type_t::object>(_data))
					return runtime::get<archivebuf::type_t::object>(_data)->rend();
				break;
			default:
				break;
			}

			return archivebuf::reverse_iterator();
		}

		archivebuf::const_reverse_iterator
		archivebuf::rbegin() const noexcept
		{
			switch (this->type())
			{
			case archivebuf::type_t::object:
				if (runtime::get<archivebuf::type_t::object>(_data))
					return runtime::get<archivebuf::type_t::object>(_data)->rbegin();
				break;
			default:
				break;
			}

			return archivebuf::const_reverse_iterator();
		}

		archivebuf::const_reverse_iterator
		archivebuf::rend() const noexcept
		{
			switch (this->type())
			{
			case archivebuf::type_t::object:
				if (runtime::get<archivebuf::type_t::object>(_data))
					return runtime::get<archivebuf::type_t::object>(_data)->rend();
				break;
			default:
				break;
			}

			return archivebuf::const_reverse_iterator();
		}

		archivebuf&
		archivebuf::front() noexcept
		{
			assert(this->type() == archivebuf::type_t::object);
			return runtime::get<archivebuf::type_t::object>(_data)->front().second;
		}

		const archivebuf&
		archivebuf::front() const noexcept
		{
			assert(this->type() == archivebuf::type_t::object);
			return runtime::get<archivebuf::type_t::object>(_data)->front().second;
		}

		archivebuf&
		archivebuf::back() noexcept
		{
			assert(this->type() == archivebuf::type_t::object);
			return runtime::get<archivebuf::type_t::object>(_data)->back().second;
		}

		const archivebuf&
		archivebuf::back() const noexcept
		{
			assert(this->type() == archivebuf::type_t::object);
			return runtime::get<archivebuf::type_t::object>(_data)->back().second;
		}

		archivebuf::type_t
		archivebuf::type() const noexcept
		{
			return (type_t)_data.index();
		}

		char*
		archivebuf::type_name() const noexcept
		{
			return this->type_name(this->type());
		}

		char*
		archivebuf::type_name(type_t type) const noexcept
		{
			switch (type)
			{
			case archivebuf::type_t::null:
				return "null";
			case archivebuf::type_t::boolean:
				return "boolean";
			case archivebuf::type_t::number_integer:
				return "interger";
			case archivebuf::type_t::number_unsigned:
				return "unsigned interger";
			case archivebuf::type_t::number_float:
				return "float point";
			case archivebuf::type_t::string:
				return "string";
			case archivebuf::type_t::array:
				return "array";
			case archivebuf::type_t::object:
				return "object";
			default:
				return "unknow";
			}
		}

		bool
		archivebuf::is_null() const noexcept
		{
			return this->type() == archivebuf::type_t::null;
		}

		bool
		archivebuf::is_boolean() const noexcept
		{
			return this->type() == archivebuf::type_t::boolean;
		}

		bool
		archivebuf::is_integral() const noexcept
		{
			return this->type() == archivebuf::type_t::number_integer || this->type() == archivebuf::type_t::number_unsigned;
		}

		bool
		archivebuf::is_float() const noexcept
		{
			return this->type() == archivebuf::type_t::number_float;
		}

		bool
		archivebuf::is_string() const noexcept
		{
			return this->type() == archivebuf::type_t::string;
		}

		bool
		archivebuf::is_numeric() const noexcept
		{
			return this->is_integral() || this->is_float();
		}

		bool
		archivebuf::is_array() const noexcept
		{
			return this->type() == archivebuf::type_t::array;
		}

		bool
		archivebuf::is_object() const noexcept
		{
			return this->type() == archivebuf::type_t::object;
		}

		void
		archivebuf::emplace(type_t type) noexcept
		{
			switch (type)
			{
			case archivebuf::type_t::null:
				_data.emplace<void*>();
				break;
			case archivebuf::type_t::boolean:
				_data.emplace<bool>(false);
				break;
			case archivebuf::type_t::number_integer:
				_data.emplace<number_integer_t>(0);
				break;
			case archivebuf::type_t::number_unsigned:
				_data.emplace<number_unsigned_t>(0);
				break;
			case archivebuf::type_t::number_float:
				_data.emplace<number_float_t>(number_float_t(0.0f));
				break;
			case archivebuf::type_t::string:
				_data.emplace<std::unique_ptr<string_t>>(std::make_unique<string_t>());
				break;
			case archivebuf::type_t::array:
				_data.emplace<std::unique_ptr<array_t>>(std::make_unique<array_t>());
				break;
			case archivebuf::type_t::object:
				_data.emplace<std::unique_ptr<map_t>>(std::make_unique<map_t>());
				break;
			default:
				break;
			}
		}

		void
		archivebuf::clear() noexcept
		{
			_data.emplace<void*>();
		}

		std::size_t
		archivebuf::size() const noexcept
		{
			if (this->is_null())
				return 0;

			if (this->is_array())
				return this->get<archivebuf::array_t>().size();
			else
				return 1;
		}

		archivebuf&
		archivebuf::operator=(boolean_t value)
		{
			_data = value;
			return *this;
		}

		archivebuf&
		archivebuf::operator=(number_integer_t value)
		{
			_data = value;
			return *this;
		}

		archivebuf&
		archivebuf::operator=(number_unsigned_t value)
		{
			_data = value;
			return *this;
		}

		archivebuf&
		archivebuf::operator=(number_float_t value)
		{
			_data = value;
			return *this;
		}

		archivebuf&
		archivebuf::operator=(string_t&& value)
		{
			_data = std::make_unique<string_t>(std::move(value));
			return *this;
		}

		archivebuf&
		archivebuf::operator=(const string_t& value)
		{
			_data = std::make_unique<string_t>(value);
			return *this;
		}

		archivebuf&
		archivebuf::operator=(archivebuf&& value)
		{
			_data = std::move(value._data);
			return *this;
		}

		archivebuf&
		archivebuf::operator[](const char* key)
		{
			if (this->is_null())
				_data.emplace<std::unique_ptr<map_t>>(std::make_unique<map_t>());

			if (this->is_object())
			{
				auto& data = runtime::get<archivebuf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				data->push_back(std::make_pair(key, archivebuf::null));
				return data->back().second;
			}
			else
			{
				throw std::runtime_error(std::string("cannot use operator[] with ") + this->type_name());
			}
		}

		archivebuf&
		archivebuf::operator[](const string_t& key)
		{
			if (this->is_null())
				_data.emplace<std::unique_ptr<map_t>>(std::make_unique<map_t>());

			if (this->is_object())
			{
				auto& data = runtime::get<archivebuf::type_t::object>(_data);

				for (auto& it : *data)
					if (it.first == key)
						return it.second;

				data->push_back(std::make_pair(key, null));
				return data->back().second;
			}
			else
			{
				throw std::runtime_error(std::string("cannot use operator[] with ") + this->type_name());
			}
		}

		archivebuf&
		archivebuf::operator[](std::size_t n)
		{
			if (this->is_null())
				_data.emplace<std::unique_ptr<array_t>>(std::make_unique<array_t>());

			if (this->is_array())
			{
				if (n >= runtime::get<archivebuf::type_t::array>(_data)->size())
					runtime::get<archivebuf::type_t::array>(_data)->resize(n + 1);

				return runtime::get<archivebuf::type_t::array>(_data)->operator[](n);
			}
			else
			{
				throw std::runtime_error(std::string("cannot use operator[] with ") + this->type_name());
			}
		}

		const archivebuf&
		archivebuf::operator[](const char* key) const
		{
			return this->at(key);
		}

		const archivebuf&
		archivebuf::operator[](const string_t& key) const
		{
			return this->at(key);
		}

		const archivebuf&
		archivebuf::operator[](std::size_t n) const
		{
			return this->at(n);
		}

		void
		archivebuf::lock() noexcept
		{
		}

		void
		archivebuf::unlock() noexcept
		{
		}
	}
}