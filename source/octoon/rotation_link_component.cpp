#include <octoon/rotation_link_component.h>
#include <octoon/transform_component.h>
#include <iostream>
namespace octoon
{
	OctoonImplementSubClass(RotationLinkComponent, GameComponent, "RotationLimit")

	RotationLinkComponent::RotationLinkComponent() noexcept
		: additiveMoveRatio_(0.0f)
		, additiveRotationRatio_(0.0f)
		, additiveUseLocal_(false)
		, rotation_(math::Quaternion::Zero)
		, localRotation_(math::Quaternion::Zero)
	{
	}

	GameComponentPtr
	RotationLinkComponent::clone() const noexcept
	{
		auto instance = std::make_shared<RotationLinkComponent>();
		instance->setName(this->getName());
		return instance;
	}

	void 
	RotationLinkComponent::setAdditiveMoveRatio(float ratio) noexcept
	{
		additiveMoveRatio_ = ratio;
	}

	void
	RotationLinkComponent::setAdditiveUseLocal(bool enable) noexcept
	{
		additiveUseLocal_ = enable;
	}

	void 
	RotationLinkComponent::setAdditiveRotationRatio(float ratio) noexcept
	{
		additiveRotationRatio_ = ratio;
	}

	bool
	RotationLinkComponent::getAdditiveUseLocal() const noexcept
	{
		return additiveUseLocal_;
	}

	float
	RotationLinkComponent::getAdditiveMoveRatio() const noexcept
	{
		return additiveMoveRatio_;
	}

	float
	RotationLinkComponent::getAdditiveRotationRatio() const noexcept
	{
		return additiveRotationRatio_;
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
	RotationLinkComponent::onActivate() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		translate_ = transform->getTranslate();
		rotation_ = transform->getQuaternion();
		localTranslate_ = transform->getLocalTranslate();
		localRotation_ = transform->getLocalQuaternion();
	}

	void
	RotationLinkComponent::solve() noexcept
	{
		if (bones_.empty())
			return;

		for (auto& bone : bones_)
		{
			auto transform = bone->getComponent<TransformComponent>();
			auto boneController = transform->getComponent<RotationLinkComponent>();

			auto additiveTranslate = this->deltaTranslate(boneController->getAdditiveUseLocal());
			if (boneController->getAdditiveMoveRatio() != 0.0f)
			{
				transform->getComponent<TransformComponent>()->setLocalTranslateAccum(additiveTranslate * boneController->getAdditiveMoveRatio());
			}

			auto additiveRotation = this->deltaRotation(boneController->getAdditiveUseLocal());
			if (!math::equal(additiveRotation, math::Quaternion::Zero))
			{
				if (boneController->getAdditiveRotationRatio() > 0.0)
				{
					auto rotation = math::slerp(math::Quaternion::Zero, additiveRotation, boneController->getAdditiveRotationRatio());
					transform->setLocalQuaternionAccum(rotation);
				}
				else if (boneController->getAdditiveRotationRatio() < 0.0)
				{
					auto rotation = math::slerp(math::Quaternion::Zero, math::inverse(additiveRotation), -boneController->getAdditiveRotationRatio());
					transform->setLocalQuaternionAccum(rotation);
				}
			}
		}
	}

	math::float3
	RotationLinkComponent::deltaTranslate(bool useLocal) noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		return useLocal ?
			transform->getLocalTranslate() - localTranslate_ :
			transform->getTranslate() - translate_;
	}

	math::Quaternion
	RotationLinkComponent::deltaRotation(bool useLocal) noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		return useLocal ?
			math::inverse(localRotation_) * transform->getLocalQuaternion() :
			math::inverse(rotation_) * transform->getQuaternion();
	}
}