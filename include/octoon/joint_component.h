#ifndef OCTOON_JOINT_COMPONENT_H_
#define OCTOON_JOINT_COMPONENT_H_

#include <functional>
#include <memory>
#include <octoon/game_component.h>
#include <octoon/math/math.h>


namespace octoon
{
	class OCTOON_EXPORT JointComponent : public GameComponent
	{
		OctoonDeclareSubInterface(JointComponent, GameComponent)
	public:
		JointComponent() noexcept;
		virtual ~JointComponent() noexcept;


	protected:

	protected:
	};
}

#endif // OCTOON_JOINT_COMPONENT_H_