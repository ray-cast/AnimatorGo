#include <octoon/rotation_link_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(RotationLinkComponent, GameComponent, "RotationLink")

	RotationLinkComponent::RotationLinkComponent() noexcept
		: translate_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, localTranslate_(math::float3::Zero)
		, localRotation_(math::Quaternion::Zero)
	{
	}

	RotationLinkComponent::RotationLinkComponent(const GameObjects& bones) noexcept
		: translate_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, localTranslate_(math::float3::Zero)
		, localRotation_(math::Quaternion::Zero)
		, bones_(bones)
	{
	}

	RotationLinkComponent::RotationLinkComponent(const GameObjectPtr& bone) noexcept
		: translate_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, localTranslate_(math::float3::Zero)
		, localRotation_(math::Quaternion::Zero)
	{
		bones_.push_back(bone);
	}

	void
	RotationLinkComponent::addBone(GameObjectPtr&& joint) noexcept
	{
		bones_.emplace_back(std::move(joint));
	}

	void
	RotationLinkComponent::addBone(const GameObjectPtr& joint) noexcept
	{
		bones_.push_back(joint);
	}

	void
	RotationLinkComponent::setBones(GameObjects&& bones) noexcept
	{
		bones_ = std::move(bones);
	}

	void
	RotationLinkComponent::setBones(const GameObjects& bones) noexcept
	{
		bones_ = bones;
	}

	const GameObjects&
	RotationLinkComponent::getBones() const noexcept
	{
		return bones_;
	}

	void
	RotationLinkComponent::setTranslate(const math::float3& translate) noexcept
	{
		this->translate_ = translate;
	}

	void
	RotationLinkComponent::setLocalTranslate(const math::float3& translate) noexcept
	{
		this->localTranslate_ = translate;
	}

	void
	RotationLinkComponent::setRotation(const math::Quaternion& quat) noexcept
	{
		this->rotation_ = quat;
	}

	void
	RotationLinkComponent::setLocalQuaternion(const math::Quaternion& quat) noexcept
	{
		this->localRotation_ = quat;
	}

	const math::float3&
	RotationLinkComponent::getTranslate() const noexcept
	{
		return this->translate_;
	}

	const math::float3&
	RotationLinkComponent::getLocalTranslate() const noexcept
	{
		return this->localTranslate_;
	}

	const math::Quaternion&
	RotationLinkComponent::getRotation() const noexcept
	{
		return this->rotation_;
	}

	const math::Quaternion&
	RotationLinkComponent::getLocalQuaternion() const noexcept
	{
		return this->localRotation_;
	}

	math::float3
	RotationLinkComponent::getDeltaTranslate(bool useLocal) const noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		return useLocal ?
			transform->getLocalTranslate() - localTranslate_ :
			transform->getTranslate() - translate_;
	}

	math::Quaternion
	RotationLinkComponent::getDeltaRotation(bool useLocal) const noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		return useLocal ?
			math::inverse(localRotation_) * transform->getLocalQuaternion() :
			math::inverse(rotation_) * transform->getRotation();
	}

	GameComponentPtr
	RotationLinkComponent::clone() const noexcept
	{
		auto instance = std::make_shared<RotationLinkComponent>();
		instance->setName(this->getName());
		instance->setBones(this->getBones());
		instance->setTranslate(this->getTranslate());
		instance->setRotation(this->getRotation());
		instance->setLocalTranslate(this->getLocalTranslate());
		instance->setLocalQuaternion(this->getLocalQuaternion());

		return instance;
	}
}