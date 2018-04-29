#include <octoon/physics_feature.h>
#include <octoon/physics.h>
#include <octoon/math/math.h>
#include <octoon/runtime/singleton.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/rtti_factory.h>


namespace octoon
{
    OctoonImplementSubClass(PhysicsFeature, GameFeature, "PhysicsFeature")
    
	PhysicsFeature::PhysicsFeature() noexcept
	{

	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{

	}

	std::shared_ptr<b2World> PhysicsFeature::getWorld() noexcept
	{
		return world;
	}

    void PhysicsFeature::onActivate() except
    {
		// get gravity
		math::Vector2 g = Physics::getGravity();
		b2Vec2 gravity;
		gravity.Set(g.x, g.y);


		world = std::make_unique<b2World>(gravity);

		world->SetAllowSleeping(true);
		world->SetWarmStarting(true);
    }

    void PhysicsFeature::onDeactivate() noexcept
    {

    }

    void PhysicsFeature::onInputEvent(const input::InputEvent& event) noexcept
    {

    }

    void PhysicsFeature::onReset() noexcept
    {

    }

    void PhysicsFeature::onFrameBegin() noexcept
    {

    }

    void PhysicsFeature::onFrame() except
    {

    }

    void PhysicsFeature::onFrameEnd() noexcept
    {

    }

    void PhysicsFeature::onOpenScene(const GameScenePtr& scene) except
    {

    }

	void PhysicsFeature::onCloseScene(const GameScenePtr& scene) noexcept
    {

    }

}