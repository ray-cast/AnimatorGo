// File: virtual_dirs.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_FARCHIVE_H_
#define OCTOON_IO_FARCHIVE_H_

#include <octoon/io/ioserver.h>

namespace octoon
{
	namespace io
	{
		/*
		* Local directory mapped directly to a virtual directory.
		*/
		class OCTOON_EXPORT farchive : public archive
		{
		public:
			farchive(const std::string& base_dir) noexcept;
			std::unique_ptr<stream> open(const Orl& orl, const ios_base::open_mode mode) override;

			bool remove(const Orl& orl, ItemType type = ItemType::File) override;
			ItemType exists(const Orl& orl) override;

		private:
			std::string make_path(const Orl& orl) const;
			std::string base_dir_;
		};

		using LocalDirPtr = std::shared_ptr<farchive>;
	}
}

#endif