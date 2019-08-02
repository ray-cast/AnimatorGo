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

	void ConfigurableJointComponent::setAngularXMotion(ConfigurableJointMotion motion)
	{
		if (joint)
		{
			joint->setAngularXMotion(motion);
		}
	}

	ConfigurableJointMotion ConfigurableJointComponent::getAngularXMotion()
	{
		if (joint)
		{
			return joint->getAngularXMotion();
		}
		else
		{
			return ConfigurableJointMotion::Locked;
		}
	}

	void ConfigurableJointComponent::setAngularYMotion(ConfigurableJointMotion motion)
	{
		if (joint)
		{
			joint->setAngularYMotion(motion);
		}
	}

	ConfigurableJointMotion ConfigurableJointComponent::getAngularYMotion()
	{
		if (joint)
		{
			return joint->getAngularYMotion();
		}
		else
		{
			return ConfigurableJointMotion::Locked;
		}
	}

	void ConfigurableJointComponent::setAngularZMotion(ConfigurableJointMotion motion)
	{
		if (joint)
		{
			joint->setAngularZMotion(motion);
		}
	}

	ConfigurableJointMotion ConfigurableJointComponent::getAngularZMotion()
	{
		if (joint)
		{
			return joint->getAngularZMotion();
		}
		else
		{
			return ConfigurableJointMotion::Locked;
		}
	}

	void ConfigurableJointComponent::setXMotion(ConfigurableJointMotion motion)
	{
		if (joint)
		{
			joint->setXMotion(motion);
		}
	}

	ConfigurableJointMotion ConfigurableJointComponent::getXMotion()
	{
		if (joint)
		{
			return joint->getXMotion();
		}
		else
		{
			return ConfigurableJointMotion::Locked;
		}
	}

	void ConfigurableJointComponent::setYMotion(ConfigurableJointMotion motion)
	{
		if (joint)
		{
			joint->setYMotion(motion);
		}
	}

	ConfigurableJointMotion ConfigurableJointComponent::getYMotion()
	{
		if (joint)
		{
			return joint->getYMotion();
		}
		else
		{
			return ConfigurableJointMotion::Locked;
		}
	}

	void ConfigurableJointComponent::setZMotion(ConfigurableJointMotion motion)
	{
		if (joint)
		{
			joint->setZMotion(motion);
		}
	}

	ConfigurableJointMotion ConfigurableJointComponent::getZMotion()
	{
		if (joint)
		{
			return joint->getZMotion();
		}
		else
		{
			return ConfigurableJointMotion::Locked;
		}
	}

	void ConfigurableJointComponent::buildJoint()
	{
		if (!thisBody.expired() && !connectedBody.expired())
		{
			auto physicsFeature = this->getGameScene()->getFeature<PhysicsFeature>();
			auto physicsContext = physicsFeature->getContext();
			joint = physicsContext->createConfigurableJoint(thisBody.lock()->getRigidbody(), connectedBody.lock()->getRigidbody());
		}
	}
}