#include "mysticLit_model.h"

namespace MysticLit
{
	MysticLitModule::MysticLitModule() noexcept
		: enable_(true)
	{
	}

	MysticLitModule::~MysticLitModule() noexcept
	{
	}

	void
	MysticLitModule::setEnable(bool enable) noexcept
	{
		enable_ = enable;
	}

	bool
	MysticLitModule::getEnable() const noexcept
	{
		return enable_;
	}

	void
	MysticLitModule::onValidate() noexcept
	{
	}
}