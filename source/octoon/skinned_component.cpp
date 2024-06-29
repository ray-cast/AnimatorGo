#include <octoon/skinned_component.h>

namespace octoon
{
	OctoonImplementSubInterface(SkinnedComponent, GameComponent, "Skinned")

	SkinnedComponent::SkinnedComponent() noexcept
		: control_(0)
	{
	}

	SkinnedComponent::~SkinnedComponent() noexcept
	{
	}

	void
	SkinnedComponent::setName(std::string_view name) noexcept
	{
		if (this->getName() != name)
		{
			this->onTargetReplace(name);
			GameComponent::setName(name);
		}
	}

	const std::string&
	SkinnedComponent::getName() const noexcept
	{
		return GameComponent::getName();
	}

	void
	SkinnedComponent::setControl(float control) noexcept
	{
		if (control_ != control)
		{
			this->sendMessage("octoon:animation:update");
			control_ = control;
		}
	}

	float
	SkinnedComponent::getControl() const noexcept
	{
		return control_;
	}

	void
	SkinnedComponent::onTargetReplace(std::string_view name) noexcept
	{
	}
}