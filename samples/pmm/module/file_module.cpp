#include "file_module.h"

namespace MysticLit
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
	}
}