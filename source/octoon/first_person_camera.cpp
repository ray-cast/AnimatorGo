#include <octoon/first_person_camera.h>
#include <octoon/game_server.h>
#include <octoon/game_app.h>
#include <octoon/camera_component.h>
#include <octoon/transform_component.h>
#include <octoon/input/input.h>
#include <octoon/input_feature.h>
#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(FirstPersonCameraComponent, GameComponent, "FirstPersonCamera")

	FirstPersonCameraComponent::FirstPersonCameraComponent() noexcept
		: _speed(10.0)
		, _gravity(15)
		, _maxVelocityChange(1.0)
		, _jumpHeight(10)
		, _sensitivityX(1.0)
		, _sensitivityY(1.0)
	{
	}

	FirstPersonCameraComponent::~FirstPersonCameraComponent() noexcept
	{
	}

	void
	FirstPersonCameraComponent::onActivate() noexcept
	{
		auto cameraComponent = this->getComponent<CameraComponent>();
		if (cameraComponent)
			_sensitivityX = _sensitivityY * cameraComponent->getRatio();

		this->addComponentDispatch(GameDispatchType::Frame);
	}

	void
	FirstPersonCameraComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::Frame);
	}

	void
	FirstPersonCameraComponent::onFrame() noexcept
	{
		float step = _speed * GameApp::instance()->getFeature<TimerFeature>()->delta();

		auto inputFeature = GameApp::instance()->getFeature<InputFeature>();
		if (inputFeature)
		{
			auto input = inputFeature->getInput();
			if (input)
			{
				if (input->isKeyPressed(input::InputKey::Code::LeftShift))
					step *= 3;

				if (input->isKeyPressed(input::InputKey::Code::W))
					moveCamera(-step);

				if (input->isKeyPressed(input::InputKey::Code::S))
					moveCamera(step);

				if (input->isKeyPressed(input::InputKey::Code::A))
					yawCamera(-step);

				if (input->isKeyPressed(input::InputKey::Code::D))
					yawCamera(step);

				if (input->isKeyPressed(input::InputKey::Code::Q))
					upCamera(-step);

				if (input->isKeyPressed(input::InputKey::Code::E))
					upCamera(step);

				if (input->isButtonPressed(input::InputButton::Code::Right))
					rotateCamera(input->getAxis(input::InputAxis::MouseX) * step, input->getAxis(input::InputAxis::MouseY) * step);
			}
		}
	}

	void
	FirstPersonCameraComponent::upCamera(float speed) noexcept
	{
		const math::float3& up = this->getGameObject()->getComponent<TransformComponent>()->getLocalUp();
		this->getGameObject()->getComponent<TransformComponent>()->setLocalTranslateAccum(up * speed);
	}

	void
	FirstPersonCameraComponent::yawCamera(float speed) noexcept
	{
		const math::float3& right = this->getGameObject()->getComponent<TransformComponent>()->getLocalRight();
		this->getGameObject()->getComponent<TransformComponent>()->setLocalTranslateAccum(right * speed);
	}

	void
	FirstPersonCameraComponent::moveCamera(float speed) noexcept
	{
		const math::float3& forward = this->getGameObject()->getComponent<TransformComponent>()->getLocalForward();
		this->getGameObject()->getComponent<TransformComponent>()->setLocalTranslateAccum(forward * speed);
	}

	void
	FirstPersonCameraComponent::rotateCamera(float angle, const math::float3& axis) noexcept
	{
		math::Quaternion quat(axis, math::radians(angle));
		this->getGameObject()->getComponent<TransformComponent>()->setLocalQuaternionAccum(quat);
	}

	void
	FirstPersonCameraComponent::rotateCamera(float axisX, float axisY) noexcept
	{
		float angleY = -axisX * _sensitivityX;
		float angleX = -axisY * _sensitivityY;

		math::float3 euler(math::euler_angles(this->getGameObject()->getComponent<TransformComponent>()->getLocalQuaternion()));

		float angle = angleX + math::degress(euler.x);

		if (angle > -89.0f && angle < 89.0f && !std::isinf(angle))
		{
			rotateCamera(angleX, this->getGameObject()->getComponent<TransformComponent>()->getLocalRight());
		}

		rotateCamera(angleY, math::float3::UnitY);
	}

	GameComponentPtr
	FirstPersonCameraComponent::clone() const noexcept
	{
		return std::make_shared<FirstPersonCameraComponent>();
	}
}