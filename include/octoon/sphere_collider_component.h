#ifndef OCTOON_SPHERE_COLLIDER_COMPONENT_H_
#define OCTOON_SPHERE_COLLIDER_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/collider_component.h>
#include <octoon/math/math.h>

class b2Body;

namespace octoon
{
    class OCTOON_EXPORT CircleCollider : public Collider
	{
        OctoonDeclareSubClass(CircleCollider, Collider)
        public:
            CircleCollider() noexcept;
            ~CircleCollider();
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

        private:
            bool isRegistered;
            float radius;
    };
}


#endif // OCTOON_SPHERE_COLLIDER_COMPONENT_H_