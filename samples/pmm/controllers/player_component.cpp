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
	PlayerComponent::onEnable() noexcept
	{
		auto& context = this->getContext()->profile;
		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
		{
			physicsFeature->setGravity(context->physicsModule->gravity);
			physicsFeature->setSolverIterationCounts(context->physicsModule->previewSolverIterationCounts);
		}
	}

	void
	PlayerComponent::onDisable() noexcept
	{
	}

	void
	PlayerComponent::play() noexcept
	{
		auto& model = this->getModel();
		auto& context = this->getContext()->profile;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
		{
			timeFeature->setTimeStep(model->playTimeStep);
			timeFeature->setTimeInterval(1.0f / model->playFps);
		}

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
			physicsFeature->setSolverIterationCounts(context->physicsModule->playSolverIterationCounts);
	}

	void
	PlayerComponent::stop() noexcept
	{
		auto& context = this->getContext()->profile;
		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
			physicsFeature->setSolverIterationCounts(context->physicsModule->previewSolverIterationCounts);
	}

	void
	PlayerComponent::render() noexcept
	{
		auto& model = this->getModel();
		auto& context = this->getContext()->profile;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
		{
			if (this->getContext()->profile->offlineModule->offlineEnable)
			{
				timeFeature->setTimeStep(model->recordTimeStep);
				timeFeature->setTimeInterval(1.0f / model->recordFps);
			}
			else
			{
				timeFeature->setTimeStep(model->playTimeStep);
				timeFeature->setTimeInterval(1.0f / model->recordFps);
			}
		}

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
			physicsFeature->setSolverIterationCounts(context->physicsModule->recordSolverIterationCounts);
	}
}