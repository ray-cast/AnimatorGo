#ifndef OCTOON_HINGE_JOINT_COMPONENT_H_
#define OCTOON_HINGE_JOINT_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/joint_component.h>
#include <octoon/joint_limits.h>
#include <octoon/joint_spring.h>
#include <octoon/math/math.h>

namespace physx
{
	class PxRevoluteJoint;
}


namespace octoon
{
    class OCTOON_EXPORT HingeJoint : public Joint
	{
        OctoonDeclareSubClass(HingeJoint, Joint)
        public:
			HingeJoint() noexcept;
			HingeJoint(const std::shared_ptr<Rigidbody>& connected_body) noexcept;
            ~HingeJoint();
            virtual GameComponentPtr clone() const noexcept;
        
        protected:
			virtual void onAttachComponent(const GameComponentPtr& component) noexcept;
			virtual void onDetachComponent(const GameComponentPtr& component) noexcept;

            virtual void onJointChange() override;
            virtual void onJointEnter() override;
            virtual void onJointExit() override;

			void buildJoint(const GameComponentPtr& component) except;
			void releaseJoint(const GameComponentPtr& component) except;
        private:
			JointLimits limits; // Limit of angular rotation(in degrees) on the hinge joint.
			//float motor; // The motor will apply a force up to a maximum force to achieve the target velocity in degrees per second.
			JointSpring spring; // The spring attempts to reach a target angle by adding spring and damping forces.
			bool useLimits; // Enables the joint's limits. Disabled by default.
			bool useMotor; // Enables the joint's motor. Disabled by default.
			bool useSpring;	// Enables the joint's spring. Disabled by default.

            friend class Rigidbody;
    };
}


#endif // OCTOON_HINGE_JOINT_COMPONENT_H_