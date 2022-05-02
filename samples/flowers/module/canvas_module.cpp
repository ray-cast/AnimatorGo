#include "canvas_module.h"

namespace flower
{
	RecordModule::RecordModule() noexcept
	{
		this->reset();
	}

	RecordModule::~RecordModule() noexcept
	{
	}

	void
	RecordModule::reset() noexcept
	{
		this->width = 1280;
		this->height = 720;

		this->hdr = false;
		this->srgb = true;

		this->albedoBuffer.resize(this->width * this->height);
		this->normalBuffer.resize(this->width * this->height);
		this->colorBuffer.resize(this->width * this->height);
		this->denoiseBuffer.resize(this->width * this->height);
	}

	void
	RecordModule::resize(std::uint32_t w, std::uint32_t h) noexcept
	{
		this->width = w;
		this->height = h;

		this->hdr = false;
		this->srgb = true;

		this->albedoBuffer.resize(this->width * this->height);
		this->normalBuffer.resize(this->width * this->height);
		this->colorBuffer.resize(this->width * this->height);
		this->denoiseBuffer.resize(this->width * this->height);
	}

	void 
	RecordModule::load(octoon::runtime::json& reader) noexcept
	{
		this->width = reader["width"];
		this->height = reader["height"];

		this->hdr = reader["hdr"];
		this->srgb = reader["srgb"];

		this->albedoBuffer.resize(this->width * this->height);
		this->normalBuffer.resize(this->width * this->height);
		this->colorBuffer.resize(this->width * this->height);
		this->denoiseBuffer.resize(this->width * this->height);
	}

	void 
	RecordModule::save(octoon::runtime::json& reader) noexcept
	{
		reader["width"] = this->width;
		reader["height"] = this->height;

		reader["hdr"] = this->hdr;
		reader["srgb"] = this->srgb;
	}
}