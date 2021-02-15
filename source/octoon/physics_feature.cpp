#include <octoon/physics_feature.h>
#include <octoon/physics/physics_system.h>

namespace octoon
{
	OctoonImplementSubClass(PhysicsFeature, GameFeature, "PhysicsFeature")

	PhysicsFeature::PhysicsFeature() except
		: physicsContext(nullptr)
		, physicsScene(nullptr)
		, gravity_(0.0f, -9.8f, 0.0f)
		, enableSimulate_(true)
		, forceSimulate_(false)
		, maxSubSteps_(10)
	{
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
	}

	void
	PhysicsFeature::setEnableSimulate(bool simulate) noexcept
	{
		this->enableSimulate_ = simulate;
	}

	bool
	PhysicsFeature::getEnableSimulate() const noexcept
	{
		return this->enableSimulate_;
	}

	void
	PhysicsFeature::setGravity(const math::float3& gravity) noexcept
	{
		if (physicsScene)
			physicsScene->setGravity(gravity);
		gravity_ = gravity;
	}

	const math::float3&
	PhysicsFeature::getGravity() const noexcept
	{
		return gravity_;
	}

	void
	PhysicsFeature::simulate() noexcept
	{
		this->forceSimulate_ = true;
	}

	void
	PhysicsFeature::onActivate() except
	{
		this->addMessageListener("feature:timer:fixed", std::bind(&PhysicsFeature::onFixedUpdate, this, std::placeholders::_1));

		PhysicsSceneDesc physicsSceneDesc;
		physicsSceneDesc.gravity = gravity_;

		physicsContext = PhysicsSystem::instance()->createContext(PhysicsDevice::Bullet);
		physicsScene = physicsContext->createScene(physicsSceneDesc);
		physicsScene->setMaxSubStepCount(maxSubSteps_);
	}

	void
	PhysicsFeature::onDeactivate() noexcept
	{
		this->removeMessageListener("feature:timer:fixed", std::bind(&PhysicsFeature::onFixedUpdate, this, std::placeholders::_1));

		physicsScene.reset();
		physicsContext.reset();
	}

	void
	PhysicsFeature::onReset() noexcept
	{
	}

	void
	PhysicsFeature::onFrameBegin() noexcept
	{
	}
	
	void
	PhysicsFeature::onFrame() except
	{
	}

	void
	PhysicsFeature::onFrameEnd() noexcept
	{
	}

	void
	PhysicsFeature::onFixedUpdate(const std::any& data) noexcept
	{
		if (data.type() == typeid(float))
		{
			auto timeInterval = std::any_cast<float>(data);
			if (timeInterval > 0.0f)
			{
				if (this->getEnableSimulate() || forceSimulate_)
				{
					physicsScene->simulate(timeInterval);
					physicsScene->fetchResults();
					physicsScene->fetchFinish();
					forceSimulate_ = false;
				}				
			}
		}
	}

	std::shared_ptr<PhysicsContext>
	PhysicsFeature::getContext()
	{
		return physicsContext;
	}

	std::shared_ptr<PhysicsScene> 
	PhysicsFeature::getScene()
	{
		return physicsScene;
	}
}