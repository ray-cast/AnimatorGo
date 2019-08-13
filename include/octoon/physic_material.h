#ifndef OCTOON_PHYSIC_MATERIAL_H_
#define OCTOON_PHYSIC_MATERIAL_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT PhysicMaterial final
	{
	public:
		PhysicMaterial() noexcept;
		~PhysicMaterial() noexcept;

	private:
		PhysicMaterial(const PhysicMaterial&) = delete;
		PhysicMaterial& operator=(const PhysicMaterial&) = delete;
	};
}

#endif