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

        void setSize(const math::Vector3& s) noexcept;
        const math::Vector3& getSize() const noexcept;

		virtual GameComponentPtr clone() const noexcept;

    private:
        virtual void onAttach() except;
        virtual void onDetach() noexcept;

		void buildCollider() except;
		void releaseCollider() except;

    private:
        math::Vector3 size; //The width and height of the rectangle.
    };
}

#endif // OCTOON_BOX_COLLIDER_COMPONENT_H_