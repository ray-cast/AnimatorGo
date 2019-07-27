#include <octoon/bone_controller_component.h>
#include <octoon/transform_component.h>
#include <iostream>
namespace octoon
{
	OctoonImplementSubClass(BoneControllerComponent, GameComponent, "RotationLimit")

	BoneControllerComponent::BoneControllerComponent() noexcept
		: additiveMoveRatio_(0.0f)
		, additiveRotationRatio_(0.0f)
		, additiveUseLocal_(false)
		, rotation_(math::Quaternion::Zero)
		, localRotation_(math::Quaternion::Zero)
	{
	}

	GameComponentPtr
	BoneControllerComponent::clone() const noexcept
	{
		auto instance = std::make_shared<BoneControllerComponent>();
		instance->setName(this->getName());
		return instance;
	}

	void 
	BoneControllerComponent::setAdditiveMoveRatio(float ratio) noexcept
	{
		additiveMoveRatio_ = ratio;
	}

	void
	BoneControllerComponent::setAdditiveUseLocal(bool enable) noexcept
	{
		additiveUseLocal_ = enable;
	}

	void 
	BoneControllerComponent::setAdditiveRotationRatio(float ratio) noexcept
	{
		additiveRotationRatio_ = ratio;
	}

	bool
	BoneControllerComponent::getAdditiveUseLocal() const noexcept
	{
		return additiveUseLocal_;
	}

	float
	BoneControllerComponent::getAdditiveMoveRatio() const noexcept
	{
		return additiveMoveRatio_;
	}

	float
	BoneControllerComponent::getAdditiveRotationRatio() const noexcept
	{
		return additiveRotationRatio_;
	}

	void
	BoneControllerComponent::addBone(GameObjectPtr&& joint) noexcept
	{
		bones_.emplace_back(std::move(joint));
	}

	void
	BoneControllerComponent::addBone(const GameObjectPtr& joint) noexcept
	{
		bones_.push_back(joint);
	}

	void
	BoneControllerComponent::setBones(GameObjects&& bones) noexcept
	{
		bones_ = std::move(bones);
	}

	void
	BoneControllerComponent::setBones(const GameObjects& bones) noexcept
	{
		bones_ = bones;
	}

	const GameObjects&
	BoneControllerComponent::getBones() const noexcept
	{
		return bones_;
	}

	void
	BoneControllerComponent::onActivate() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		translate_ = transform->getTranslate();
		rotation_ = transform->getQuaternion();
		localTranslate_ = transform->getLocalTranslate();
		localRotation_ = transform->getLocalQuaternion();
	}

	void
	BoneControllerComponent::solve() noexcept
	{
		if (bones_.empty())
			return;

		for (auto& bone : bones_)
		{
			auto transform = bone->getComponent<TransformComponent>();
			auto boneController = transform->getComponent<BoneControllerComponent>();

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
	BoneControllerComponent::deltaTranslate(bool useLocal) noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		return useLocal ?
			transform->getLocalTranslate() - localTranslate_ :
			transform->getTranslate() - translate_;
	}

	math::Quaternion
	BoneControllerComponent::deltaRotation(bool useLocal) noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		return useLocal ?
			math::inverse(localRotation_) * transform->getLocalQuaternion() :
			math::inverse(rotation_) * transform->getQuaternion();
	}
}