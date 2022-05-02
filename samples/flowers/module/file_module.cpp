#include "file_module.h"

namespace rabbit
{
	FileModule::FileModule() noexcept
	{
		this->reset();
	}

	FileModule::~FileModule() noexcept
	{
	}

	void
	FileModule::reset() noexcept
	{
		this->PATHLIMIT = 4096;

		this->projectExtensions = { "pmm" };
		this->modelExtensions = { "pmx" };
		this->imageExtensions = { "png", "jpg" };
		this->videoExtensions = { "264" };
		this->hdriExtensions = { "hdr", "hdri" };
	}

	void 
	FileModule::load(octoon::runtime::json& reader) noexcept
	{
	}

	void 
	FileModule::save(octoon::runtime::json& writer) noexcept
	{
	}
}