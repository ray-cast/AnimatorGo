#include <octoon/physics_feature.h>

namespace octoon
{
    OctoonImplementSubClass(PhysicsFeature, GameFeature, "PhysicsFeature")

	PhysicsFeature::PhysicsFeature() except
		: physics_system()
		, physics_context(nullptr)
		, physics_scene(nullptr)
		, needUpdate_(false)
		, iterationCounts_(3)
		, timeInterval_(0.02f)
	{
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
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

		physics_context = physics_system.createContext();
		physics_scene = physics_context->createScene(physics::PhysicsSceneDesc());
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
			physics_scene->fetchResult();
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