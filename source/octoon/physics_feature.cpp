#include <octoon/physics_feature.h>
#include <octoon/physics/physics_system.h>

namespace octoon
{
    OctoonImplementSubClass(PhysicsFeature, GameFeature, "PhysicsFeature")

	PhysicsFeature::PhysicsFeature() except
		: physics_context(nullptr)
		, physics_scene(nullptr)
		, needUpdate_(false)
		, iterationCounts_(1)
		, timeInterval_(0.02f)
		, gravity_(0.0f, -9.8f, 0.0f)
	{
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
	}

	void
	PhysicsFeature::setGravity(const math::float3& gravity) noexcept
	{
		if (physics_scene)
			physics_scene->setGravity(gravity);
		gravity_ = gravity;
	}

	const math::float3&
	PhysicsFeature::getGravity() const noexcept
	{
		return gravity_;
	}

	void
	PhysicsFeature::setSolverIterationCounts(std::uint32_t iterationCounts) noexcept
	{
		iterationCounts_ = iterationCounts;
	}

	std::uint32_t
	PhysicsFeature::getSolverIterationCounts() const noexcept
	{
		return iterationCounts_;
	}

    void
	PhysicsFeature::onActivate() except
    {
		this->addMessageListener("feature:timer:fixed", std::bind(&PhysicsFeature::onFixedUpdate, this, std::placeholders::_1));

		physics::PhysicsSceneDesc physicsSceneDesc;
		physicsSceneDesc.gravity = gravity_;

		physics_context = physics::PhysicsSystem::instance()->createContext();
		physics_scene = physics_context->createScene(physicsSceneDesc);
    }

    void
	PhysicsFeature::onDeactivate() noexcept
    {
		this->removeMessageListener("feature:timer:fixed", std::bind(&PhysicsFeature::onFixedUpdate, this, std::placeholders::_1));

		physics_scene.reset();
		physics_context.reset();
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
		if (needUpdate_)
		{
			for (std::uint8_t i = 0; i < iterationCounts_; i++)
			{
				physics_scene->simulate(timeInterval_ / iterationCounts_);
				physics_scene->fetchStart();
			}

			needUpdate_ = false;
		}

		if (iterationCounts_ > 0)
			physics_scene->fetchResult();
    }

    void
	PhysicsFeature::onFrameEnd() noexcept
    {
    }

	void
	PhysicsFeature::onFixedUpdate(const runtime::any& data) noexcept
	{
		if (data.type() == typeid(float))
		{
			timeInterval_ = runtime::any_cast<float>(data);
			if (timeInterval_ > 0.0f)
				needUpdate_ = true;
		}
	}

	std::shared_ptr<physics::PhysicsContext>
	PhysicsFeature::getContext()
	{
		return physics_context;
	}

	std::shared_ptr<physics::PhysicsScene> 
	PhysicsFeature::getScene()
	{
		return physics_scene;
	}
}