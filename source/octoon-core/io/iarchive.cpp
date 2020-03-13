#include <octoon/io/iarchive.h>

namespace octoon
{
	namespace io
	{
		iarchive::isentry::isentry(iarchive* _istr)
			: _ok(true)
			, _my_istr(_istr)
		{
			if (_my_istr->rdbuf() != 0)
				_my_istr->rdbuf()->lock();
		}

		iarchive::isentry::~isentry() noexcept
		{
			if (_my_istr->rdbuf() != 0)
				_my_istr->rdbuf()->unlock();
		}

		iarchive::isentry::operator bool() const noexcept
		{
			return _ok ? true : false;
		}

		iarchive::iarchive(archivebuf* buf) noexcept
		{
			archive::_init(buf, ios_base::in);
		}

		iarchive::~iarchive() noexcept
		{
		}

		const archivebuf&
		iarchive::at(const string_t& key) const except
		{
			return this->rdbuf()->at(key);
		}

		const archivebuf&
		iarchive::at(string_t::const_pointer key) const except
		{
			return this->rdbuf()->at(key);
		}

		const archivebuf&
		iarchive::at(const std::size_t n) const except
		{
			return this->rdbuf()->at(n);
		}

		const archivebuf&
		iarchive::operator[](std::size_t n) const except
		{
			return this->rdbuf()->operator[](n);
		}

		const archivebuf&
		iarchive::operator[](const string_t& key) const except
		{
			return this->rdbuf()->operator[](key);
		}

		const archivebuf&
		iarchive::operator[](string_t::const_pointer key) const except
		{
			return this->rdbuf()->operator[](key);
		}
	}
}