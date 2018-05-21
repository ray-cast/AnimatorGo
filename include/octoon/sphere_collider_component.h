#ifndef OCTOON_SPHERE_COLLIDER_COMPONENT_H_
#define OCTOON_SPHERE_COLLIDER_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/collider_component.h>
#include <octoon/math/math.h>


namespace octoon
{
    class OCTOON_EXPORT SphereCollider : public Collider
	{
        OctoonDeclareSubClass(SphereCollider, Collider)
        public:
			SphereCollider() noexcept;
			SphereCollider(float r) noexcept;
            ~SphereCollider();
            virtual GameComponentPtr clone() const noexcept;

            void setRadius(float r) noexcept;
            float getRadius() const noexcept;

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
            bool isRegistered;
            float radius;
    };
}


#endif // OCTOON_SPHERE_COLLIDER_COMPONENT_H_