#include <octoon/io/stream.h>

namespace octoon
{
	namespace io
	{
		stream_buf*
		stream::rdbuf() const noexcept
		{
			return _strbuf;
		}

		void
		stream::set_rdbuf(stream_buf* buf) noexcept
		{
			_strbuf = buf;
		}

		void
		stream::_init(stream_buf* _buf, ios_base::openmode mode) noexcept
		{
			this->set_rdbuf(_buf);
			_mode = mode;
			ios_base::_init();
		}

		void
		stream::set_open_mode(ios_base::openmode mode) noexcept
		{
			_mode = mode;
		}

		ios_base::openmode
		stream::get_open_mode() const noexcept
		{
			return _mode;
		}
	}
}