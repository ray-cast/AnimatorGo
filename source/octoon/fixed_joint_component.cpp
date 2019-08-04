#include <octoon/fixed_joint_component.h>

#include <octoon/physics_feature.h>
#include <octoon/transform_component.h>
#include <octoon/game_app.h>
#include <octoon/game_scene.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	OctoonImplementSubInterface(FixedJointComponent, JointComponent, "FixedJointComponent")

	FixedJointComponent::FixedJointComponent() noexcept
		:joint(), thisBody(), target_()
	{
	}

	FixedJointComponent::~FixedJointComponent()
	{
	}

	void
	FixedJointComponent::onActivate() except
	{
	}

	void
	FixedJointComponent::onDeactivate() noexcept
	{
	}

	void
	FixedJointComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<RigidbodyComponent>())
		{
			thisBody = component->downcast_pointer<RigidbodyComponent>();
			setupFixedJoint();
		}
	}

	void
	FixedJointComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<RigidbodyComponent>())
		{
			thisBody.reset();
		}
	}

	std::shared_ptr<RigidbodyComponent>
	FixedJointComponent::getTarget()
	{
		return target_.lock();
	}

	void
	FixedJointComponent::setTarget(std::shared_ptr<RigidbodyComponent> component)
	{
		target_ = component;
	}

	void FixedJointComponent::setupFixedJoint()
	{
		if (!thisBody.expired() && !target_.expired())
		{
			auto physicsFeature = this->getGameScene()->getFeature<PhysicsFeature>();
			auto physicsContext = physicsFeature->getContext();
			joint = physicsContext->createFixedJoint(thisBody.lock()->getRigidbody(), target_.lock()->getRigidbody());
		}
	}
}