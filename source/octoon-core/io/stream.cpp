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
			ios_base::_init(mode);
		}
	}
}