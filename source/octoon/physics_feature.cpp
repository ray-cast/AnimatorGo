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
		:physics_system(), physics_context(nullptr)
	{
		physics_context = physics_system.createContext();
	}

	PhysicsFeature::~PhysicsFeature() noexcept
	{
	}

    void PhysicsFeature::onActivate() except
    {
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
}