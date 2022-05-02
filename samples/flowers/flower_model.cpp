#include "flower_model.h"

namespace flower
{
	FlowerModule::FlowerModule() noexcept
		: enable_(true)
	{
	}

	FlowerModule::~FlowerModule() noexcept
	{
	}

	void
	FlowerModule::setEnable(bool enable) noexcept
	{
		enable_ = enable;
	}

	bool
	FlowerModule::getEnable() const noexcept
	{
		return enable_;
	}

	void
	FlowerModule::onValidate() noexcept
	{
	}
}