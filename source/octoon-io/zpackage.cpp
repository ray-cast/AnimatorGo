// File: virtual_dirs.h
// Author: PENGUINLIONG
#include <cassert>
#include <zipper/unzipper.h>

#include <octoon/io/zpackage.h>
#include <octoon/io/membuf.h>

namespace octoon
{
	namespace io
	{
		zpackage::zpackage(const char* zip_file) except
			: unzipper_(nullptr)
			, entries_(nullptr)
		{
			unzipper_ = new zipper::Unzipper(zip_file);
			entries_ = new std::vector<zipper::ZipEntry>(std::move(((zipper::Unzipper*)unzipper_)->entries()));
		}

		zpackage::zpackage(std::string&& zip_file) except
			: unzipper_(nullptr)
			, entries_(nullptr)
		{
			unzipper_ = new zipper::Unzipper(zip_file);
			entries_ = new std::vector<zipper::ZipEntry>(std::move(((zipper::Unzipper*)unzipper_)->entries()));
		}

		zpackage::zpackage(const std::string& zip_file) except
			: unzipper_(nullptr)
			, entries_(nullptr)
		{
			unzipper_ = new zipper::Unzipper(zip_file);
			entries_ = new std::vector<zipper::ZipEntry>(std::move(((zipper::Unzipper*)unzipper_)->entries()));
		}

		zpackage::~zpackage()
		{
			delete ((zipper::Unzipper*)unzipper_);
			delete ((std::vector<zipper::ZipEntry>*)entries_);
		}

		std::unique_ptr<stream_buf>
		zpackage::open(const Orl& orl, const ios_base::open_mode opts)
		{
			std::vector<uint8_t> buf;

			// Zip archives are read-only.
			if (opts & ios_base::out)
				return nullptr;

			if (unzipper_ == nullptr)
				return nullptr;

			if (!reinterpret_cast<zipper::Unzipper*>(unzipper_)->extractEntryToMemory(orl.path(), buf))
				return nullptr;

			return std::make_unique<membuf>(buf);
		}

		bool
		zpackage::remove(const Orl& orl, ios_base::file_type type)
		{
			return false;
		}

		ios_base::file_type
		zpackage::exists(const Orl& orl)
		{
			if (entries_ == nullptr)
				return ios_base::none;

			auto size = orl.path().size();
			for (auto entry : *(std::vector<zipper::ZipEntry>*)entries_)
			{
				if (entry.name.size() == size)
				{
					if (entry.name == orl.path())
						return ios_base::file;
				}
				else if (entry.name.size() == size + 1)
				{
					if (orl.path().compare(entry.name) == 0 && entry.name.back() == '/')
						return ios_base::directory;
				}
			}

			return ios_base::none;
		}
	}
}