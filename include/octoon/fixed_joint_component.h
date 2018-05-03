#ifndef OCTOON_FIXED_JOINT_COMPONENT_H_
#define OCTOON_FIXED_JOINT_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/joint_component.h>
#include <octoon/math/math.h>


namespace octoon
{
    class OCTOON_EXPORT FixedJoint : public Joint
	{
        OctoonDeclareSubClass(FixedJoint, Joint)
        public:
            FixedJoint() noexcept;
            ~FixedJoint();
            virtual GameComponentPtr clone() const noexcept;
        
        protected:
            virtual void onJointChange() override;
            virtual void onJointEnter() override;
            virtual void onJointExit() override;
        private:

            friend class Rigidbody;
    };
}


#endif // OCTOON_FIXED_JOINT_COMPONENT_H_