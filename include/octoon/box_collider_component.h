#ifndef OCTOON_BOX_COLLIDER_COMPONENT_H_
#define OCTOON_BOX_COLLIDER_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/collider_component.h>
#include <octoon/math/math.h>


namespace octoon
{
    class OCTOON_EXPORT BoxCollider : public Collider
	{
        OctoonDeclareSubClass(BoxCollider, Collider)
        public:
            BoxCollider() noexcept;
			BoxCollider(const math::Vector3& s) noexcept;
            ~BoxCollider();
            virtual GameComponentPtr clone() const noexcept;

            void setAutoTiling(bool isAutoTilling) noexcept;
            bool getAutoTiling() const noexcept;

            void setEdgeRadius(float r) noexcept;
            float getEdgeRadius() const noexcept;

            void setSize(const math::Vector3& s) noexcept;
            math::Vector3 getSize() const noexcept;

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
			void buildCollider(const GameComponentPtr& component) except;
        private:
            bool isRegistered;
            bool autoTiling; //Determines whether the BoxCollider2D's shape is automatically updated based on a SpriteRenderer's tiling properties.
            float edgeRadius; //Controls the radius of all edges created by the collider.
            math::Vector3 size; //The width and height of the rectangle.
    };
}


#endif // OCTOON_BOX_COLLIDER_COMPONENT_H_