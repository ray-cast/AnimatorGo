#include "denoise_module.h"

namespace MysticLit
{
	DenoiseModule::DenoiseModule() noexcept
	{
		this->reset();
	}

	DenoiseModule::~DenoiseModule() noexcept
	{
	}

	void
	DenoiseModule::reset() noexcept
	{
		this->active_ = false;
		this->hdr = false;
		this->srgb = true;
	}
}