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
			MeshCollider(model::MeshPtr m) noexcept;
            ~MeshCollider();
            virtual GameComponentPtr clone() const noexcept;

            void setMesh(model::MeshPtr m) noexcept;
			model::MeshPtr getMesh() const noexcept;

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
            model::MeshPtr sharedMesh; // The mesh object used for collision detection.
    };
}


#endif // OCTOON_MESH_COLLIDER_COMPONENT_H_