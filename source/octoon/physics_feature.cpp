#include <octoon/physics_feature.h>
#include <octoon/timer_feature.h>

namespace octoon
{
    OctoonImplementSubClass(PhysicsFeature, GameFeature, "PhysicsFeature")

	PhysicsFeature::PhysicsFeature() except
		: physics_system()
		, physics_context(nullptr)
		, physics_scene(nullptr)
		, time_(0.0f)
		, timeStep_(1000.0f / 60.0f)
		, timeInterval_(1000.0f / 60.0f)
	{
		
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
	}

	void
	PhysicsFeature::setTimeStep(float timeStep) noexcept
	{
		timeStep_ = timeStep;
	}

	float
	PhysicsFeature::getTimeStep() const noexcept
	{
		return timeStep_;
	}

	void
	PhysicsFeature::setTimeInterval(float timeInterval) noexcept
	{
		timeInterval_ = timeInterval;
	}
	
	float
	PhysicsFeature::getTimeInterval() const noexcept
	{
		return timeInterval_;
	}

    void
	PhysicsFeature::onActivate() except
    {
		physics_context = physics_system.createContext();
		physics_scene = physics_context->createScene(physics::PhysicsSceneDesc());
    }

    void
	PhysicsFeature::onDeactivate() noexcept
    {
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
    }

    void
	PhysicsFeature::onFrameEnd() noexcept
    {
		auto feature = this->getFeature<TimerFeature>();
		if (feature)
		{
			time_ += feature->delta() * CLOCKS_PER_SEC;

			if (time_ > timeStep_)
			{
				physics_scene->simulate(timeInterval_ / CLOCKS_PER_SEC);
				time_ -= timeStep_;
			}
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