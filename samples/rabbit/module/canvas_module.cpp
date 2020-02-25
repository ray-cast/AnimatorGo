#include "canvas_module.h"

namespace rabbit
{
	CanvasModule::CanvasModule() noexcept
	{
		this->reset();
	}

	CanvasModule::~CanvasModule() noexcept
	{
	}

	void
	CanvasModule::reset() noexcept
	{
		this->width = 1280;
		this->height = 720;

		this->albedoBuffer.resize(this->width * this->height);
		this->normalBuffer.resize(this->width * this->height);
		this->colorBuffer.resize(this->width * this->height);
		this->outputBuffer.resize(this->width * this->height);
	}

	void 
	CanvasModule::load(octoon::runtime::json& reader) noexcept
	{
		this->width = reader["width"];
		this->height = reader["height"];

		this->albedoBuffer.resize(this->width * this->height);
		this->normalBuffer.resize(this->width * this->height);
		this->colorBuffer.resize(this->width * this->height);
		this->outputBuffer.resize(this->width * this->height);
	}

	void 
	CanvasModule::save(octoon::runtime::json& reader) noexcept
	{
		reader["width"] = this->width;
		reader["height"] = this->height;
	}
}