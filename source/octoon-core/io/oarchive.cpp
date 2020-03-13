#include <octoon/io/oarchive.h>

namespace octoon
{
	namespace io
	{
		oarchive::osentry::osentry(oarchive* _istr)
			: _ok(true)
			, _my_istr(_istr)
		{
			if (_my_istr->rdbuf() != 0)
				_my_istr->rdbuf()->lock();
		}

		oarchive::osentry::~osentry() noexcept
		{
			if (_my_istr->rdbuf() != 0)
				_my_istr->rdbuf()->unlock();
		}

		oarchive::osentry::operator bool() const noexcept
		{
			return _ok ? true : false;
		}

		oarchive::oarchive(archivebuf* buf) noexcept
		{
			archive::_init(buf, ios_base::out);
		}

		oarchive::~oarchive() noexcept
		{
		}

		void
		oarchive::emplace(type_t type) noexcept
		{
			this->rdbuf()->emplace(type);
		}
		void
		oarchive::clear() noexcept
		{
			this->rdbuf()->clear();
		}

		archivebuf&
		oarchive::at(const string_t& key) except
		{
			return this->rdbuf()->at(key);
		}

		archivebuf&
		oarchive::at(const string_t::value_type* key) except
		{
			return this->rdbuf()->at(key);
		}

		archivebuf&
		oarchive::at(const std::size_t n) except
		{
			return this->rdbuf()->at(n);
		}

		const archivebuf&
		oarchive::at(const string_t& key) const except
		{
			return this->rdbuf()->at(key);
		}

		const archivebuf&
		oarchive::at(string_t::const_pointer key) const except
		{
			return this->rdbuf()->at(key);
		}

		const archivebuf&
		oarchive::at(const std::size_t n) const except
		{
			return this->rdbuf()->at(n);
		}

		void
		oarchive::push_back(const string_t& key, boolean_t value)
		{
			this->rdbuf()->push_back(key, value);
		}

		void
		oarchive::push_back(const string_t& key, const number_integer_t& value)
		{
			this->rdbuf()->push_back(key, value);
		}

		void
		oarchive::push_back(const string_t& key, const number_unsigned_t& value)
		{
			this->rdbuf()->push_back(key, value);
		}

		void
		oarchive::push_back(const string_t& key, const number_float_t& value)
		{
			this->rdbuf()->push_back(key, value);
		}

		void
		oarchive::push_back(const string_t& key, const string_t& value)
		{
			this->rdbuf()->push_back(key, value);
		}

		void
		oarchive::push_back(const string_t& key, const string_t::value_type* value)
		{
			this->rdbuf()->push_back(key, value);
		}

		void
		oarchive::push_back(archivebuf&& value)
		{
			this->rdbuf()->push_back(std::move(value));
		}

		oarchive&
		oarchive::operator=(boolean_t value) except
		{
			this->rdbuf()->operator=(value);
			return *this;
		}

		oarchive&
		oarchive::operator=(number_integer_t value) except
		{
			this->rdbuf()->operator=(value);
			return *this;
		}

		oarchive&
		oarchive::operator=(number_unsigned_t value) except
		{
			this->rdbuf()->operator=(value);
			return *this;
		}

		oarchive&
		oarchive::operator=(number_float_t value) except
		{
			this->rdbuf()->operator=(value);
			return *this;
		}

		oarchive&
		oarchive::operator=(string_t&& value) except
		{
			this->rdbuf()->operator=(value);
			return *this;
		}

		oarchive&
		oarchive::operator=(const string_t& value) except
		{
			this->rdbuf()->operator=(value);
			return *this;
		}

		oarchive&
		oarchive::operator[](std::size_t n) except
		{
			this->rdbuf()->operator[](n);
			return *this;
		}

		oarchive&
		oarchive::operator[](const string_t& key) except
		{
			this->rdbuf()->operator[](key);
			return *this;
		}

		oarchive&
		oarchive::operator[](string_t::const_pointer key) except
		{
			this->rdbuf()->operator[](key);
			return *this;
		}

		const oarchive&
		oarchive::operator[](std::size_t n) const except
		{
			this->rdbuf()->operator[](n);
			return *this;
		}

		const oarchive&
		oarchive::operator[](const string_t& key) const except
		{
			this->rdbuf()->operator[](key);
			return *this;
		}

		const oarchive&
		oarchive::operator[](string_t::const_pointer key) const except
		{
			this->rdbuf()->operator[](key);
			return *this;
		}
	}
}