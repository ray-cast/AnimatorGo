#include "player_component.h"
#include "client_component.h"
#include "flower_behaviour.h"
#include <octoon/timer_feature.h>
#include <octoon/physics_feature.h>
#include <iostream>

namespace flower
{
	PlayerComponent::PlayerComponent() noexcept
		: time_(0)
		, timeCount_(0)
		, needUpdate_(false)
		, animationLerp_(0)
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

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
			timeFeature->setTimeStep(this->getModel()->normalTimeStep);

		this->addMessageListener("flower:project:open", [this](const std::any& data)
		{
			auto& model = this->getModel();
			model->timeLength = this->getContext()->behaviour->getComponent<PlayerComponent>()->timeLength();
			model->startFrame = 0;
			model->endFrame = this->getModel()->timeLength * 30;

			this->reset();
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
		model->playing_ = true;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
			timeFeature->setTimeStep(model->playTimeStep);

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
			physicsFeature->setEnableSimulate(false);

		auto sound = this->getContext()->profile->entitiesModule->sound;
		if (sound)
		{
			auto source = sound->getComponent<octoon::AudioSourceComponent>();
			if (source)
			{
				source->setTime(model->curTime);
				source->play();
			}
		}
	}

	void
	PlayerComponent::pause() noexcept
	{
		auto& model = this->getModel();
		model->playing_ = false;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
			timeFeature->setTimeStep(model->normalTimeStep);

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
			physicsFeature->setEnableSimulate(true);

		auto sound = this->getContext()->profile->entitiesModule->sound;
		if (sound)
			sound->getComponent<octoon::AudioSourceComponent>()->pause();
	}

	void
	PlayerComponent::render() noexcept
	{
		this->reset();

		auto& model = this->getModel();
		model->playing_ = true;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
			timeFeature->setTimeStep(1.0f / model->recordFps / 5.0f);

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
			physicsFeature->setEnableSimulate(false);
	}

	void
	PlayerComponent::reset() noexcept
	{
		auto& model = this->getModel();
		model->curTime = model->startFrame / 30.0f;
		model->playing_ = false;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
			timeFeature->setTimeStep(model->normalTimeStep);

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
			physicsFeature->setEnableSimulate(true);

		auto camera = this->getContext()->profile->entitiesModule->camera;
		if (camera)
		{
			auto animation = camera->getComponent<octoon::AnimationComponent>();
			if (animation)
			{
				animation->setTime(model->curTime);
				animation->sample();
			}
			else
			{
				camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 0));
				camera->getComponent<octoon::TransformComponent>()->setQuaternion(octoon::math::Quaternion(0, 0, 0, 1));
			}
		}

		auto sound = this->getContext()->profile->entitiesModule->sound;
		if (sound)
		{
			auto soundComponent = sound->getComponent<octoon::AudioSourceComponent>();
			if (soundComponent)
			{
				soundComponent->setTime(model->curTime);
				soundComponent->reset();
			}
		}

		for (auto& it : this->getContext()->profile->entitiesModule->objects)
		{
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimationComponent>())
				{
					auto animation = component->downcast<octoon::AnimationComponent>();
					animation->setTime(model->curTime);
					animation->sample();
				}
			}
		}
	}

	void
	PlayerComponent::sample(float delta) noexcept
	{
		auto& model = this->getModel();
		model->curTime += delta;

		auto sound = this->getContext()->profile->entitiesModule->sound;
		if (sound)
		{
			auto source = sound->getComponent<octoon::AudioSourceComponent>();
			if (source)
				source->setTime(model->curTime);
		}

		for (auto& it : this->getContext()->profile->entitiesModule->objects)
		{
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimationComponent>())
				{
					auto animation = component->downcast<octoon::AnimationComponent>();
					animation->setTime(model->curTime);
					animation->sample();
				}
			}
		}

		if (this->getContext()->profile->offlineModule->offlineEnable)
		{
			for (auto& it : this->getContext()->profile->entitiesModule->objects)
			{
				auto smr = it->getComponent<octoon::SkinnedMeshRendererComponent>();
				if (smr)
					smr->updateMeshData();
			}
		}

		auto camera = this->getContext()->profile->entitiesModule->camera;
		if (camera)
		{
			auto animation = camera->getComponent<octoon::AnimationComponent>();
			if (animation)
			{
				animation->setTime(model->curTime);
				animation->sample();
			}

			this->updateDofTarget();
		}
	}

	void
	PlayerComponent::evaluate(float delta) noexcept
	{
		auto& model = this->getModel();
		model->curTime += delta;

		for (auto& it : this->getContext()->profile->entitiesModule->objects)
		{
			for (auto& component : it->getComponents())
			{
				if (component->isA<octoon::AnimationComponent>())
				{
					auto animation = component->downcast<octoon::AnimationComponent>();
					animation->setTime(model->curTime);
					animation->evaluate();
				}
			}
		}

		if (this->getContext()->profile->offlineModule->offlineEnable)
		{
			for (auto& it : this->getContext()->profile->entitiesModule->objects)
			{
				auto smr = it->getComponent<octoon::SkinnedMeshRendererComponent>();
				if (smr)
					smr->updateMeshData();
			}
		}

		auto camera = this->getContext()->profile->entitiesModule->camera;
		if (camera)
		{
			auto animation = camera->getComponent<octoon::AnimationComponent>();
			if (animation)
			{
				animation->setTime(model->curTime);
				animation->evaluate();
			}

			this->updateDofTarget();
		}

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
		{
			if (!physicsFeature->getEnableSimulate())
				physicsFeature->simulate();
		}
	}

	float
	PlayerComponent::timeLength() const noexcept
	{
		float timeLength = 0;

		auto camera = this->getContext()->profile->entitiesModule->camera;
		if (camera)
		{
			auto animation = camera->getComponent<octoon::AnimationComponent>();
			if (animation)
				timeLength = std::max(timeLength, animation->getCurrentAnimatorStateInfo().timeLength);
		}

		for (auto& it : this->getContext()->profile->entitiesModule->objects)
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
	PlayerComponent::updateDofTarget() noexcept
	{
		auto camera = this->getContext()->profile->entitiesModule->camera;
		if (camera)
		{
			auto cameraPos = camera->getComponent<octoon::TransformComponent>()->getTranslate();
			auto filmCamera = camera->getComponent<octoon::FilmCameraComponent>();

			auto& model = this->getModel();
			if (model->dofTarget)
			{
				auto hitObject = model->dofTarget->object.lock();
				if (hitObject)
				{
					auto meshFilter = hitObject->getComponent<octoon::MeshFilterComponent>();
					auto mesh = meshFilter->getMesh();
					auto& aabb = mesh->getBoundingBox(model->dofTarget->mesh);
					auto center = hitObject->getComponent<octoon::TransformComponent>()->getTransform() * aabb.center();

					octoon::Raycaster raycaster(octoon::math::Raycast(cameraPos, octoon::math::normalize(center - cameraPos)));
					auto& intersects = raycaster.intersectObjects(this->getContext()->profile->entitiesModule->objects);
					if (!intersects.empty())
						filmCamera->setFocalDistance(intersects[0].distance);
					else
						filmCamera->setFocalDistance(octoon::math::distance(center, cameraPos));
				}
			}
		}
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
			if (animationLerp_ == 0)
			{
				timeCount_++;

				if (timeCount_ >= model->spp)
				{
					needUpdate_ = true;
					timeCount_ = 0;
				}
			}
		}
		else
		{
			needUpdate_ = true;
		}

		if (profile->h265Module->enable)
		{
			if (needUpdate_)
			{
				for (auto& it : this->getContext()->behaviour->getComponents())
				{
					if (it->getActive())
						it->onPostProcess();
				}

				needUpdate_ = false;
				animationLerp_ = 5;
			}
		}
	}

	void
	PlayerComponent::onFixedUpdate() noexcept
	{
		auto& model = this->getModel();
		auto& context = this->getContext();
		auto& profile = context->profile;
		this->updateDofTarget();

		if (!model->playing_)
		{
			time_ = 0;
			return;
		}

		if (model->curTime < model->endFrame / 30.0f)
		{
			if (profile->h265Module->enable)
			{
				if (animationLerp_ > 0)
				{
					auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
					if (timeFeature)
						this->evaluate(timeFeature->delta());

					animationLerp_--;
				}
			}
			else
			{
				auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
				if (timeFeature)
					this->evaluate(timeFeature->delta());
			}
		}
		else
		{
			this->reset();
			this->sendMessage("flower:player:finish");
		}
	}
}