// File: IoServer.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_ARCHIVE_H_
#define OCTOON_IO_ARCHIVE_H_

#include <memory>

#include <octoon/io/ori.h>
#include <octoon/io/istream.h>

namespace octoon
{
	namespace io
	{
		/*
		* File system item types supported by `octoon-io`. This type is used to ensure
		* all FS operations are adopted as expected. `NA` stands for 'not available',
		* meaning that the file was not found or of a type out of capability of
		* `octoon-io`.
		*/
		enum class ItemType
		{
			File,
			Directory,
			NA
		};

		/*
		* A virtual directory in `IoServer`. Different variants of virtual
		* directories are distinguished by URI scheme.
		*/
		class OCTOON_EXPORT archive
		{
		public:
			archive() noexcept = default;
			virtual ~archive() = default;

			/*
			* Open a file in current virtual directory. For writing, all hierarchy of
			* absent directory will be created. This operation targets files only.
			*
			* Returns:
			*   A pointer to general stream created if succeeded. `nullptr` otherwise.
			*
			*/
			virtual std::unique_ptr<stream_buf> open(const Orl& orl, const ios_base::open_mode mode) = 0;

			/*
			* Remove an item in current virtual directory if and only if it has the
			* corresponding `type`. Type `NA` is always rejected.
			*
			* Returns:
			*   `true` if the file located at `orl` is successfully removed or the file
			*   doesn't exist.
			*/
			virtual bool remove(const Orl& orl, ItemType type = ItemType::File) = 0;

			/*
			* Test if a file exists in current virtual directory.
			*
			* Returns:
			*   Type of file found via `orl`. `Unknown` if the item doesn't exist.
			*/
			virtual ItemType exists(const Orl& orl) = 0;
		};

		using archive_pointer = std::shared_ptr<archive>;
	}
}

#endif