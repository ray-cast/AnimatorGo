#include <octoon/io/serialization.h>

namespace octoon
{
	namespace io
	{
		serialization::serialization() noexcept
		{
		}

		serialization::~serialization() noexcept
		{
		}

		serialization_buf*
		serialization::rdbuf() const noexcept
		{
			return _strbuf;
		}

		void
		serialization::set_rdbuf(serialization_buf* buf) noexcept
		{
			_strbuf = buf;
		}

		void
		serialization::_init(serialization_buf* _buf, ios_base::openmode mode) noexcept
		{
			this->set_rdbuf(_buf);
			_mode = mode;
			ios_base::_init(mode);
		}

		void
		serialization::set_open_mode(ios_base::openmode mode) noexcept
		{
			_mode = mode;
		}

		ios_base::openmode
		serialization::get_open_mode() const noexcept
		{
			return _mode;
		}

		std::size_t
		serialization::size() const noexcept
		{
			return this->rdbuf()->size();
		}

		const char*
		serialization::type_name() const noexcept
		{
			return this->rdbuf()->type_name();
		}

		const char*
		serialization::type_name(type_t type) const noexcept
		{
			return this->rdbuf()->type_name(type);
		}

		const serialization::type_t
		serialization::type() const noexcept
		{
			return this->rdbuf()->type();
		}

		bool
		serialization::is_null() const noexcept
		{
			return this->rdbuf()->is_null();
		}

		bool
		serialization::is_boolean() const noexcept
		{
			return this->rdbuf()->is_boolean();
		}

		bool
		serialization::is_integral() const noexcept
		{
			return this->rdbuf()->is_integral();
		}

		bool
		serialization::is_float() const noexcept
		{
			return this->rdbuf()->is_float();
		}

		bool
		serialization::is_string() const noexcept
		{
			return this->rdbuf()->is_string();
		}

		bool
		serialization::is_numeric() const noexcept
		{
			return this->rdbuf()->is_numeric();
		}

		bool
		serialization::is_array() const noexcept
		{
			return this->rdbuf()->is_array();
		}

		bool
		serialization::is_object() const noexcept
		{
			return this->rdbuf()->is_object();
		}
	}
}