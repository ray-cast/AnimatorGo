#ifndef OCTOON_BULLET_SCENE_H_
#define OCTOON_BULLET_SCENE_H_

#include <memory>
#include <vector>

#include <octoon/runtime/platform.h>
#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_listener.h>

#include "bullet_rigidbody.h"
#include "bullet_type.h"

namespace octoon
{
	class OCTOON_EXPORT BulletScene : public PhysicsScene
	{
	public:
		BulletScene(PhysicsSceneDesc desc);
		virtual ~BulletScene();

		virtual void setGravity(const math::float3& gravity) noexcept override;
		virtual math::float3 getGravity() const noexcept override;

		virtual void addRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody) override;
		virtual void removeRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody) override;

		virtual void addConstraint(std::shared_ptr<PhysicsJoint> joint) override;
		virtual void removeConstraint(std::shared_ptr<PhysicsJoint> joint) override;

		virtual void simulate(float time) override;
			
		virtual void fetchResults() override;
		virtual void fetchFinish() override;

	private:
		BulletScene(const BulletScene&) noexcept = delete;
		BulletScene& operator=(const BulletScene&) noexcept = delete;

    private:
		std::unique_ptr<btBroadphaseInterface> broadphase_;
		std::unique_ptr<btCollisionDispatcher> dispatcher_;
		std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration_;
		std::unique_ptr<btSequentialImpulseConstraintSolver> solver_;
		std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld_;
	};
}

#endif