#include "rabbit_model.h"

namespace rabbit
{
	RabbitModule::RabbitModule() noexcept
		: enable_(true)
	{
	}

	RabbitModule::~RabbitModule() noexcept
	{
	}

	void
	RabbitModule::setEnable(bool enable) noexcept
	{
		enable_ = enable;
	}

	bool
	RabbitModule::getEnable() const noexcept
	{
		return enable_;
	}

	void
	RabbitModule::onValidate() noexcept
	{
	}
}