#ifndef OCTOON_JOINT_COMPONENT_H_
#define OCTOON_JOINT_COMPONENT_H_

#include <functional>
#include <memory>
#include <octoon/game_component.h>
#include <octoon/math/math.h>
#include <octoon/rigidbody_component.h>

namespace octoon
{
	class OCTOON_EXPORT JointComponent : public GameComponent
	{
		OctoonDeclareSubInterface(JointComponent, GameComponent)
	public:
		JointComponent() noexcept;
		virtual ~JointComponent() noexcept;

		virtual std::shared_ptr<RigidbodyComponent> getTarget() = 0;
		virtual void setTarget(std::shared_ptr<RigidbodyComponent>) = 0;
	};
}

#endif