#include <octoon/io/archive.h>

namespace octoon
{
	namespace io
	{
		archive::archive() noexcept
		{
		}

		archive::~archive() noexcept
		{
		}

		archivebuf*
		archive::rdbuf() const noexcept
		{
			return _strbuf;
		}

		void
		archive::set_rdbuf(archivebuf* buf) noexcept
		{
			_strbuf = buf;
		}

		void
		archive::_init(archivebuf* _buf, ios_base::openmode mode) noexcept
		{
			this->set_rdbuf(_buf);
			_mode = mode;
			ios_base::_init(mode);
		}

		std::size_t
		archive::size() const noexcept
		{
			return this->rdbuf()->size();
		}

		const char*
		archive::type_name() const noexcept
		{
			return this->rdbuf()->type_name();
		}

		const char*
		archive::type_name(type_t type) const noexcept
		{
			return this->rdbuf()->type_name(type);
		}

		archive::type_t
		archive::type() const noexcept
		{
			return this->rdbuf()->type();
		}

		bool
		archive::is_null() const noexcept
		{
			return this->rdbuf()->is_null();
		}

		bool
		archive::is_boolean() const noexcept
		{
			return this->rdbuf()->is_boolean();
		}

		bool
		archive::is_integral() const noexcept
		{
			return this->rdbuf()->is_integral();
		}

		bool
		archive::is_float() const noexcept
		{
			return this->rdbuf()->is_float();
		}

		bool
		archive::is_string() const noexcept
		{
			return this->rdbuf()->is_string();
		}

		bool
		archive::is_numeric() const noexcept
		{
			return this->rdbuf()->is_numeric();
		}

		bool
		archive::is_array() const noexcept
		{
			return this->rdbuf()->is_array();
		}

		bool
		archive::is_object() const noexcept
		{
			return this->rdbuf()->is_object();
		}
	}
}