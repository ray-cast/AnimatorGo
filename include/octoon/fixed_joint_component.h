#ifndef OCTOON_FIXED_JOINT_COMPONENT_H_
#define OCTOON_FIXED_JOINT_COMPONENT_H_

#include <functional>
#include <memory>
#include <octoon/game_component.h>
#include <octoon/joint_component.h>
#include <octoon/math/math.h>


namespace octoon
{
	class OCTOON_EXPORT FixedJointComponent : public JointComponent
	{
		OctoonDeclareSubInterface(FixedJointComponent, GameComponent)
	public:
		FixedJointComponent() noexcept;
		virtual ~FixedJointComponent() noexcept;


	protected:

	protected:
	};
}

#endif // OCTOON_FIXED_JOINT_COMPONENT_H_