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
		, gravity_(15)
		, maxVelocityChange_(1.0)
		, jumpHeight_(10)
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
	EditorCameraComponent::setGravity(float gravity) noexcept
	{
		gravity_ = gravity;
	}

	void
	EditorCameraComponent::setMaxVelocityChange(float maxVelocityChange) noexcept
	{
		maxVelocityChange_ = maxVelocityChange;
	}

	void
	EditorCameraComponent::setJumpHeight(float jumpHeight) noexcept
	{
		jumpHeight_ = jumpHeight;
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
	EditorCameraComponent::getGravity() const noexcept
	{
		return gravity_;
	}

	float
	EditorCameraComponent::getMaxVelocityChange() const noexcept
	{
		return maxVelocityChange_;
	}

	float
	EditorCameraComponent::getJumpHeight() const noexcept
	{
		return jumpHeight_;
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
					moveCamera(step);

				if (input->isButtonUp(input::InputButton::Code::MouseWheel))
					moveCamera(-step);

				if (input->isButtonPressed(input::InputButton::Code::Right))
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
		const math::float3& up = this->getGameObject()->getComponent<TransformComponent>()->getLocalUp();
		this->getGameObject()->getComponent<TransformComponent>()->setLocalTranslateAccum(up * speed);
	}

	void
	EditorCameraComponent::yawCamera(float speed) noexcept
	{
		const math::float3& right = this->getGameObject()->getComponent<TransformComponent>()->getLocalRight();
		this->getGameObject()->getComponent<TransformComponent>()->setLocalTranslateAccum(right * speed);
	}

	void
	EditorCameraComponent::moveCamera(float speed) noexcept
	{
		const math::float3& forward = this->getGameObject()->getComponent<TransformComponent>()->getLocalForward();
		this->getGameObject()->getComponent<TransformComponent>()->setLocalTranslateAccum(forward * speed);
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
		instance->gravity_ = this->gravity_;
		instance->maxVelocityChange_ = this->maxVelocityChange_;
		instance->jumpHeight_ = this->jumpHeight_;
		instance->sensitivityX_ = this->sensitivityX_;
		instance->sensitivityY_ = this->sensitivityY_;

		return instance;
	}
}