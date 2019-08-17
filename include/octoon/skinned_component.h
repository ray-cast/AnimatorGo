#ifndef OCTOON_SKINNED_COMPONENT_H_
#define OCTOON_SKINNED_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/physics/physics_shape.h>

namespace octoon
{
	class OCTOON_EXPORT SkinnedComponent : public GameComponent
	{
		OctoonDeclareSubInterface(SkinnedComponent, GameComponent)
	public:
		SkinnedComponent() noexcept;
		virtual ~SkinnedComponent() noexcept;

		virtual void setControl(float control) noexcept = 0;
		virtual float getControl() const noexcept = 0;
	};
}

#endif