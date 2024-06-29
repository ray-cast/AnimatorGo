#include "unreal_model.h"

namespace unreal
{
	UnrealModule::UnrealModule() noexcept
		: enable(true)
	{
	}

	UnrealModule::~UnrealModule() noexcept
	{
	}

	void
	UnrealModule::setEnable(bool value) noexcept
	{
		enable = value;
	}

	bool
	UnrealModule::getEnable() const noexcept
	{
		return enable;
	}

	void
	UnrealModule::onValidate() noexcept
	{
	}

	void
	UnrealModule::disconnect() noexcept
	{
		enable.disconnect();
	}
}