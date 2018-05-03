#ifndef OCTOON_JOINT_COMPONENT_H_
#define OCTOON_JOINT_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/math/math.h>



namespace octoon
{
    class OCTOON_EXPORT Joint : public GameComponent
	{
        OctoonDeclareSubInterface(Joint, runtime::RttiInterface)
        public:
            Joint() noexcept;
            ~Joint();
            virtual GameComponentPtr clone() const noexcept = 0;
        
        protected:
            virtual void onJointChange() = 0;
            virtual void onJointEnter() = 0;
            virtual void onJointExit() = 0;
        private:

            friend class Rigidbody;
    };
}


#endif // OCTOON_JOINT_COMPONENT_H_