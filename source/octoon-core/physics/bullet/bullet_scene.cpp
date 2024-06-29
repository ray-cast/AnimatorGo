#include "bullet_scene.h"
#include "bullet_rigidbody.h"
#include "bullet_joint.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>

namespace octoon
{
	struct FilterCallback : public btOverlapFilterCallback
	{
		virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
		{
			bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
			collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);
			return collides;
		}
	};

	BulletScene::BulletScene(PhysicsSceneDesc desc)
		: broadphase_(std::make_unique<btDbvtBroadphase>())
		, collisionConfiguration_(std::make_unique<btDefaultCollisionConfiguration>())
		, filterCallback_(std::make_unique<FilterCallback>())
		, solver_(std::make_unique<btSequentialImpulseConstraintSolver>())
		, maxSubSteps_(1)
		, fixedTimeStep_(1.0f / 60.f)
		, groundEnabled_(true)
		, groundBoxShape_(nullptr)
		, groundCollisionObject_(nullptr)
	{
		dispatcher_ = std::make_unique<btCollisionDispatcher>(collisionConfiguration_.get());

		auto groundBoxShape_ = new btBoxShape(btVector3(50, 50, 50));
		groundBoxShape_->setMargin(0.5f);

		btTransform transform;
		transform.getBasis().setIdentity();
		transform.setOrigin(btVector3(0, -groundBoxShape_->getHalfExtentsWithoutMargin().y(), 0));

		auto groundCollisionObject_ = new btCollisionObject();
		groundCollisionObject_->setWorldTransform(transform);
		groundCollisionObject_->setInterpolationWorldTransform(transform);
		groundCollisionObject_->setCollisionShape(groundBoxShape_);
		groundCollisionObject_->setUserIndex(15);
		groundCollisionObject_->setUserIndex2(0xffff);
		groundCollisionObject_->setUserIndex3(0);
		groundCollisionObject_->setCollisionFlags(groundCollisionObject_->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		groundCollisionObject_->setActivationState(DISABLE_DEACTIVATION);

		dynamicsWorld_ = std::make_unique<btDiscreteDynamicsWorld>(dispatcher_.get(), broadphase_.get(), solver_.get(), collisionConfiguration_.get());
		dynamicsWorld_->setGravity(btVector3(desc.gravity.x, desc.gravity.y, desc.gravity.z));
		dynamicsWorld_->getPairCache()->setOverlapFilterCallback(filterCallback_.get());
		dynamicsWorld_->addCollisionObject(groundCollisionObject_, 1 << groundCollisionObject_->getUserIndex(), groundCollisionObject_->getUserIndex2());
	}

	BulletScene::~BulletScene()
	{
		for (int i = 0; i < dynamicsWorld_->getNumConstraints(); i++)
			dynamicsWorld_->removeConstraint(dynamicsWorld_->getConstraint(i));

		for (int i = 0; i < dynamicsWorld_->getNumCollisionObjects(); i++)
			dynamicsWorld_->removeCollisionObject(dynamicsWorld_->getCollisionObjectArray()[i]);

		delete groundCollisionObject_;
		delete groundBoxShape_;

		dynamicsWorld_.reset();
		dispatcher_.reset();
	}

	void
	BulletScene::setGroundEnable(bool value) noexcept
	{
		if (groundEnabled_ != value)
		{
			if (value)
				dynamicsWorld_->addCollisionObject(groundCollisionObject_, 1 << groundCollisionObject_->getUserIndex(), groundCollisionObject_->getUserIndex2());
			else
				dynamicsWorld_->removeCollisionObject(groundCollisionObject_);

			groundEnabled_ = true;
		}
	}

	bool
	BulletScene::getGroundEnable() const noexcept
	{
		return groundEnabled_;
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
		this->dynamicsWorld_->addRigidBody(bulletRigidbody, 1 << bulletRigidbody->getUserIndex(), bulletRigidbody->getUserIndex2());
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
	BulletScene::setMaxSubStepCount(int numSubSteps) noexcept
	{
		maxSubSteps_ = numSubSteps;
	}

	int
	BulletScene::getMaxSubStepCount() noexcept
	{
		return maxSubSteps_;
	}

	void
	BulletScene::setFixedTimeStep(float fixedTimeStep) noexcept
	{
		fixedTimeStep_ = fixedTimeStep;
	}

	float
	BulletScene::getFixedTimeStep() noexcept
	{
		return fixedTimeStep_;
	}

	void
	BulletScene::simulate(float time)
	{
		auto collision = this->dynamicsWorld_->getCollisionObjectArray();
		auto collisionNums = this->dynamicsWorld_->getNumCollisionObjects();

		for (int i = 0; i < collisionNums; ++i)
		{
			auto& collider = collision[i];
			if (collider->getUserIndex3() > 0)
			{
				btRigidBody* body = btRigidBody::upcast(collision[i]);

				this->dynamicsWorld_->removeRigidBody(body);
				this->dynamicsWorld_->addRigidBody(body, 1 << body->getUserIndex(), body->getUserIndex2());

				body->setUserIndex3(false);
			}
		}

		this->dynamicsWorld_->stepSimulation(time, maxSubSteps_, fixedTimeStep_);
	}

	void
	BulletScene::reset()
	{
		btDispatcher* dispatcher = dynamicsWorld_->getDispatcher();

		dynamicsWorld_->getBroadphase()->resetPool(dispatcher);
		dynamicsWorld_->getConstraintSolver()->reset();
		dynamicsWorld_->clearForces();
	}

	void
	BulletScene::fetchResults()
	{
		auto rigidbodies = this->dynamicsWorld_->getCollisionObjectArray();
		auto rigidbodiesNums = rigidbodies.size();

		for (int i = 0; i < rigidbodiesNums; ++i)
		{
			if (rigidbodies[i]->isActive())
			{
				PhysicsListener* listener = static_cast<PhysicsListener*>(rigidbodies[i]->getUserPointer());
				if (listener)
					listener->onFetchResult();
			}
		}
	}
}