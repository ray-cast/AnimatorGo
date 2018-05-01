// File: stream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_STREAM_H_
#define OCTOON_IO_STREAM_H_
#include <cstdint>
#include <vector>
#include <iostream>

#include <octoon/io/iosbase.h>
#include <octoon/io/stream_buf.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT stream : public ios_base
		{
		public:
			stream() noexcept = default;
			virtual ~stream() noexcept = default;

			stream_buf* rdbuf() const noexcept;
			void set_rdbuf(stream_buf* buf) noexcept;

		protected:
			void _init(stream_buf* _buf, ios_base::openmode mode) noexcept;

		private:
			stream_buf * _strbuf;
		};
	}
}

#endif
