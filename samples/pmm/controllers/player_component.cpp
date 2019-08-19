#include "player_component.h"
#include "mysticlit_behaviour.h"
#include <octoon/timer_feature.h>
#include <octoon/physics_feature.h>

namespace MysticLit
{
	PlayerComponent::PlayerComponent() noexcept
	{
	}

	PlayerComponent::~PlayerComponent() noexcept
	{
	}

	void
	PlayerComponent::setActive(bool active) noexcept
	{
	}

	bool
	PlayerComponent::getActive() const noexcept
	{
		return true;
	}

	void
	PlayerComponent::play() noexcept
	{
		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
		{
			timeFeature->setTimeStep(1.0f / 60.0f);
			timeFeature->setTimeInterval(1.0f / 60.0f);
		}

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
			physicsFeature->setSolverIterationCounts(10);
	}

	void
	PlayerComponent::stop() noexcept
	{
		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
			physicsFeature->setSolverIterationCounts(1);
	}

	void
	PlayerComponent::render() noexcept
	{
		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
		{
			timeFeature->setTimeStep(1.0f);
			timeFeature->setTimeInterval(1.0f / 30.0f);
		}

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
			physicsFeature->setSolverIterationCounts(10);
	}
}