#ifndef OCTOON_CONFIGURALE_JOINT_COMPONENT_H_
#define OCTOON_CONFIGURALE_JOINT_COMPONENT_H_

#include <functional>
#include <memory>
#include <octoon/game_component.h>
#include <octoon/joint_component.h>
#include <octoon/math/math.h>

#include <octoon/rigidbody_component.h>
#include <octoon/physics/physics_fixed_joint.h>


namespace octoon
{
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

		virtual std::shared_ptr<RigidbodyComponent> getConnectedBody() override;
		virtual void setConnectedBody(std::shared_ptr<RigidbodyComponent> component) override;

	protected:
		void buildJoint();
	private:
		std::shared_ptr<physics::PhysicsFixedJoint> joint;
		std::weak_ptr<RigidbodyComponent> thisBody;
		std::weak_ptr<RigidbodyComponent> connectedBody;
	};
}

#endif // OCTOON_CONFIGURALE_JOINT_COMPONENT_H_