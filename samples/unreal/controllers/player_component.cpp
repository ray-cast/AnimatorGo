#include "player_component.h"
#include "client_component.h"
#include "unreal_behaviour.h"
#include <octoon/timer_feature.h>
#include <octoon/physics_feature.h>
#include <iostream>

namespace unreal
{
	PlayerComponent::PlayerComponent() noexcept
		: needAnimationEvaluate_(false)
	{
	}

	PlayerComponent::~PlayerComponent() noexcept
	{
	}

	void
	PlayerComponent::play() noexcept
	{
		auto& model = this->getModel();
		model->isPlaying = true;
		model->finish = false;

		auto& context = this->getContext()->profile;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
			timeFeature->setTimeStep(1.0f / model->playFps);

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
		{
			physicsFeature->setEnableSimulate(context->physicsModule->getEnable());
			physicsFeature->setFixedTimeStep(context->physicsModule->fixedTimeStep);
			physicsFeature->setSolverIterationCounts(context->physicsModule->playSolverIterationCounts);
		}

		auto& sound = this->getContext()->profile->soundModule->sound.getValue();
		if (sound)
		{
			auto source = sound->getComponent<octoon::AudioSourceComponent>();
			if (source)
			{
				source->setTime(model->curTime);
				source->play();
			}
		}

		for (auto& it : this->getContext()->profile->entitiesModule->objects.getValue())
		{
			for (auto component : it->getComponents())
			{
				if (!component->isInstanceOf<octoon::AnimatorComponent>())
					continue;

				auto animator = component->downcast<octoon::AnimatorComponent>();
				for (auto& bone : animator->getAvatar())
				{
					for (auto& child : bone->getChildren())
					{
						auto rigidbody = child->getComponent<octoon::RigidbodyComponent>();
						if (rigidbody)
						{
							auto transform = child->getComponent<octoon::TransformComponent>();
							transform->setAllowRelativeMotion(rigidbody->getIsKinematic());
						}
					}
				}
			}
		}

		this->timer_.reset();

		this->sendMessage("editor:player:play");
	}

	void
	PlayerComponent::pause() noexcept
	{
		auto& model = this->getModel();
		model->isPlaying = false;
		model->takeupTime = 0;
		model->finish = false;

		auto& context = this->getContext()->profile;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
			timeFeature->setTimeStep(1.0f / model->previewFps);

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
		{
			physicsFeature->setEnableSimulate(context->physicsModule->getEnable());
			physicsFeature->setFixedTimeStep(context->physicsModule->fixedTimeStep);
			physicsFeature->setSolverIterationCounts(context->physicsModule->previewSolverIterationCounts);
		}

		auto& sound = this->getContext()->profile->soundModule->sound.getValue();
		if (sound)
			sound->getComponent<octoon::AudioSourceComponent>()->pause();

		for (auto& it : this->getContext()->profile->entitiesModule->objects.getValue())
		{
			for (auto component : it->getComponents())
			{
				if (!component->isInstanceOf<octoon::AnimatorComponent>())
					continue;

				auto animator = component->downcast<octoon::AnimatorComponent>();
				for (auto& bone : animator->getAvatar())
				{
					for (auto& child : bone->getChildren())
					{
						auto transform = child->getComponent<octoon::TransformComponent>();
						transform->setAllowRelativeMotion(true);
					}
				}
			}
		}

		this->sendMessage("editor:player:pause");
	}

	void
	PlayerComponent::render() noexcept
	{
		this->reset();

		auto& model = this->getModel();
		model->isPlaying = true;
		model->takeupTime = 0;
		model->finish = false;

		auto& context = this->getContext()->profile;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
			timeFeature->setTimeStep(1.0f / model->recordFps);

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
		{
			physicsFeature->setEnableSimulate(false);
			physicsFeature->setFixedTimeStep(context->physicsModule->fixedTimeStep);
			physicsFeature->setSolverIterationCounts(context->physicsModule->recordSolverIterationCounts);
		}

		this->timer_.reset();

		this->sendMessage("editor:player:play");
	}

	void
	PlayerComponent::reset() noexcept
	{
		auto& model = this->getModel();
		model->curTime = model->startFrame / 30.0f;
		model->isPlaying = false;
		model->takeupTime = 0;
		model->finish = true;

		auto& context = this->getContext()->profile;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
			timeFeature->setTimeStep(1.0f / model->previewFps);

		auto camera = this->getContext()->profile->cameraModule->camera.getValue();
		if (camera)
		{
			auto animation = camera->getComponent<octoon::AnimationComponent>();
			if (animation)
			{
				animation->setTime(model->curTime);
				animation->sample();
			}
		}

		auto& sound = this->getContext()->profile->soundModule->sound.getValue();
		if (sound)
		{
			auto audioSource = sound->getComponent<octoon::AudioSourceComponent>();
			if (audioSource)
			{
				audioSource->setTime(model->curTime);
				audioSource->reset();
			}
		}

		for (auto& it : this->getContext()->profile->entitiesModule->objects.getValue())
		{
			for (auto component : it->getComponents())
			{
				if (!component->isInstanceOf<octoon::AnimatorComponent>())
					continue;

				auto animator = component->downcast<octoon::AnimatorComponent>();
				if (animator)
				{
					for (auto& bone : animator->getAvatar())
					{
						for (auto& child : bone->getChildren())
						{
							auto transform = child->getComponent<octoon::TransformComponent>();
							transform->setAllowRelativeMotion(true);
						}
					}

					animator->setTime(model->curTime);
					animator->sample();

					for (auto& transform : animator->getAvatar())
					{
						auto solver = transform->getComponent<octoon::CCDSolverComponent>();
						if (solver)
							solver->solve();
					}
				}
			}

			if (this->getContext()->profile->offlineModule->getEnable())
			{
				auto smr = it->getComponent<octoon::SkinnedMeshRendererComponent>();
				if (smr)
					smr->uploadMeshData();
			}
		}

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
		{
			physicsFeature->setEnableSimulate(context->physicsModule->getEnable());
			physicsFeature->setFixedTimeStep(context->physicsModule->fixedTimeStep);
			physicsFeature->setSolverIterationCounts(context->physicsModule->previewSolverIterationCounts);

			if (context->physicsModule->getEnable())
				physicsFeature->simulate(timeFeature->getTimeStep());
		}

		for (auto& it : this->getContext()->profile->entitiesModule->objects.getValue())
		{
			for (auto component : it->getComponents())
			{
				if (!component->isA<octoon::AnimationComponent>())
					continue;

				auto animation = component->downcast<octoon::AnimationComponent>();
				animation->setTime(model->curTime);
				animation->sample();

				if (animation->isInstanceOf<octoon::AnimatorComponent>())
				{
					auto animator = component->downcast<octoon::AnimatorComponent>();
					for (auto& transform : animator->getAvatar())
					{
						auto solver = transform->getComponent<octoon::CCDSolverComponent>();
						if (solver)
							solver->solve();
					}
				}
			}
		}
	}

	void
	PlayerComponent::sample(float delta) noexcept
	{
		auto& model = this->getModel();
		model->curTime = std::clamp<float>(model->curTime + delta, 0.0f, model->timeLength);

		auto& context = this->getContext();

		auto& sound = context->profile->soundModule->sound.getValue();
		if (sound)
		{
			auto source = sound->getComponent<octoon::AudioSourceComponent>();
			if (source)
				source->setTime(model->curTime);
		}

		for (auto& it : context->profile->entitiesModule->objects.getValue())
		{
			for (auto component : it->getComponents())
			{
				if (!component->isA<octoon::AnimationComponent>())
					continue;

				auto animation = component->downcast<octoon::AnimationComponent>();
				animation->setTime(model->curTime);
				animation->sample();

				if (animation->isInstanceOf<octoon::AnimatorComponent>())
				{
					auto animator = component->downcast<octoon::AnimatorComponent>();
					for (auto& transform : animator->getAvatar())
					{
						auto solver = transform->getComponent<octoon::CCDSolverComponent>();
						if (solver)
							solver->solve();
					}
				}
			}

			if (context->profile->offlineModule->getEnable())
			{
				auto smr = it->getComponent<octoon::SkinnedMeshRendererComponent>();
				if (smr)
					smr->uploadMeshData();
			}
		}

		auto camera = context->profile->cameraModule->camera.getValue();
		if (camera)
		{
			auto animation = camera->getComponent<octoon::AnimationComponent>();
			if (animation)
			{
				animation->setTime(model->curTime);
				animation->sample();
			}
		}

		auto physicsFeature = context->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature && context->profile->physicsModule->getEnable())
		{
			physicsFeature->simulate(std::abs(delta));
		}

		for (auto& it : context->profile->entitiesModule->objects.getValue())
		{
			for (auto component : it->getComponents())
			{
				if (!component->isInstanceOf<octoon::AnimatorComponent>())
					continue;

				auto animator = component->downcast<octoon::AnimatorComponent>();
				if (animator)
				{
					animator->setTime(model->curTime);
					animator->sample();

					for (auto& transform : animator->getAvatar())
					{
						auto solver = transform->getComponent<octoon::CCDSolverComponent>();
						if (solver)
							solver->solve();
					}
				}
			}
		}

		if (camera)
			this->updateDofTarget();

		this->sendMessage("editor:player:sample");
	}

	void
	PlayerComponent::evaluate(float delta) noexcept
	{
		auto& model = this->getModel();
		model->curTime = model->curTime.getValue() + delta;

		auto& profile = this->getContext()->profile;

		auto& sound = profile->soundModule->sound.getValue();
		if (sound)
		{
			auto source = sound->getComponent<octoon::AudioSourceComponent>();
			if (source && source->isPlaying())
			{
				auto time = source->getTime();
				if (time > (model->curTime + model->syncTime) || time < (model->curTime - model->syncTime))
					model->curTime = source->getTime();
			}
		}

		for (auto& it : profile->entitiesModule->objects.getValue())
		{
			for (auto component : it->getComponents())
			{
				if (!component->isA<octoon::AnimationComponent>())
					continue;

				auto animation = component->downcast<octoon::AnimationComponent>();
				animation->setTime(model->curTime);
				animation->evaluate();

				if (animation->isInstanceOf<octoon::AnimatorComponent>())
				{
					auto animator = component->downcast<octoon::AnimatorComponent>();
					for (auto& transform : animator->getAvatar())
					{
						auto solver = transform->getComponent<octoon::CCDSolverComponent>();
						if (solver)
							solver->solve();
					}
				}
			}

			if (profile->offlineModule->getEnable())
			{
				auto smr = it->getComponent<octoon::SkinnedMeshRendererComponent>();
				if (smr)
					smr->uploadMeshData();
			}
		}

		auto camera = profile->cameraModule->camera.getValue();
		if (camera)
		{
			auto animation = camera->getComponent<octoon::AnimationComponent>();
			if (animation)
			{
				animation->setTime(model->curTime);
				animation->evaluate();
			}
		}

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature && profile->physicsModule->getEnable())
			physicsFeature->simulate(delta);

		if (camera)
			this->updateDofTarget();
	}

	void
	PlayerComponent::updateTimeLength() noexcept
	{
		float timeLength = 0;

		auto camera = this->getContext()->profile->cameraModule->camera.getValue();
		if (camera)
		{
			auto animation = camera->getComponent<octoon::AnimationComponent>();
			if (animation)
				timeLength = std::max(timeLength, animation->getCurrentAnimatorStateInfo().timeLength);
		}

		for (auto& it : this->getContext()->profile->entitiesModule->objects.getValue())
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

		auto& model = this->getModel();
		model->timeLength = timeLength;
		model->startFrame = 0;
		model->endFrame = static_cast<std::uint32_t>(model->timeLength * 30.0f);
	}

	void
	PlayerComponent::updateDofTarget() noexcept
	{
		auto camera = this->getContext()->profile->cameraModule->camera.getValue();
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

					if (mesh)
					{
						auto& aabb = mesh->getBoundingBox(model->dofTarget->mesh);
						auto center = hitObject->getComponent<octoon::TransformComponent>()->getTransform() * aabb.center();

						filmCamera->setFocusDistance(octoon::math::distance(center, cameraPos));
					}
				}
			}
		}
	}

	void
	PlayerComponent::onEnable() noexcept
	{
		auto& model = this->getModel();
		auto& context = this->getContext()->profile;

		auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
		if (timeFeature)
			timeFeature->setTimeStep(1.0f / model->previewFps);

		auto physicsFeature = this->getContext()->behaviour->getFeature<octoon::PhysicsFeature>();
		if (physicsFeature)
		{
			physicsFeature->setEnableSimulate(context->physicsModule->enable);
			physicsFeature->setGravity(context->physicsModule->gravity * context->physicsModule->gravityScale);
			physicsFeature->setSolverIterationCounts(context->physicsModule->previewSolverIterationCounts);
			physicsFeature->setFixedTimeStep(1.0f / model->previewFps);
		}
	}

	void
	PlayerComponent::onDisable() noexcept
	{
	}

	void
	PlayerComponent::onLateUpdate() noexcept
	{
		auto& model = this->getModel();
		auto& profile = this->getContext()->profile;

		if (!model->isPlaying)
			return;

		if (profile->offlineModule->getEnable() && profile->recordModule->active)
		{
			if (profile->offlineModule->sppCount >= profile->offlineModule->spp)
			{
				needAnimationEvaluate_ = true;

				this->sendMessage("editor:player:record");
				this->timer_.update();
				this->getContext()->profile->offlineModule->sppCount = 0;

				auto curFrame = static_cast<int>(std::max(1.0f, std::round(model->curTime * 30.0f)));
				auto totalFrame = static_cast<int>(std::max(1.0f, std::round(model->timeLength * 30.0f)));

				model->takeupTime = model->takeupTime.getValue() + this->timer_.frame_time();
				model->estimatedTime = (totalFrame - curFrame) * (model->takeupTime / curFrame);
			}
		}
		else
		{
			needAnimationEvaluate_ = true;

			this->sendMessage("editor:player:record");
			this->timer_.update();

			auto curFrame = static_cast<int>(std::max(1.0f, std::round(model->curTime * 30.0f)));
			auto totalFrame = static_cast<int>(std::max(1.0f, std::round(model->timeLength * 30.0f)));

			model->takeupTime = model->takeupTime.getValue() + this->timer_.frame_time();
			model->estimatedTime = (totalFrame - curFrame) * (model->takeupTime / curFrame);
		}
	}

	void
	PlayerComponent::onFixedUpdate() noexcept
	{
		auto& model = this->getModel();

		this->updateDofTarget();

		if (!model->isPlaying)
			return;

		if (model->curTime < model->endFrame / 30.0f)
		{
			if (needAnimationEvaluate_)
			{
				auto timeFeature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
				if (timeFeature)
					this->evaluate(timeFeature->delta());

				needAnimationEvaluate_ = false;
			}
		}
		else
		{
			this->reset();
		}
	}
}