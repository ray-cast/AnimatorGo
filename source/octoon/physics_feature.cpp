#include <stdexcept>

#include <octoon/game_object.h>
#include <octoon/transform_component.h>
#include <octoon/physics_feature.h>
#include <octoon/math/math.h>
#include <octoon/runtime/singleton.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/rtti_factory.h>

namespace octoon
{
    OctoonImplementSubClass(PhysicsFeature, GameFeature, "PhysicsFeature")

	PhysicsFeature::PhysicsFeature() except
		:physics_system(), physics_context(nullptr), physics_scene(nullptr)
	{
		
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
	}

    void PhysicsFeature::onActivate() except
    {
		physics_context = physics_system.createContext();
		physics_scene = physics_context->createScene(physics::PhysicsSceneDesc());
    }

    void PhysicsFeature::onDeactivate() noexcept
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
		physics_scene->simulate(1 / 30.f);
    }

    void PhysicsFeature::onOpenScene(const GameScenePtr& scene) except
    {
    }

	void PhysicsFeature::onCloseScene(const GameScenePtr& scene) noexcept
    {
    }

	std::shared_ptr<physics::PhysicsContext> PhysicsFeature::getContext()
	{
		return physics_context;
	}
	std::shared_ptr<physics::PhysicsScene> octoon::PhysicsFeature::getScene()
	{
		return physics_scene;
	}
}