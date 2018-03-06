#ifndef OCTOON_ISTREAM_H_
#define OCTOON_ISTREAM_H_

#include <octoon/io/ostream.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT istream : public virtual stream
		{
		public:
			istream(stream_buf* buf) noexcept;
			virtual ~istream() noexcept;

			istream& read(char* str, std::streamsize cnt) noexcept;
			istream& read(char* str, streamsize size, streamsize cnt) noexcept;

			istream& seekg(ios_base::off_type pos) noexcept;
			istream& seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;

			istream& flush() noexcept;

			streamsize size() noexcept;

			streamoff tellg() noexcept;

			streamsize gcount() const noexcept;

		protected:
			class isentry final
			{
			public:
				isentry(istream* _istr);
				~isentry() noexcept;

				operator bool() const noexcept;

			private:
				isentry(const isentry&) noexcept = delete;
				const isentry& operator=(const isentry&) noexcept = delete;

			private:
				bool _ok;
				istream* _my_istr;
			};

		private:
			istream & operator=(const istream&) = delete;
			istream(const istream&) = delete;

		private:
			streamsize _count;
		};
	}
}

#endif