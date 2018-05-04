#ifndef OCTOON_COLLIDER_COMPONENT_H_
#define OCTOON_COLLIDER_COMPONENT_H_

#include <functional>
#include <memory>
#include <octoon/game_component.h>
#include <octoon/math/math.h>
#include <octoon/physics_material.h>

namespace physx
{
	class PxShape;
}


namespace octoon
{
    class OCTOON_EXPORT Collider : public GameComponent
	{
        OctoonDeclareSubInterface(Collider, GameComponent)
        public:
            Collider() noexcept;
            ~Collider();

            void setBounciness(float b) noexcept;
            float getBounciness() const noexcept;

            void setDensity(float d) noexcept;
            float getDensity() const noexcept;

            void setFriction(float f) noexcept;
            float getFriction() const noexcept;

			void setSharedMaterial(PhysicsMaterial material) except;
			std::shared_ptr<PhysicsMaterial> getSharedMaterial() except;

			physx::PxShape* getShape() noexcept { return shape; }

        protected:
            virtual void onCollisionChange() = 0;
            virtual void onCollisionEnter() = 0;
            virtual void onCollisionExit() = 0;
            virtual void onCollisionStay() = 0;
        
        protected:
			physx::PxShape* shape;

            float bounciness; // Get the bounciness used by the collider.
            float density; // The density of the collider used to calculate its mass (when auto mass is enabled).
            float friction; // Get the friction used by the collider.
            std::shared_ptr<PhysicsMaterial> shared_material; // The PhysicsMaterial2D that is applied to this collider.

            friend class Rigidbody;
    };
}


#endif // OCTOON_COLLIDER_COMPONENT_H_