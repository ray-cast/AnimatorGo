#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(RotationLinkComponent, GameComponent, "RotationLimit")

	RotationLinkComponent::RotationLinkComponent() noexcept
		: rotation_(math::Quaternion::Zero)
		, localRotation_(math::Quaternion::Zero)
	{
	}

	RotationLinkComponent::RotationLinkComponent(const GameObjects& bones) noexcept
		: rotation_(math::Quaternion::Zero)
		, localRotation_(math::Quaternion::Zero)
		, bones_(bones)
	{
	}

	RotationLinkComponent::RotationLinkComponent(const GameObjectPtr& bone) noexcept
		: rotation_(math::Quaternion::Zero)
		, localRotation_(math::Quaternion::Zero)
	{
		bones_.push_back(bone);
	}

	GameComponentPtr
	RotationLinkComponent::clone() const noexcept
	{
		auto instance = std::make_shared<RotationLinkComponent>();
		instance->setName(this->getName());
		instance->setBones(this->getBones());
		return instance;
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
			auto rotationLimit = transform->getComponent<RotationLinkLimitComponent>();

			auto additiveTranslate = this->deltaTranslate(rotationLimit->getAdditiveUseLocal());
			if (rotationLimit->getAdditiveMoveRatio() != 0.0f)
			{
				transform->getComponent<TransformComponent>()->setLocalTranslateAccum(additiveTranslate * rotationLimit->getAdditiveMoveRatio());
			}

			if (rotationLimit->getAdditiveRotationRatio() != 0.0)
			{
				auto additiveRotation = this->deltaRotation(rotationLimit->getAdditiveUseLocal());
				if (!math::equal(additiveRotation, math::Quaternion::Zero))
				{
					if (rotationLimit->getAdditiveRotationRatio() > 0.0)
					{
						auto rotation = math::slerp(math::Quaternion::Zero, additiveRotation, rotationLimit->getAdditiveRotationRatio());
						transform->setLocalQuaternionAccum(rotation);
					}
					else if (rotationLimit->getAdditiveRotationRatio() < 0.0)
					{
						auto rotation = math::slerp(math::Quaternion::Zero, math::inverse(additiveRotation), -rotationLimit->getAdditiveRotationRatio());
						transform->setLocalQuaternionAccum(rotation);
					}
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