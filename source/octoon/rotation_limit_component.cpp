#include <octoon/rotation_limit_component.h>

namespace octoon
{
	OctoonImplementSubClass(RotationLimitComponent, GameComponent, "RotationLimit")

	RotationLimitComponent::RotationLimitComponent() noexcept
		: mininumAngle_(-math::PI)
		, maximumAngle_(math::PI)
		, minimumAxis_(math::float3::Zero)
		, maximumAxis_(math::float3::Zero)
	{
	}

	RotationLimitComponent::RotationLimitComponent(float mininumAngle, float maximumAngle, math::float3 minimumAxis, math::float3 maximumAxis) noexcept
		: mininumAngle_(mininumAngle)
		, maximumAngle_(maximumAngle)
		, minimumAxis_(minimumAxis)
		, maximumAxis_(maximumAxis)
	{
	}

	void
	RotationLimitComponent::setMininumAngle(float limit) noexcept
	{
		mininumAngle_ = limit;
	}

	void
	RotationLimitComponent::setMaximumAngle(float limit) noexcept
	{
		maximumAngle_ = limit;
	}

	void
	RotationLimitComponent::setMinimumAxis(const math::float3& limit) noexcept
	{
		minimumAxis_ = limit;
	}

	void
	RotationLimitComponent::setMaximumAxis(const math::float3& limit) noexcept
	{
		maximumAxis_ = limit;
	}

	float
	RotationLimitComponent::getMininumAngle() const noexcept
	{
		return mininumAngle_;
	}

	float
	RotationLimitComponent::getMaximumAngle() const noexcept
	{
		return maximumAngle_;
	}

	const math::float3&
	RotationLimitComponent::getMinimumAxis() const noexcept
	{
		return minimumAxis_;
	}

	const math::float3&
	RotationLimitComponent::getMaximumAxis() const noexcept
	{
		return maximumAxis_;
	}

	GameComponentPtr
	RotationLimitComponent::clone() const noexcept
	{
		auto instance = std::make_shared<RotationLimitComponent>();
		instance->setName(this->getName());
		instance->setMinimumAxis(this->getMinimumAxis());
		instance->setMaximumAxis(this->getMaximumAxis());
		instance->setMininumAngle(this->getMininumAngle());
		instance->setMaximumAngle(this->getMaximumAngle());

		return instance;
	}
}