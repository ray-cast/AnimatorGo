#ifndef OCTOON_JOINT_COMPONENT_H_
#define OCTOON_JOINT_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/math/math.h>


namespace physx
{
	class PxJoint;
}


namespace octoon
{
    class OCTOON_EXPORT Joint : public GameComponent
	{
        OctoonDeclareSubInterface(Joint, runtime::RttiInterface)
    public:
        Joint() noexcept;
		Joint(const std::shared_ptr<Rigidbody>& connected_body) noexcept;
        virtual ~Joint();
        virtual GameComponentPtr clone() const noexcept = 0;
        
    protected:
        virtual void onJointChange() = 0;
        virtual void onJointEnter() = 0;
        virtual void onJointExit() = 0;

	protected:
		physx::PxJoint * joint;

		math::float3 anchor; // The Position of the anchor around which the joints motion is constrained.
		math::float3 connectedAnchor; // Position of the anchor relative to the connected Rigidbody.
		std::shared_ptr<Rigidbody> connectedBody;	// A reference to another rigidbody this joint connects to.

        friend class Rigidbody;
    };
}


#endif // OCTOON_JOINT_COMPONENT_H_