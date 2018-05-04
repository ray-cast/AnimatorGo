#ifndef OCTOON_FIXED_JOINT_COMPONENT_H_
#define OCTOON_FIXED_JOINT_COMPONENT_H_

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
    class OCTOON_EXPORT FixedJoint : public Joint
	{
        OctoonDeclareSubClass(FixedJoint, Joint)
        public:
            FixedJoint() noexcept;
			FixedJoint(const std::shared_ptr<Rigidbody>& connected_body) noexcept;
            ~FixedJoint();
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
            friend class Rigidbody;
    };
}


#endif // OCTOON_FIXED_JOINT_COMPONENT_H_