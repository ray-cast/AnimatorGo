#include "canvas_module.h"

namespace MysticLit
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
}