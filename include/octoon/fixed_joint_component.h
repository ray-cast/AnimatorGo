#ifndef OCTOON_FIXED_JOINT_COMPONENT_H_
#define OCTOON_FIXED_JOINT_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/joint_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/physics/physics_fixed_joint.h>

namespace octoon
{
	class OCTOON_EXPORT FixedJointComponent final: public JointComponent
	{
		OctoonDeclareSubInterface(FixedJointComponent, GameComponent)
	public:
		FixedJointComponent() noexcept;
		virtual ~FixedJointComponent() noexcept;

		virtual std::shared_ptr<RigidbodyComponent> getTarget() override;
		virtual void setTarget(std::shared_ptr<RigidbodyComponent> component) override;

	private:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onAttachComponent(const GameComponentPtr& component) noexcept override;
		void onDetachComponent(const GameComponentPtr& component) noexcept override;

	private:
		void setupFixedJoint();

	private:
		std::shared_ptr<PhysicsFixedJoint> joint;
		std::weak_ptr<RigidbodyComponent> thisBody;
		std::weak_ptr<RigidbodyComponent> target_;
	};
}

#endif