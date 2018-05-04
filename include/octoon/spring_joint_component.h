#ifndef OCTOON_SPRING_JOINT_COMPONENT_H_
#define OCTOON_SPRING_JOINT_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/joint_component.h>
#include <octoon/math/math.h>

namespace physx
{
	class PxRevoluteJoint;
}


namespace octoon
{
    class OCTOON_EXPORT SpringJoint : public Joint
	{
        OctoonDeclareSubClass(SpringJoint, Joint)
        public:
			SpringJoint() noexcept;
			SpringJoint(const std::shared_ptr<Rigidbody>& connected_body) noexcept;
            ~SpringJoint();
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
			float damper; // The damper force used to dampen the spring force.;
			float maxDistance; // The maximum distance between the bodies relative to their initial distance.
			float minDistance; // The minimum distance between the bodies relative to their initial distance.
			float spring; // The spring force used to keep the two objects together.
			float tolerance; // The maximum allowed error between the current spring length and the length defined by minDistance and maxDistance.

            friend class Rigidbody;
    };
}


#endif // OCTOON_SPRING_JOINT_COMPONENT_H_