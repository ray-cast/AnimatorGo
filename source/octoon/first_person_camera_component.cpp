#include <octoon/first_person_camera_component.h>
#include <octoon/camera_component.h>
#include <octoon/transform_component.h>
#include <octoon/input/input.h>
#include <octoon/input_feature.h>
#include <octoon/timer_feature.h>
#include <iostream>

namespace octoon
{
	OctoonImplementSubClass(FirstPersonCameraComponent, GameComponent, "FirstPersonCamera")

	FirstPersonCameraComponent::FirstPersonCameraComponent() noexcept
		: speed_(50.0f)
		, accelerationSpeed_(1.0f)
		, wheelSpeed_(2.0f)
		, gravity_(15)
		, maxVelocityChange_(1.0)
		, jumpHeight_(10)
		, sensitivityX_(0.20f)
		, sensitivityY_(0.20f)
	{
	}

	FirstPersonCameraComponent::~FirstPersonCameraComponent() noexcept
	{
	}

	void
	FirstPersonCameraComponent::setSpeed(float speed) noexcept
	{
		speed_ = speed;
	}

	void
	FirstPersonCameraComponent::setGravity(float gravity) noexcept
	{
		gravity_ = gravity;
	}

	void
	FirstPersonCameraComponent::setMaxVelocityChange(float maxVelocityChange) noexcept
	{
		maxVelocityChange_ = maxVelocityChange;
	}

	void
	FirstPersonCameraComponent::setJumpHeight(float jumpHeight) noexcept
	{
		jumpHeight_ = jumpHeight;
	}

	void
	FirstPersonCameraComponent::setSensitivityX(float sensitivityX) noexcept
	{
		sensitivityX_ = sensitivityX;
	}

	void
	FirstPersonCameraComponent::setSensitivityY(float sensitivityY) noexcept
	{
		sensitivityY_ = sensitivityY;
	}

	float
	FirstPersonCameraComponent::getSpeed() const noexcept
	{
		return speed_;
	}

	float
	FirstPersonCameraComponent::getGravity() const noexcept
	{
		return gravity_;
	}

	float
	FirstPersonCameraComponent::getMaxVelocityChange() const noexcept
	{
		return maxVelocityChange_;
	}

	float
	FirstPersonCameraComponent::getJumpHeight() const noexcept
	{
		return jumpHeight_;
	}

	float
	FirstPersonCameraComponent::getSensitivityX() const noexcept
	{
		return sensitivityX_;
	}

	float
	FirstPersonCameraComponent::getSensitivityY() const noexcept
	{
		return sensitivityY_;
	}

	void
	FirstPersonCameraComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::Frame);
	}

	void
	FirstPersonCameraComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::Frame);
	}

	void
	FirstPersonCameraComponent::onUpdate() noexcept
	{
		float step = std::min(1.0f, math::pow(accelerationSpeed_ / 6, 2)) * speed_;

		auto timerFeature = this->getFeature<TimerFeature>();
		if (timerFeature)
			step *= timerFeature->frameTime();

		auto inputFeature = this->getFeature<InputFeature>();
		if (inputFeature)
		{
			auto input = inputFeature->getInput();
			if (input)
			{
				bool useAcceleratonSpeed = false;

				if (input->isKeyPressed(input::InputKey::Code::W))
				{
					moveCamera(step);
					useAcceleratonSpeed = true;
				}

				if (input->isKeyPressed(input::InputKey::Code::S))
				{
					moveCamera(-step);
					useAcceleratonSpeed = true;
				}

				if (input->isKeyPressed(input::InputKey::Code::A))
				{
					yawCamera(-step);
					useAcceleratonSpeed = true;
				}

				if (input->isKeyPressed(input::InputKey::Code::D))
				{
					yawCamera(step);
					useAcceleratonSpeed = true;
				}

				if (input->isKeyPressed(input::InputKey::Code::Q))
				{
					upCamera(-step);
					useAcceleratonSpeed = true;
				}

				if (input->isKeyPressed(input::InputKey::Code::E))
				{
					upCamera(step);
					useAcceleratonSpeed = true;
				}

				if (useAcceleratonSpeed)
					accelerationSpeed_ += timerFeature->frameTime();
				else
					accelerationSpeed_ = 1.0f;

				if (input->isButtonUp(input::InputButton::Code::MouseWheel))
					moveCamera(wheelSpeed_);

				if (input->isButtonDown(input::InputButton::Code::MouseWheel))
					moveCamera(-wheelSpeed_);

				if (input->isButtonPressed(input::InputButton::Code::Middle))
				{
					auto x = input->getAxis(input::InputAxis::MouseX) * 0.025f;
					auto y = input->getAxis(input::InputAxis::MouseY) * 0.025f;
					upCamera(y);
					yawCamera(-x);
				}

				if (input->isButtonPressed(input::InputButton::Code::Right))
					rotateCamera(input->getAxis(input::InputAxis::MouseX), input->getAxis(input::InputAxis::MouseY));
			}
		}
	}

	void
	FirstPersonCameraComponent::upCamera(float speed) noexcept
	{
		this->getGameObject()->getComponent<TransformComponent>()->up(speed);
	}

	void
	FirstPersonCameraComponent::yawCamera(float speed) noexcept
	{
		this->getGameObject()->getComponent<TransformComponent>()->yaw(speed);
	}

	void
	FirstPersonCameraComponent::moveCamera(float speed) noexcept
	{
		this->getGameObject()->getComponent<TransformComponent>()->move(speed);
	}

	void
	FirstPersonCameraComponent::rotateCamera(float angle, const math::float3& axis) noexcept
	{
		math::Quaternion quat(axis, math::radians(angle));
		auto transform = this->getGameObject()->getComponent<TransformComponent>();
		transform->setLocalQuaternion(math::normalize(quat) * transform->getLocalQuaternion());
	}

	void
	FirstPersonCameraComponent::rotateCamera(float axisX, float axisY) noexcept
	{
		float angleY = axisX * sensitivityX_;
		float angleX = axisY * sensitivityY_;

		auto transform = this->getGameObject()->getComponent<TransformComponent>();
		math::float3 euler(math::eulerAngles(transform->getLocalQuaternion()));

		float angle = angleX + math::degrees(euler.x);

		if (angle > -89.0f && angle < 89.0f && !std::isinf(angle) && angleX != 0)
			rotateCamera(angleX, transform->getLocalRight());

		if (angleY != 0)
			rotateCamera(angleY, math::float3::UnitY);
	}

	GameComponentPtr
	FirstPersonCameraComponent::clone() const noexcept
	{
		auto instance = std::make_shared<FirstPersonCameraComponent>();
		instance->speed_ = this->speed_;
		instance->gravity_ = this->gravity_;
		instance->maxVelocityChange_ = this->maxVelocityChange_;
		instance->jumpHeight_ = this->jumpHeight_;
		instance->sensitivityX_ = this->sensitivityX_;
		instance->sensitivityY_ = this->sensitivityY_;

		return instance;
	}
}