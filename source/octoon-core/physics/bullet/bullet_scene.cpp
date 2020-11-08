#include "bullet_scene.h"
#include "bullet_rigidbody.h"

#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
	namespace physics
	{
		BulletScene::BulletScene(PhysicsSceneDesc desc)
		{
			collisionConfiguration_ = std::make_unique<btDefaultCollisionConfiguration>();
			dispatcher_ = std::make_unique<btCollisionDispatcher>(collisionConfiguration_.get());

			broadphase_ = std::make_unique<btDbvtBroadphase>();
			solver_ = std::make_unique<btSequentialImpulseConstraintSolver>();

			dynamicsWorld_ = std::make_unique<btDiscreteDynamicsWorld>(dispatcher_.get(), broadphase_.get(), solver_.get(), collisionConfiguration_.get());
		}

		BulletScene::~BulletScene()
		{
			dynamicsWorld_.reset();
			dispatcher_.reset();
		}

		void
		BulletScene::setGravity(const math::float3& gravity) noexcept
		{
			dynamicsWorld_->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
		}

		math::float3
		BulletScene::getGravity() const noexcept
		{
			auto gravity = dynamicsWorld_->getGravity();
			return math::float3(gravity.getX(), gravity.getY(), gravity.getZ());
		}

		void
		BulletScene::addRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody)
		{
			auto bulletRigidbody = std::dynamic_pointer_cast<BulletRigidbody>(rigidbody);
			this->dynamicsWorld_->addRigidBody(bulletRigidbody->getPxRigidbody());
		}

		void
		BulletScene::removeRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody)
		{
			auto bulletRigidbody = std::dynamic_pointer_cast<BulletRigidbody>(rigidbody);
			this->dynamicsWorld_->removeRigidBody(bulletRigidbody->getPxRigidbody());
		}

		void
		BulletScene::simulate(float time)
		{
			this->dynamicsWorld_->stepSimulation(time);
		}

		void
		BulletScene::fetchResults()
		{
		}

		void
		BulletScene::fetchFinish()
		{
			for (int i = 0; i < this->dynamicsWorld_->getNumCollisionObjects(); ++i)
			{
				btCollisionObject* obj = this->dynamicsWorld_->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);

				PhysicsListener* listener = static_cast<PhysicsListener*>(body->getUserPointer());
				if (listener)
					listener->onFetchResult();
			}
		}
	}
}