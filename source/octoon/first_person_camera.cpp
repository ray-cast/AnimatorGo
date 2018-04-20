#include <octoon/first_person_camera.h>
#include <octoon/game_server.h>
#include <octoon/input_feature.h>
#include <octoon/camera_component.h>
#include <octoon/transform_component.h>
#include <octoon/input/input.h>

namespace octoon
{
	OctoonImplementSubClass(FirstPersonCameraComponent, GameComponent, "FirstPersonCamera")

	FirstPersonCameraComponent::FirstPersonCameraComponent() noexcept
		: _speed(10.0)
		, _gravity(15)
		, _maxVelocityChange(1.0)
		, _jumpHeight(10)
		, _sensitivityX(150)
		, _sensitivityY(150)
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
		/*auto inputFeature = GameServer::instance()->getFeature<InputFeature>();
		if (inputFeature)
			inputFeature->getInput()->lockCursor(false);

		this->removeComponentDispatch(GameDispatchType::Frame);*/
	}

	void
	FirstPersonCameraComponent::onFrame() noexcept
	{
		/*float delta = GameServer::instance()->getTimer()->delta();
		float step = _speed * delta;

		auto inputFeature = GameServer::instance()->getFeature<InputFeature>();
		if (inputFeature)
		{
			auto input = inputFeature->getInput();
			if (!input)
				return;

			if (input->isKeyPressed(input::InputKey::Code::LeftShift))
				step *= 3;

			if (!input->isLockedCursor())
				return;

			if (input->isKeyPressed(input::InputKey::Code::W))
				moveCamera(step);

			if (input->isKeyPressed(input::InputKey::Code::A))
				yawCamera(-step);

			if (input->isKeyPressed(input::InputKey::Code::S))
				moveCamera(-step);

			if (input->isKeyPressed(input::InputKey::Code::D))
				yawCamera(step);

			rotateCamera(input->getAxis(input::InputAxis::Horizontal), input->getAxis(input::InputAxis::Vertical));
		}*/
	}

	/*void
	FirstPersonCameraComponent::onMessage(const MessagePtr& message) noexcept
	{
		if (!message->isInstanceOf<InputMessage>())
			return;

		auto inputFeature = GameServer::instance()->getFeature<InputFeature>();
		if (inputFeature)
		{
			auto input = inputFeature->getInput();
			if (!input)
				return;

			if (input->isKeyDown(InputKey::Code::Escape))
				input->lockCursor(input->isLockedCursor() ^ 1);
		}
	}*/

	void
	FirstPersonCameraComponent::yawCamera(float speed) noexcept
	{
		const math::float3& right = this->getGameObject()->getComponent<TransformComponent>()->getLocalRight();
		this->getGameObject()->getComponent<TransformComponent>()->setTranslateAccum(right * speed);
	}

	void
	FirstPersonCameraComponent::moveCamera(float speed) noexcept
	{
		const math::float3& forward = this->getGameObject()->getComponent<TransformComponent>()->getLocalForward();
		this->getGameObject()->getComponent<TransformComponent>()->setTranslateAccum(forward * speed);
	}

	void
	FirstPersonCameraComponent::rotateCamera(float angle, const math::float3& axis) noexcept
	{
		math::Quaternion quat(axis, angle);
		this->getGameObject()->getComponent<TransformComponent>()->setQuaternionAccum(quat);
	}

	void
	FirstPersonCameraComponent::rotateCamera(float axisX, float axisY) noexcept
	{
		float angleY = axisX * _sensitivityX;
		float angleX = axisY * _sensitivityY;

		math::float3 euler(math::euler_angles(this->getGameObject()->getComponent<TransformComponent>()->getLocalQuaternion()));

		float angle = angleX + euler.x;
		if (angle > -89.0f && angle < 89.0f && !std::isinf(angle))
			rotateCamera(angleX, this->getGameObject()->getComponent<TransformComponent>()->getLocalRight());

		rotateCamera(angleY, math::float3::UnitY);
	}

	GameComponentPtr
	FirstPersonCameraComponent::clone() const noexcept
	{
		return std::make_shared<FirstPersonCameraComponent>();
	}
}