#ifndef OCTOON_CAPSULE_COLLIDER_COMPONENT_H_
#define OCTOON_CAPSULE_COLLIDER_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/collider_component.h>
#include <octoon/math/math.h>


namespace octoon
{
    class OCTOON_EXPORT CapsuleCollider : public Collider
	{
        OctoonDeclareSubClass(CapsuleCollider, Collider)
        public:
            CapsuleCollider() noexcept;
			CapsuleCollider(float r, float half_h) noexcept;
            ~CapsuleCollider();
            virtual GameComponentPtr clone() const noexcept;

			void setRadius(float r) except;
			float getRadius() const except;

			void setHalfHeight(float h) except;
			float getHalfHeight() const except;

        protected:
            virtual void onCollisionChange() noexcept;
            virtual void onCollisionEnter() noexcept;
            virtual void onCollisionExit() noexcept;
            virtual void onCollisionStay() noexcept;

        private:
            virtual void onAttach() except;
            virtual void onDetach() noexcept;

            virtual void onAttachComponent(const GameComponentPtr& component) except;
            virtual void onDetachComponent(const GameComponentPtr& component) noexcept;

			void buildCollider() except;
			void releaseCollider() except;
        private:
			float radius;
			float halfHeight;
    };
}


#endif // OCTOON_CAPSULE_COLLIDER_COMPONENT_H_