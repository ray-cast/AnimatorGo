#include <octoon/rotation_link_limit_component.h>

namespace octoon
{
	OctoonImplementSubClass(RotationLinkLimitComponent, GameComponent, "RotationLinkLimit")

	RotationLinkLimitComponent::RotationLinkLimitComponent() noexcept
		: additiveMoveRatio_(0.0f)
		, additiveRotationRatio_(0.0f)
		, additiveUseLocal_(false)
		, translate_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, localTranslate_(math::float3::Zero)
		, localRotation_(math::Quaternion::Zero)
	{
	}

	RotationLinkLimitComponent::RotationLinkLimitComponent(float move, float rotation, bool useLocal) noexcept
		: additiveMoveRatio_(move)
		, additiveRotationRatio_(rotation)
		, additiveUseLocal_(useLocal)
		, translate_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, localTranslate_(math::float3::Zero)
		, localRotation_(math::Quaternion::Zero)
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

	void
	RotationLinkLimitComponent::setTranslate(const math::float3& translate) noexcept
	{
		this->translate_ = translate;
	}

	void
	RotationLinkLimitComponent::setLocalTranslate(const math::float3& translate) noexcept
	{
		this->localTranslate_ = translate;
	}

	void
	RotationLinkLimitComponent::setRotation(const math::Quaternion& quat) noexcept
	{
		this->rotation_ = quat;
	}

	void
	RotationLinkLimitComponent::setLocalQuaternion(const math::Quaternion& quat) noexcept
	{
		this->localRotation_ = quat;
	}

	const math::float3&
	RotationLinkLimitComponent::getTranslate() const noexcept
	{
		return this->translate_;
	}

	const math::float3&
	RotationLinkLimitComponent::getLocalTranslate() const noexcept
	{
		return this->localTranslate_;
	}

	const math::Quaternion&
	RotationLinkLimitComponent::getRotation() const noexcept
	{
		return this->rotation_;
	}

	const math::Quaternion&
	RotationLinkLimitComponent::getLocalQuaternion() const noexcept
	{
		return this->localRotation_;
	}

	GameComponentPtr
	RotationLinkLimitComponent::clone() const noexcept
	{
		auto instance = std::make_shared<RotationLinkLimitComponent>();
		instance->setName(this->getName());
		instance->setAdditiveMoveRatio(this->getAdditiveMoveRatio());
		instance->setAdditiveRotationRatio(this->getAdditiveRotationRatio());
		instance->setAdditiveUseLocal(this->getAdditiveUseLocal());
		instance->setTranslate(this->getTranslate());
		instance->setRotation(this->getRotation());
		instance->setLocalTranslate(this->getLocalTranslate());
		instance->setLocalQuaternion(this->getLocalQuaternion());

		return instance;
	}
}