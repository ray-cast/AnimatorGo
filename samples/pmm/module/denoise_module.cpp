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

	void 
	DenoiseModule::load(octoon::runtime::json& reader) noexcept
	{
		this->hdr = reader["hdr"];
		this->srgb = reader["srgb"];
	}

	void 
	DenoiseModule::save(octoon::runtime::json& reader) noexcept
	{
		reader["hdr"] = this->hdr;
		reader["srgb"] = this->srgb;
	}
}