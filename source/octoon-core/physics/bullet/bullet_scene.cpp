#include "bullet_scene.h"
#include "bullet_rigidbody.h"
#include "bullet_joint.h"

#include <bullet/btBulletDynamicsCommon.h>

namespace octoon
{
	struct FilterCallback : public btOverlapFilterCallback
	{
		virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
		{
			if ((1 << proxy0->m_collisionFilterGroup) & ~proxy1->m_collisionFilterMask || 
				(1 << proxy1->m_collisionFilterGroup) & ~proxy0->m_collisionFilterMask)
				return false;

			return true;
		}
	};

	BulletScene::BulletScene(PhysicsSceneDesc desc)
	{
		broadphase_ = std::make_unique<btDbvtBroadphase>();

		collisionConfiguration_ = std::make_unique<btDefaultCollisionConfiguration>();
		dispatcher_ = std::make_unique<btCollisionDispatcher>(collisionConfiguration_.get());

		btOverlapFilterCallback* filterCallback = new FilterCallback();

		solver_ = std::make_unique<btSequentialImpulseConstraintSolver>();

		dynamicsWorld_ = std::make_unique<btDiscreteDynamicsWorld>(dispatcher_.get(), broadphase_.get(), solver_.get(), collisionConfiguration_.get());
		dynamicsWorld_->setGravity(btVector3(desc.gravity.x, desc.gravity.y, desc.gravity.z));
		dynamicsWorld_->getPairCache()->setOverlapFilterCallback(filterCallback);
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
		auto bulletRigidbody = std::dynamic_pointer_cast<BulletRigidbody>(rigidbody)->getRigidbody();
		this->dynamicsWorld_->addRigidBody(bulletRigidbody, bulletRigidbody->getUserIndex(), bulletRigidbody->getUserIndex2());
	}

	void
	BulletScene::removeRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody)
	{
		auto bulletRigidbody = std::dynamic_pointer_cast<BulletRigidbody>(rigidbody);
		this->dynamicsWorld_->removeRigidBody(bulletRigidbody->getRigidbody());
	}

	void
	BulletScene::addConstraint(std::shared_ptr<PhysicsJoint> joint)
	{
		auto constraint = std::dynamic_pointer_cast<BulletJoint>(joint);
		this->dynamicsWorld_->addConstraint(constraint->getConstraint(), true);
	}

	void
	BulletScene::removeConstraint(std::shared_ptr<PhysicsJoint> joint)
	{
		auto constraint = std::dynamic_pointer_cast<BulletJoint>(joint);
		this->dynamicsWorld_->removeConstraint(constraint->getConstraint());
	}

	void
	BulletScene::simulate(float time)
	{
		this->dynamicsWorld_->stepSimulation(time);
	}

	void
	BulletScene::fetchResults()
	{
		this->dynamicsWorld_->synchronizeMotionStates();
	}

	void
	BulletScene::fetchFinish()
	{
		auto collision = this->dynamicsWorld_->getCollisionObjectArray();
		auto collisionNums = this->dynamicsWorld_->getNumCollisionObjects();

		for (int i = 0; i < collisionNums; ++i)
		{
			btCollisionObject* obj = this->dynamicsWorld_->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);

			if (body->getUserIndex3())
			{
				this->dynamicsWorld_->removeRigidBody(body);
				this->dynamicsWorld_->addRigidBody(body, body->getUserIndex(), body->getUserIndex2());

				body->setUserIndex3(false);
			}
		}

		auto& rigidbodies = this->dynamicsWorld_->getNonStaticRigidBodies();

		for (int i = 0; i < rigidbodies.size(); ++i)
		{
			PhysicsListener* listener = static_cast<PhysicsListener*>(rigidbodies[i]->getUserPointer());
			if (listener)
				listener->onFetchResult();
		}
	}
}