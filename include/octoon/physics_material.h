#ifndef OCTOON_PHYSICS_MATERIAL_COMPONENT_H_
#define OCTOON_PHYSICS_MATERIAL_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/physics_feature.h>
#include <octoon/math/math.h>

namespace physx
{
	class PxMaterial;
}

namespace octoon
{
    class OCTOON_EXPORT PhysicsMaterial : public runtime::RttiInterface
	{
        OctoonDeclareSubClass(PhysicsMaterial, runtime::RttiInterface)
        public:
            PhysicsMaterial() noexcept;
            ~PhysicsMaterial();

            void setBounciness(float b) noexcept;
            float getBounciness() const noexcept;

            void setFriction(float f) noexcept;
            float getFriction() const noexcept;
        
			physx::PxMaterial* getMaterial() noexcept { return material; }
        protected:
            float bounciness; // The degree of elasticity during collisions.
            float friction; // Coefficient of friction.
        private:
			physx::PxMaterial* material;

            friend class Rigidbody;
    };
}


#endif // OCTOON_PHYSICS_MATERIAL_COMPONENT_H_