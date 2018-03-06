// File: virtual_dirs.h
// Author: PENGUINLIONG
#include <cassert>
#include <zipper/unzipper.h>

#include <octoon/io/zarchive.h>
#include <octoon/io/membuf.h>

namespace octoon
{
	namespace io
	{
		zarchive::zarchive(const char* zip_file) except
			: unzipper_(nullptr)
			, entries_(nullptr)
		{
			unzipper_ = new zipper::Unzipper(zip_file);
			entries_ = new std::vector<zipper::ZipEntry>(std::move(((zipper::Unzipper*)unzipper_)->entries()));
		}

		zarchive::zarchive(std::string&& zip_file) except
			: unzipper_(nullptr)
			, entries_(nullptr)
		{
			unzipper_ = new zipper::Unzipper(zip_file);
			entries_ = new std::vector<zipper::ZipEntry>(std::move(((zipper::Unzipper*)unzipper_)->entries()));
		}

		zarchive::zarchive(const std::string& zip_file) except
			: unzipper_(nullptr)
			, entries_(nullptr)
		{
			unzipper_ = new zipper::Unzipper(zip_file);
			entries_ = new std::vector<zipper::ZipEntry>(std::move(((zipper::Unzipper*)unzipper_)->entries()));
		}

		zarchive::~zarchive()
		{
			delete ((zipper::Unzipper*)unzipper_);
			delete ((std::vector<zipper::ZipEntry>*)entries_);
		}

		std::unique_ptr<stream_buf>
		zarchive::open(const Orl& orl, const ios_base::open_mode opts)
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
		zarchive::remove(const Orl& orl, ItemType type)
		{
			return false;
		}

		ItemType
		zarchive::exists(const Orl& orl)
		{
			if (entries_ == nullptr)
				return ItemType::NA;

			auto size = orl.path().size();
			for (auto entry : *(std::vector<zipper::ZipEntry>*)entries_)
			{
				if (entry.name.size() == size)
				{
					if (entry.name == orl.path())
						return ItemType::File;
				}
				else if (entry.name.size() == size + 1)
				{
					if (orl.path().compare(entry.name) == 0 && entry.name.back() == '/')
						return ItemType::Directory;
				}
			}

			return ItemType::NA;
		}
	}
}