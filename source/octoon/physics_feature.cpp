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
		, enableGround_(true)
		, maxSubSteps_(10)
		, fixedTimeStep_(1.0f / 50.0f)
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
	PhysicsFeature::setGroundEnable(bool value) noexcept
	{
		if (physicsScene)
			physicsScene->setGroundEnable(value);

		this->enableGround_;
	}

	bool
	PhysicsFeature::getGroundEnable() const noexcept
	{
		return this->enableGround_;
	}

	void
	PhysicsFeature::setFixedTimeStep(float fixedTimeStep) noexcept
	{
		if (physicsScene)
			physicsScene->setFixedTimeStep(fixedTimeStep);

		fixedTimeStep_ = fixedTimeStep;
	}

	float
	PhysicsFeature::getFixedTimeStep() const noexcept
	{
		return fixedTimeStep_;
	}

	void
	PhysicsFeature::setSolverIterationCounts(std::uint32_t maxSubSteps) noexcept
	{
		if (physicsScene)
			physicsScene->setMaxSubStepCount(maxSubSteps);
		maxSubSteps_ = maxSubSteps;
	}

	std::uint32_t
	PhysicsFeature::getSolverIterationCounts() const noexcept
	{
		return maxSubSteps_;
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
	PhysicsFeature::simulate(float delta) noexcept
	{
		if (physicsScene)
		{
			physicsScene->simulate(delta);
			physicsScene->fetchResults();
		}
	}

	void
	PhysicsFeature::reset() noexcept
	{
		if (physicsScene)
			physicsScene->reset();
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
		physicsScene->setFixedTimeStep(fixedTimeStep_);
		physicsScene->setGroundEnable(enableGround_);
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
			if (timeInterval > 0.0f && this->getEnableSimulate())
			{
				physicsScene->simulate(timeInterval);
				physicsScene->fetchResults();
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