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
		:joint()
	{
	}

	FixedJointComponent::~FixedJointComponent()
	{
	}

	void FixedJointComponent::onActivate() except
	{
	}

	void FixedJointComponent::onDeactivate() noexcept
	{
	}

	void FixedJointComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<RigidbodyComponent>())
		{
			thisBody = component->downcast_pointer<RigidbodyComponent>();
			buildJoint();
		}
	}

	void FixedJointComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<RigidbodyComponent>())
		{
			thisBody.reset();
		}
	}

	std::shared_ptr<RigidbodyComponent> FixedJointComponent::getConnectedBody()
	{
		return connectedBody.lock();
	}

	void FixedJointComponent::setConnectedBody(std::shared_ptr<RigidbodyComponent> component)
	{
		connectedBody = component;
		buildJoint();
	}

	void FixedJointComponent::buildJoint()
	{
		if (!thisBody.expired() && !connectedBody.expired())
		{
			auto physicsFeature = this->getGameObject()->getGameScene()->getFeature<PhysicsFeature>();
			auto physicsContext = physicsFeature->getContext();
			joint = physicsContext->createFixedJoint(thisBody.lock()->getRigidbody(), connectedBody.lock()->getRigidbody());
		}
	}
}