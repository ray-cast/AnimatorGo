#include "camera_component.h"
#include "../unreal_profile.h"
#include "../unreal_behaviour.h"

namespace unreal
{
	CameraComponent::CameraComponent() noexcept
	{
	}

	CameraComponent::~CameraComponent() noexcept
	{
	}

	bool
	CameraComponent::loadAnimation(const std::shared_ptr<octoon::Animation>& animation) noexcept(false)
	{
		if (animation)
		{
			auto& profile = this->getContext()->profile;

			auto animator = this->getModel()->camera.getValue()->getComponent<octoon::AnimatorComponent>();
			if (!animator)
				animator = this->getModel()->camera.getValue()->addComponent<octoon::AnimatorComponent>();

			animator->setAnimation(animation);
			animator->sample(profile->playerModule->curTime);

			return true;
		}

		return false;
	}

	void
	CameraComponent::removeAnimation() noexcept(false)
	{
		auto& profile = this->getContext()->profile;
		auto mainCamera = this->getModel()->camera.getValue();
		mainCamera->removeComponent<octoon::AnimatorComponent>();

		profile->cameraModule->reset();
	}

	void
	CameraComponent::onInit() noexcept
	{
		this->getModel()->fov += [this](float value)
		{
			auto camera = this->getModel()->camera.getValue();
			if (camera)
			{
				auto cameraComponent = camera->getComponent<octoon::FilmCameraComponent>();
				if (cameraComponent)
				{
					cameraComponent->setFov(value);
					this->getModel()->focalLength = cameraComponent->getFocalLength();
				}
			}
		};

		this->getModel()->focalLength += [this](float value)
		{
			auto camera = this->getModel()->camera.getValue();
			if (camera)
			{
				auto cameraComponent = camera->getComponent<octoon::FilmCameraComponent>();
				if (cameraComponent)
				{
					cameraComponent->setFocalLength(value);
					this->getModel()->fov = cameraComponent->getFov();
				}
			}
		};

		this->getModel()->focusDistance += [this](float value)
		{
			auto camera = this->getModel()->camera.getValue();
			if (camera)
			{
				auto cameraComponent = camera->getComponent<octoon::FilmCameraComponent>();
				if (cameraComponent)
					cameraComponent->setFocusDistance(value);
			}
		};

		this->getModel()->aperture += [this](float value)
		{
			auto camera = this->getModel()->camera.getValue();
			if (camera)
			{
				auto cameraComponent = camera->getComponent<octoon::FilmCameraComponent>();
				if (cameraComponent)
					cameraComponent->setAperture(this->getModel()->useDepthOfFiled.getValue() ? value : 0.0f);
			}
		};

		this->getModel()->useDepthOfFiled += [this](bool value)
		{
			auto camera = this->getModel()->camera.getValue();
			if (camera)
			{
				auto cameraComponent = camera->getComponent<octoon::FilmCameraComponent>();
				if (cameraComponent)
					cameraComponent->setAperture(value ? this->getModel()->aperture.getValue() : 0.0f);
			}
		};

		this->getModel()->translate += [this](const octoon::math::float3& translate)
		{
			auto camera = this->getModel()->camera.getValue();
			if (camera)
			{
				auto transform = camera->getComponent<octoon::TransformComponent>();
				transform->setTranslate(translate);
			}
		};

		this->getModel()->rotation += [this](const octoon::math::float3& rotation)
		{
			auto camera = this->getModel()->camera.getValue();
			if (camera)
			{
				auto transform = camera->getComponent<octoon::TransformComponent>();
				transform->setEulerAngles(rotation);
			}
		};

		this->getModel()->animation += [this](const std::shared_ptr<octoon::Animation>& value)
		{
			auto mainCamera = this->getModel()->camera.getValue();
			if (mainCamera)
			{
				auto& profile = this->getContext()->profile;

				if (value)
				{
					auto animator = mainCamera->getComponent<octoon::AnimatorComponent>();
					if (!animator)
						animator = mainCamera->addComponent<octoon::AnimatorComponent>();

					animator->setAnimation(value);
					animator->sample(profile->playerModule->curTime);
				}
				else
				{
					mainCamera->removeComponent<octoon::AnimatorComponent>();
				}

				this->update();
			}
		};

		this->getModel()->framebufferSize += [this](const octoon::math::uint2& size)
		{
			auto camera = this->getModel()->camera.getValue();
			if (camera)
			{
				auto cameraComponent = camera->getComponent<octoon::FilmCameraComponent>();
				if (cameraComponent)
				{
					auto framebuffer = cameraComponent->getFramebuffer();
					if (framebuffer)
					{
						if (framebuffer->getFramebufferDesc().getWidth() == size.x &&
							framebuffer->getFramebufferDesc().getHeight() == size.y)
						{
							return;
						}
					}

					cameraComponent->setupFramebuffers(size.x, size.y, 0, octoon::GraphicsFormat::R32G32B32SFloat);
				}
			}
		};

		this->getModel()->camera += [this](const octoon::GameObjectPtr& camera)
		{
			if (camera)
			{
				if (!camera->getComponent<octoon::FirstPersonCameraComponent>())
					camera->addComponent<octoon::FirstPersonCameraComponent>();

				auto cameraComponent = camera->getComponent<octoon::FilmCameraComponent>();
				if (cameraComponent)
				{
					cameraComponent->setCameraType(octoon::CameraType::Main);
					cameraComponent->setClearFlags(octoon::ClearFlagBits::AllBit);
					cameraComponent->setClearColor(octoon::math::float4(0.6f, 0.6f, 0.64f, 1.0f));

					auto size = this->getModel()->framebufferSize.getValue();
					auto framebuffer = cameraComponent->getFramebuffer();
					if (framebuffer)
					{
						if (framebuffer->getFramebufferDesc().getWidth() == size.x &&
							framebuffer->getFramebufferDesc().getHeight() == size.y)
						{
							return;
						}
					}

					cameraComponent->setupFramebuffers(size.x, size.y, 0, octoon::GraphicsFormat::R32G32B32SFloat);
				}

				this->update();
			}
		};

		this->getContext()->profile->playerModule->isPlaying += [this](bool value)
		{
			this->update();
		};

		this->getContext()->profile->playerModule->finish += [this](bool value)
		{
			this->update();
		};

		this->addMessageListener("editor:project:open", [this](const std::any& data)
		{
			this->update();
		});

		this->addMessageListener("editor:player:sample", [this](const std::any& data)
		{
			this->update();
		});
	}

	void
	CameraComponent::onEnable() noexcept
	{
		auto& size = this->getModel()->framebufferSize.getValue();

		auto mainCamera = std::make_shared<octoon::GameObject>(std::string_view("MainCamera"));
		mainCamera->addComponent<octoon::FirstPersonCameraComponent>();
		mainCamera->getComponent<octoon::TransformComponent>()->setTranslate(this->getModel()->translate);
		mainCamera->getComponent<octoon::TransformComponent>()->setEulerAngles(this->getModel()->rotation);

		auto camera = mainCamera->addComponent<octoon::FilmCameraComponent>();
		camera->setFov(this->getModel()->fov);
		camera->setAperture(this->getModel()->useDepthOfFiled ? this->getModel()->aperture.getValue() : 0.0f);
		camera->setCameraType(octoon::CameraType::Main);
		camera->setClearFlags(octoon::ClearFlagBits::AllBit);
		camera->setClearColor(octoon::math::float4(0.6f, 0.6f, 0.64f, 1.0f));
		camera->setupFramebuffers(size.x, size.y, 0, octoon::GraphicsFormat::R32G32B32SFloat);

		this->getModel()->camera = mainCamera;
	}

	void
	CameraComponent::onDisable() noexcept
	{
	}

	void
	CameraComponent::onFixedUpdate() noexcept
	{
		auto& model = this->getModel();

		auto camera = this->getModel()->camera.getValue();
		if (camera)
		{
			auto transformComponent = camera->getComponent<octoon::TransformComponent>();
			if (transformComponent)
			{
				model->translate = camera->getComponent<octoon::TransformComponent>()->getTranslate();
				model->rotation = camera->getComponent<octoon::TransformComponent>()->getEulerAngles();
			}
		}
	}

	void
	CameraComponent::update() noexcept
	{
		auto camera = this->getModel()->camera.getValue();
		if (camera)
		{
			auto& model = this->getModel();

			auto transformComponent = camera->getComponent<octoon::TransformComponent>();
			if (transformComponent)
			{
				model->translate = camera->getComponent<octoon::TransformComponent>()->getTranslate();
				model->rotation = camera->getComponent<octoon::TransformComponent>()->getEulerAngles();
			}

			auto cameraComponent = camera->getComponent<octoon::FilmCameraComponent>();
			if (cameraComponent)
			{
				model->fov = cameraComponent->getFov();
				model->focusDistance = cameraComponent->getFocalDistance();

				if (cameraComponent->getAperture() > 0.0f)
				{
					model->useDepthOfFiled = true;
					model->aperture = cameraComponent->getAperture();
				}
				else
				{
					model->useDepthOfFiled = false;
				}
			}
		}
	}
}