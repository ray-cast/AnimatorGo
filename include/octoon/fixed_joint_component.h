#ifndef OCTOON_FIXED_JOINT_COMPONENT_H_
#define OCTOON_FIXED_JOINT_COMPONENT_H_

#include <functional>
#include <memory>
#include <octoon/game_component.h>
#include <octoon/joint_component.h>
#include <octoon/math/math.h>

#include <octoon/rigidbody_component.h>
#include <octoon/physics/physics_fixed_joint.h>


namespace octoon
{
	class OCTOON_EXPORT FixedJointComponent : public JointComponent
	{
		OctoonDeclareSubInterface(FixedJointComponent, GameComponent)
	public:
		FixedJointComponent() noexcept;
		virtual ~FixedJointComponent() noexcept;

		void FixedJointComponent::onActivate() except;
		void FixedJointComponent::onDeactivate() noexcept;

		void FixedJointComponent::onAttachComponent(const GameComponentPtr& component) noexcept;
		void FixedJointComponent::onDetachComponent(const GameComponentPtr& component) noexcept;

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

#endif // OCTOON_FIXED_JOINT_COMPONENT_H_