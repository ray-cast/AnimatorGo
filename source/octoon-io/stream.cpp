#include <utility>
#include "octoon/io/stream.h"

namespace octoon
{
	namespace io
	{
		namespace detail
		{
			StdStreamExt::StdStreamExt(stream* src) noexcept
				: std::iostream(this)
				, src_(src)
			{
			}

			StdStreamExt::StdStreamExt(StdStreamExt&& rv) noexcept
				: std::iostream(this)
				, src_(rv.src_)
			{
			}

			int
			StdStreamExt::overflow(int c)
			{
				uint8_t byte = (uint8_t)c;
				if (src_->write(&byte, 1))
					return c;
				else
					return EOF;
			}

			int
			StdStreamExt::underflow()
			{
				uint8_t byte;
				if (src_->read(&byte, 1))
					return byte;
				else
					return EOF;
			}
		}

		const char*
		stream::last_hint()
		{
			return hint_;
		}

		detail::StdStreamExt
		stream::as_std()
		{
			return detail::StdStreamExt(this);
		}
	}
}