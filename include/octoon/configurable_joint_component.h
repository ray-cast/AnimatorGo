#ifndef OCTOON_CONFIGURALE_JOINT_COMPONENT_H_
#define OCTOON_CONFIGURALE_JOINT_COMPONENT_H_

#include <functional>
#include <memory>
#include <octoon/game_component.h>
#include <octoon/joint_component.h>
#include <octoon/math/math.h>

#include <octoon/rigidbody_component.h>
#include <octoon/physics/physics_joint.h>
#include <octoon/physics/physics_configurable_joint.h>


namespace octoon
{
	using physics::ConfigurableJointMotion;
	class OCTOON_EXPORT ConfigurableJointComponent : public JointComponent
	{
		OctoonDeclareSubInterface(ConfigurableJointComponent, GameComponent)
	public:
		ConfigurableJointComponent() noexcept;
		virtual ~ConfigurableJointComponent() noexcept;

		void ConfigurableJointComponent::onActivate() except;
		void ConfigurableJointComponent::onDeactivate() noexcept;

		void ConfigurableJointComponent::onAttachComponent(const GameComponentPtr& component) noexcept;
		void ConfigurableJointComponent::onDetachComponent(const GameComponentPtr& component) noexcept;

		virtual std::shared_ptr<RigidbodyComponent> getConnectedBody();
		virtual void setConnectedBody(std::shared_ptr<RigidbodyComponent> component);

		virtual void setAngularXMotion(ConfigurableJointMotion motion);
		virtual ConfigurableJointMotion getAngularXMotion();

		virtual void setAngularYMotion(ConfigurableJointMotion motion);
		virtual ConfigurableJointMotion getAngularYMotion();

		virtual void setAngularZMotion(ConfigurableJointMotion motion);
		virtual ConfigurableJointMotion getAngularZMotion();

		virtual void setXMotion(ConfigurableJointMotion motion);
		virtual ConfigurableJointMotion getXMotion();

		virtual void setYMotion(ConfigurableJointMotion motion);
		virtual ConfigurableJointMotion getYMotion();

		virtual void setZMotion(ConfigurableJointMotion motion);
		virtual ConfigurableJointMotion getZMotion();

	protected:
		void buildJoint();
	private:
		std::shared_ptr<physics::PhysicsConfigurableJoint> joint;
		std::weak_ptr<RigidbodyComponent> thisBody;
		std::weak_ptr<RigidbodyComponent> connectedBody;
	};
}

#endif // OCTOON_CONFIGURALE_JOINT_COMPONENT_H_