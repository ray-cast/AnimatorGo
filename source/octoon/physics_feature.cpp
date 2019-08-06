#include <octoon/physics_feature.h>
#include <octoon/timer_feature.h>

namespace octoon
{
    OctoonImplementSubClass(PhysicsFeature, GameFeature, "PhysicsFeature")

	PhysicsFeature::PhysicsFeature() except
		: physics_system()
		, physics_context(nullptr)
		, physics_scene(nullptr)
		, needUpdate_(false)
	{
		
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
	}

    void
	PhysicsFeature::onActivate() except
    {
		this->addMessageListener("feature:timer:fixed", std::bind(&PhysicsFeature::onFixedUpdate, this, std::placeholders::_1));

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
		physics_scene->fetchResult();
    }

    void
	PhysicsFeature::onFrame() except
    {
		if (needUpdate_)
		{
			auto timeFeature = this->getFeature<TimerFeature>();
			if (timeFeature)
			{
				physics_scene->simulate(timeFeature->getTimeInterval() / CLOCKS_PER_SEC);
				physics_scene->fetchStart();
			}

			needUpdate_ = false;
		}
    }

    void
	PhysicsFeature::onFrameEnd() noexcept
    {
    }

	void
	PhysicsFeature::onFixedUpdate(const runtime::any& data) noexcept
	{
		needUpdate_ = true;
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