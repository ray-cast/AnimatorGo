#include "bullet_scene.h"
#include "bullet_rigidbody.h"

#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
	BulletScene::BulletScene(PhysicsSceneDesc desc)
	{
		broadphase_ = std::make_unique<btDbvtBroadphase>();

		collisionConfiguration_ = std::make_unique<btDefaultCollisionConfiguration>();
		dispatcher_ = std::make_unique<btCollisionDispatcher>(collisionConfiguration_.get());

		solver_ = std::make_unique<btSequentialImpulseConstraintSolver>();

		dynamicsWorld_ = std::make_unique<btDiscreteDynamicsWorld>(dispatcher_.get(), broadphase_.get(), solver_.get(), collisionConfiguration_.get());
		dynamicsWorld_->setGravity(btVector3(desc.gravity.x, desc.gravity.y, desc.gravity.z));
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
		this->dynamicsWorld_->addRigidBody(bulletRigidbody->getRigidbody());
	}

	void
	BulletScene::removeRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody)
	{
		auto bulletRigidbody = std::dynamic_pointer_cast<BulletRigidbody>(rigidbody);
		this->dynamicsWorld_->removeRigidBody(bulletRigidbody->getRigidbody());
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
		auto& rigidbodies = this->dynamicsWorld_->getNonStaticRigidBodies();

		for (int i = 0; i < rigidbodies.size(); ++i)
		{
			PhysicsListener* listener = static_cast<PhysicsListener*>(rigidbodies[i]->getUserPointer());
			if (listener)
				listener->onFetchResult();
		}
	}
}