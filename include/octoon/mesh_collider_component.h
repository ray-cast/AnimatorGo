#ifndef OCTOON_MESH_COLLIDER_COMPONENT_H_
#define OCTOON_MESH_COLLIDER_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/collider_component.h>
#include <octoon/model/mesh.h>
#include <octoon/math/math.h>


namespace octoon
{
    class OCTOON_EXPORT MeshCollider : public Collider
	{
        OctoonDeclareSubClass(MeshCollider, Collider)
        public:
			MeshCollider() noexcept;
            ~MeshCollider();
            virtual GameComponentPtr clone() const noexcept;

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

			void buildCollider(const GameComponentPtr& component) except;
			void releaseCollider() except;
        private:
			bool isConvex;
    };
}


#endif // OCTOON_MESH_COLLIDER_COMPONENT_H_