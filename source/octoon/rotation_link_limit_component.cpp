#include <octoon/rotation_link_limit_component.h>

namespace octoon
{
	OctoonImplementSubClass(RotationLinkLimitComponent, GameComponent, "RotationLimit")

	RotationLinkLimitComponent::RotationLinkLimitComponent() noexcept
		: additiveMoveRatio_(0.0f)
		, additiveRotationRatio_(0.0f)
		, additiveUseLocal_(false)
	{
	}

	RotationLinkLimitComponent::RotationLinkLimitComponent(float move, float rotation, bool useLocal) noexcept
		: additiveMoveRatio_(move)
		, additiveRotationRatio_(rotation)
		, additiveUseLocal_(useLocal)
	{
	}

	void 
	RotationLinkLimitComponent::setAdditiveMoveRatio(float ratio) noexcept
	{
		additiveMoveRatio_ = ratio;
	}

	void
	RotationLinkLimitComponent::setAdditiveUseLocal(bool enable) noexcept
	{
		additiveUseLocal_ = enable;
	}

	void 
	RotationLinkLimitComponent::setAdditiveRotationRatio(float ratio) noexcept
	{
		additiveRotationRatio_ = ratio;
	}

	bool
	RotationLinkLimitComponent::getAdditiveUseLocal() const noexcept
	{
		return additiveUseLocal_;
	}

	float
	RotationLinkLimitComponent::getAdditiveMoveRatio() const noexcept
	{
		return additiveMoveRatio_;
	}

	float
	RotationLinkLimitComponent::getAdditiveRotationRatio() const noexcept
	{
		return additiveRotationRatio_;
	}

	GameComponentPtr
	RotationLinkLimitComponent::clone() const noexcept
	{
		auto instance = std::make_shared<RotationLinkLimitComponent>();
		instance->setName(this->getName());
		instance->setAdditiveMoveRatio(this->getAdditiveMoveRatio());
		instance->setAdditiveRotationRatio(this->getAdditiveRotationRatio());
		instance->setAdditiveUseLocal(this->getAdditiveUseLocal());
		return instance;
	}
}