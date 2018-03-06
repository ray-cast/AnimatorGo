#include <octoon/io/virtual_buf.h>
#include <octoon/io/ioserver.h>

namespace octoon
{
	namespace io
	{
		virtual_buf::virtual_buf() noexcept
		{
		}

		virtual_buf::~virtual_buf() noexcept
		{
		}

		bool
		virtual_buf::is_open() const noexcept
		{
			return buf_ ? buf_->is_open() : false;
		}

		bool
		virtual_buf::open(IoServer* filesystem, const Orl& orl, const ios_base::open_mode mode) noexcept
		{
			this->close();

			auto vdir = filesystem->get_archive(orl);
			if (vdir == nullptr)
				return false;

			buf_ = vdir->open(orl, mode);
			return buf_ != nullptr;
		}

		void
		virtual_buf::close()  noexcept
		{
			if (buf_)
				buf_.reset();
		}

		streamsize
		virtual_buf::read(char* str, std::streamsize cnt) noexcept
		{
			return buf_ ? buf_->read(str, cnt) : 0;
		}
		streamsize
		virtual_buf::write(const char* str, std::streamsize cnt) noexcept
		{
			return buf_ ? buf_->write(str, cnt) : 0;
		}

		streamoff
		virtual_buf::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
		{
			return buf_ ? buf_->seekg(pos, dir) : 0;
		}

		streamoff
		virtual_buf::tellg() noexcept
		{
			return buf_ ? buf_->tellg() : 0;
		}

		streamsize
		virtual_buf::size() const noexcept
		{
			return buf_ ? buf_->size() : 0;
		}

		int
		virtual_buf::flush() noexcept
		{
			return buf_ ? buf_->flush() : 0;
		}
	}
}