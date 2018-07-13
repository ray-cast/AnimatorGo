#include <octoon/editor_camera_component.h>
#include <octoon/game_app.h>
#include <octoon/camera_component.h>
#include <octoon/transform_component.h>
#include <octoon/input/input.h>
#include <octoon/input_feature.h>
#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(EditorCameraComponent, GameComponent, "EditorCameraComponent")

	EditorCameraComponent::EditorCameraComponent() noexcept
		: speed_(10.0)
		, moveSpeed_(10.0f)
		, sensitivityX_(1.0)
		, sensitivityY_(1.0)
	{
	}

	EditorCameraComponent::~EditorCameraComponent() noexcept
	{
	}

	void
	EditorCameraComponent::setSpeed(float speed) noexcept
	{
		speed_ = speed;
	}

	void
	EditorCameraComponent::setMoveSpeed(float speed) noexcept
	{
		moveSpeed_ = speed;
	}

	void
	EditorCameraComponent::setSensitivityX(float sensitivityX) noexcept
	{
		sensitivityX_ = sensitivityX;
	}

	void
	EditorCameraComponent::setSensitivityY(float sensitivityY) noexcept
	{
		sensitivityY_ = sensitivityY;
	}

	float
	EditorCameraComponent::getSpeed() const noexcept
	{
		return speed_;
	}

	float
	EditorCameraComponent::getMoveSpeed() const noexcept
	{
		return speed_;
	}

	float
	EditorCameraComponent::getSensitivityX() const noexcept
	{
		return sensitivityX_;
	}

	float
	EditorCameraComponent::getSensitivityY() const noexcept
	{
		return sensitivityY_;
	}

	void
	EditorCameraComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::Frame);
	}

	void
	EditorCameraComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::Frame);
	}

	void
	EditorCameraComponent::onFrame() noexcept
	{
		float step = speed_;
#if OCTOON_FEATURE_TIMER_ENABLE
		step *= GameApp::instance()->getFeature<TimerFeature>()->delta();
#endif

		auto inputFeature = GameApp::instance()->getFeature<InputFeature>();
		if (inputFeature)
		{
			auto input = inputFeature->getInput();
			if (input)
			{
				if (input->isKeyPressed(input::InputKey::Code::LeftShift))
					step *= 3;

				if (input->isButtonDown(input::InputButton::Code::MouseWheel))
					moveCamera(step * moveSpeed_);

				if (input->isButtonUp(input::InputButton::Code::MouseWheel))
					moveCamera(-step * moveSpeed_);

				if (input->isButtonPressed(input::InputButton::Code::Right))
					rotateCamera(input->getAxis(input::InputAxis::MouseX) * step, input->getAxis(input::InputAxis::MouseY) * step);

				if (input->isButtonPressed(input::InputButton::Code::Middle))
				{
					auto x = input->getAxis(input::InputAxis::MouseX) * step;
					auto y = input->getAxis(input::InputAxis::MouseY) * step;
					upCamera(y);
					yawCamera(-x);
				}
			}
		}
	}

	void
	EditorCameraComponent::upCamera(float speed) noexcept
	{
		this->getGameObject()->getComponent<TransformComponent>()->up(speed);
	}

	void
	EditorCameraComponent::yawCamera(float speed) noexcept
	{
		this->getGameObject()->getComponent<TransformComponent>()->yaw(speed);
	}

	void
	EditorCameraComponent::moveCamera(float speed) noexcept
	{
		this->getGameObject()->getComponent<TransformComponent>()->move(speed);
	}

	void
	EditorCameraComponent::rotateCamera(float angle, const math::float3& axis) noexcept
	{
		math::Quaternion quat(axis, math::radians(angle));
		this->getGameObject()->getComponent<TransformComponent>()->setLocalQuaternionAccum(quat);
	}

	void
	EditorCameraComponent::rotateCamera(float axisX, float axisY) noexcept
	{
		float angleY = axisX * sensitivityX_;
		float angleX = axisY * sensitivityY_;

		auto transform = this->getGameObject()->getComponent<TransformComponent>();
		math::float3 euler(math::eulerAngles(transform->getLocalQuaternion()));

		float angle = angleX + math::degress(euler.x);

		if (angle > -89.0f && angle < 89.0f && !std::isinf(angle))
			rotateCamera(angleX, transform->getLocalRight());

		rotateCamera(angleY, math::float3::UnitY);
	}

	GameComponentPtr
	EditorCameraComponent::clone() const noexcept
	{
		auto instance = std::make_shared<EditorCameraComponent>();
		instance->speed_ = this->speed_;
		instance->moveSpeed_ = this->moveSpeed_;
		instance->sensitivityX_ = this->sensitivityX_;
		instance->sensitivityY_ = this->sensitivityY_;

		return instance;
	}
}