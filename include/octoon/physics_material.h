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
			PhysicsMaterial(const PhysicsMaterial& other) noexcept;
			PhysicsMaterial(PhysicsMaterial&& other) noexcept;
            ~PhysicsMaterial();

            void setStaticFriction(float f) noexcept;
            float getStaticFriction() const noexcept;

            void setDynamicFriction(float f) noexcept;
            float getDynamicFriction() const noexcept;

			void setRestitution(float r) noexcept;
			float getRestitution() const noexcept;
        
			physx::PxMaterial* getMaterial() noexcept { return material; }
        protected:
			float static_friction;
			float dynamic_friction;
			float restitution;
        private:
			physx::PxMaterial* material;

            friend class Rigidbody;
    };
}


#endif // OCTOON_PHYSICS_MATERIAL_COMPONENT_H_