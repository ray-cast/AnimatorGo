#include <octoon/configurable_joint_component.h>

#include <octoon/physics_feature.h>
#include <octoon/transform_component.h>
#include <octoon/game_app.h>
#include <octoon/game_scene.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	OctoonImplementSubInterface(ConfigurableJointComponent, JointComponent, "ConfigurableJointComponent")

	ConfigurableJointComponent::ConfigurableJointComponent() noexcept
		:joint(), thisBody(), connectedBody()
	{
	}

	ConfigurableJointComponent::~ConfigurableJointComponent()
	{
	}

	void ConfigurableJointComponent::onActivate() except
	{
	}

	void ConfigurableJointComponent::onDeactivate() noexcept
	{
	}

	void ConfigurableJointComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<RigidbodyComponent>())
		{
			thisBody = component->downcast_pointer<RigidbodyComponent>();
			buildJoint();
		}
	}

	void ConfigurableJointComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<RigidbodyComponent>())
		{
			thisBody.reset();
		}
	}

	std::shared_ptr<RigidbodyComponent> ConfigurableJointComponent::getConnectedBody()
	{
		return connectedBody.lock();
	}

	void ConfigurableJointComponent::setConnectedBody(std::shared_ptr<RigidbodyComponent> component)
	{
		connectedBody = component;
		buildJoint();
	}

	void ConfigurableJointComponent::buildJoint()
	{
		if (!thisBody.expired() && !connectedBody.expired())
		{
			auto physicsFeature = this->getGameObject()->getGameScene()->getFeature<PhysicsFeature>();
			auto physicsContext = physicsFeature->getContext();
			joint = physicsContext->createFixedJoint(thisBody.lock()->getRigidbody(), connectedBody.lock()->getRigidbody());
		}
	}
}