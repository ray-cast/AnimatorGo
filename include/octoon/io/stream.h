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
		/*
		 * Base of all Octoon stream types. Provide fundamental I/O functionalities.
		 */
		class OCTOON_EXPORT stream : public ios_base
		{
		public:
		  /*
		   * Report the current stream's capability of reading.
		   */
		  virtual bool can_read() const noexcept = 0;
		  /*
		   * Report the current stream's capability of writing.
		   */
		  virtual bool can_write() const noexcept = 0;
		  /*
		   * Report the current stream's capability of seeking.
		   */
		  virtual bool can_seek() const noexcept = 0;

		  /*
		   * Read from source of the current stream and write all written data into
		   * provided buffer `buf`. The number of bytes written is AT MOST `size`; but
		   * the actual number can be less.
		   *
		   * Returns:
		   *   Number of bytes read.
		   */
		  virtual size_t read(uint8_t* buf, size_t size) = 0;
		  /*
		   * Write to source of the current stream using data in provided buffer `buf`
		   * The number of bytes written is AT MOST `size`; but the actual number can be
		   * less.
		   *
		   * Returns:
		   *   Number of bytes written.
		   */
		  virtual size_t write(const uint8_t* buf, size_t size) = 0;
		  /*
		   * Seek in the stream. This operation will move the current position for given
		   * distance `dist`. The origin `ori` indicates the position in the stream to
		   * start seeking.
		   *
		   * Returns:
		   *   `true` if the operation was finished successfully; `false` otherwise. On
		   *   failure, the internal position will not change.
		   */
		  virtual bool seek(long dist, ios_base::seek_dir seek = ios_base::cur) = 0;

		  stream_buf* rdbuf() const noexcept;
		  void set_rdbuf(stream_buf* buf) noexcept;

		  void set_open_mode(ios_base::openmode mode) noexcept;
		  ios_base::openmode get_open_mode() const noexcept;

		protected:
			void _init(stream_buf* _buf, ios_base::openmode mode) noexcept;

		private:
			stream_buf * _strbuf;
			ios_base::openmode _mode;
		};
	}
}

#endif
