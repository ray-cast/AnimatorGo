#ifndef OCTOON_PHYSX_SCENE_H_
#define OCTOON_PHYSX_SCENE_H_

#include <memory>

#include <octoon/runtime/platform.h>

#include <octoon/physics/physics_scene.h>

#include "physx_type.h"

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysxScene : public PhysicsScene
		{
		public:
			PhysxScene(PhysxContext* context, PhysicsSceneDesc desc);
			virtual ~PhysxScene();

			virtual void addRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody) override;

		private:
			PhysxScene(const PhysxScene&) noexcept = delete;
			PhysxScene& operator=(const PhysxScene&) noexcept = delete;
        private:
			PhysxContext* context;
			physx::PxScene* px_scene;
		};
	}
}

#endif