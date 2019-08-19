#include "entities_module.h"

namespace MysticLit
{
	EntitiesModule::EntitiesModule() noexcept
	{
		this->reset();
	}

	EntitiesModule::~EntitiesModule() noexcept
	{
	}

	void
	EntitiesModule::reset() noexcept
	{
		this->camera.reset();
		this->objects.clear();
		this->objects.shrink_to_fit();
	}
}