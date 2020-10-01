#include "player_component.h"
#include "client_component.h"
#include "rabbit_behaviour.h"
#include <octoon/timer_feature.h>
#include <octoon/physics_feature.h>

namespace rabbit
{
	PlayerComponent::PlayerComponent() noexcept
		: time_(0)
		, timeStep_(0.0f)
		, timeInterval_(0.0f)
		, timeCount_(0)
		, needUpdate_(false)
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
			physicsFeature->setGravity(context->physicsModule->gravity);

		this->addMessageListener("rabbit:project:open", [this](const std::any& data)
		{
			auto& model = this->getModel();
			model->timeLength = this->getContext()->behaviour->getComponent<PlayerComponent>()->timeLength();
			model->startFrame = 0;
			model->endFrame = this->getModel()->timeLength * 30;

			this->sample();
		});
	}

	void
	PlayerComponent::onDisable() noexcept
	{
	}

	void
	PlayerComponent::play() noexcept
	{
		auto& model = this->getModel();

		this->timeStep_ = model->playTimeStep;
		this->timeInterval_ = 1.0f / model->playFps;

		model->playing_ = true;

		auto sound = this->getContext()->profile->entitiesModule->sound;
		if (sound)
			sound->getComponent<octoon::AudioSourceComponent>()->play(true);
	}

	void
	PlayerComponent::stop() noexcept
	{
		this->getModel()->playing_ = false;
	}

	void
	PlayerComponent::reset() noexcept
	{
		auto& model = this->getModel();

		auto camera = this->getContext()->profile->entitiesModule->camera;
		camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 0));
		camera->getComponent<octoon::TransformComponent>()->setQuaternion(octoon::math::Quaternion(0, 0, 0, 1));

		for (auto& it : this->getContext()->profile->entitiesModule->objects)
		{
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimationComponent>())
				{
					auto animation = component->downcast<octoon::AnimationComponent>();
					animation->setTime(model->startFrame / 30.0f);
					animation->sample();
				}
			}
		}

		this->getModel()->playing_ = false;
		this->sendMessage("rabbit:player:start");
	}

	void
	PlayerComponent::render() noexcept
	{
		auto& model = this->getModel();

		for (auto& it : this->getContext()->profile->entitiesModule->objects)
		{
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimationComponent>())
				{
					auto animation = component->downcast<octoon::AnimationComponent>();
					animation->setTime(model->startFrame / 30.0f);
					animation->sample();
				}
			}
		}

		if (this->getContext()->profile->offlineModule->offlineEnable)
		{
			this->timeStep_ = model->playTimeStep;
			this->timeInterval_ = 1.0f / model->recordFps;
		}
		else
		{
			this->timeStep_ = model->playTimeStep;
			this->timeInterval_ = 1.0f / model->recordFps;
		}

		this->getModel()->playing_ = true;
		this->sendMessage("rabbit:player:start");
	}

	void
	PlayerComponent::sample(float delta) noexcept
	{
		for (auto& it : this->getContext()->profile->entitiesModule->objects)
		{
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimationComponent>())
				{
					auto animation = component->downcast<octoon::AnimationComponent>();
					animation->sample(delta);
				}
			}
		}
	}

	void
	PlayerComponent::evaluate(float delta) noexcept
	{
		for (auto& it : this->getContext()->profile->entitiesModule->objects)
		{
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimationComponent>())
				{
					auto animation = component->downcast<octoon::AnimationComponent>();
					animation->evaluate(delta);
				}
			}
		}
	}

	float
	PlayerComponent::time() const noexcept
	{
		float time = 0;

		for (auto& it : octoon::GameObjectManager::instance()->instances())
		{
			if (!it) continue;

			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimationComponent>())
				{
					auto animation = component->downcast<octoon::AnimationComponent>();
					time = std::max(time, animation->getTime());
				}
			}
		}

		return time;
	}

	float
	PlayerComponent::timeLength() const noexcept
	{
		float timeLength = 0;

		for (auto& it : octoon::GameObjectManager::instance()->instances())
		{
			if (!it) continue;

			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimationComponent>())
				{
					auto animation = component->downcast<octoon::AnimationComponent>();
					timeLength = std::max(timeLength, animation->getCurrentAnimatorStateInfo().timeLength);
				}
			}
		}

		return timeLength;
	}

	void
	PlayerComponent::onLateUpdate() noexcept
	{
		auto& model = this->getModel();
		auto& profile = this->getContext()->profile;

		if (!model->playing_)
		{
			time_ = 0;
			return;
		}

		if (profile->offlineModule->offlineEnable)
		{
			timeCount_++;

			if (timeCount_ >= model->spp)
			{
				needUpdate_ = true;
				timeCount_ = 0;
			}
		}
		else
		{
			needUpdate_ = true;
		}
	}

	void
	PlayerComponent::onFixedUpdate() noexcept
	{
		auto& model = this->getModel();
		auto& context = this->getContext();
		auto& profile = context->profile;

		if (!model->playing_)
		{
			time_ = 0;
			return;
		}

		bool finish = true;
		float time = 0;
		for (auto& it : profile->entitiesModule->objects)
		{
			auto animation = it->getComponent<octoon::AnimationComponent>();
			if (animation)
			{
				auto stateInfo = animation->getCurrentAnimatorStateInfo();
				finish &= stateInfo.finish;
				time = std::max(time, stateInfo.time);
			}
		}

		if (time >= model->endFrame / 30.0f)
			finish = true;

		if (!finish)
		{
			if (profile->h265Module->enable)
			{
				if (needUpdate_)
				{
					for (auto& it : context->behaviour->getComponents())
					{
						if (it->getActive())
							it->onPostProcess();
					}

					this->evaluate(timeInterval_);
					needUpdate_ = false;
				}
			}
			else
			{
				auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
				if (timeFeature)
					this->evaluate(timeFeature->getTimeInterval());
			}
		}
		else
		{
			this->sendMessage("rabbit:player:finish");
		}
	}
}