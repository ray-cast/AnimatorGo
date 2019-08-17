#include <octoon/skinned_component.h>

namespace octoon
{
	OctoonImplementSubInterface(SkinnedComponent, GameComponent, "Skinned")

	SkinnedComponent::SkinnedComponent() noexcept
		: control_(0.0f)
	{
	}

	SkinnedComponent::~SkinnedComponent() noexcept
	{
	}

	void
	SkinnedComponent::setName(const std::string& name) noexcept
	{
		if (this->getName() != name)
		{
			this->onTargetReplace(name);
			GameComponent::setName(name);
		}
	}

	void
	SkinnedComponent::setName(std::string&& name) noexcept
	{
		if (this->getName() != name)
		{
			this->onTargetReplace(name);
			GameComponent::setName(std::move(name));
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
	SkinnedComponent::onTargetReplace(const std::string& name) noexcept
	{
	}
}