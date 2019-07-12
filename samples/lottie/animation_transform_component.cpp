#include "animation_transform_component.h"

#include <octoon/game_scene.h>
#include <octoon/transform_component.h>
#include <octoon/camera_component.h>
#include <octoon/timer_feature.h>

OctoonImplementSubClass(AnimationTransformComponent, octoon::GameComponent, "AnimTransformComponent")

AnimationTransformComponent::AnimationTransformComponent() noexcept
{
}

AnimationTransformComponent::~AnimationTransformComponent() noexcept
{
}

void
AnimationTransformComponent::setScale(octoon::animation::Keyframes<octoon::math::float3>&& frames) noexcept
{
	scale_.frames = std::move(frames);
}

void
AnimationTransformComponent::setAnchorPoint(octoon::animation::Keyframes<octoon::math::float3>&& frames) noexcept
{
	anchor_.frames = std::move(frames);
}

void
AnimationTransformComponent::setTranslate(octoon::animation::Keyframes<octoon::math::float3>&& frames) noexcept
{
	pos_.frames = std::move(frames);
}

void
AnimationTransformComponent::setOrientation(octoon::animation::Keyframes<octoon::math::float3>&& frames) noexcept
{
	orientation_.frames = std::move(frames);
}

void
AnimationTransformComponent::setRotationX(octoon::animation::Keyframes<octoon::math::float1>&& frames) noexcept
{
	rx_.frames = std::move(frames);
}

void
AnimationTransformComponent::setRotationY(octoon::animation::Keyframes<octoon::math::float1>&& frames) noexcept
{
	ry_.frames = std::move(frames);
}

void
AnimationTransformComponent::setRotationZ(octoon::animation::Keyframes<octoon::math::float1>&& frames) noexcept
{
	rz_.frames = std::move(frames);
}

void
AnimationTransformComponent::setScale(const octoon::animation::Keyframes<octoon::math::float3>& frames) noexcept
{
	scale_.frames = frames;
}

void
AnimationTransformComponent::setAnchorPoint(const octoon::animation::Keyframes<octoon::math::float3>& frames) noexcept
{
	anchor_.frames = frames;
}

void
AnimationTransformComponent::setTranslate(const octoon::animation::Keyframes<octoon::math::float3>& frames) noexcept
{
	pos_.frames = frames;
}

void
AnimationTransformComponent::setOrientation(const octoon::animation::Keyframes<octoon::math::float3>& frames) noexcept
{
	orientation_.frames = frames;
}

void
AnimationTransformComponent::setRotationX(const octoon::animation::Keyframes<octoon::math::float1>& frames) noexcept
{
	rx_.frames = frames;
}

void
AnimationTransformComponent::setRotationY(const octoon::animation::Keyframes<octoon::math::float1>& frames) noexcept
{
	ry_.frames = frames;
}

void
AnimationTransformComponent::setRotationZ(const octoon::animation::Keyframes<octoon::math::float1>& frames) noexcept
{
	rz_.frames = frames;
}

void
AnimationTransformComponent::setScale(octoon::animation::AnimationCurve<octoon::math::float3>&& frames) noexcept
{
	scale_ = std::move(frames);
}

void
AnimationTransformComponent::setAnchorPoint(octoon::animation::AnimationCurve<octoon::math::float3>&& frames) noexcept
{
	anchor_ = std::move(frames);
}

void
AnimationTransformComponent::setTranslate(octoon::animation::AnimationCurve<octoon::math::float3>&& frames) noexcept
{
	pos_ = std::move(frames);
}

void
AnimationTransformComponent::setOrientation(octoon::animation::AnimationCurve<octoon::math::float3>&& frames) noexcept
{
	orientation_ = std::move(frames);
}

void
AnimationTransformComponent::setRotationX(octoon::animation::AnimationCurve<octoon::math::float1>&& frames) noexcept
{
	rx_ = std::move(frames);
}

void
AnimationTransformComponent::setRotationY(octoon::animation::AnimationCurve<octoon::math::float1>&& frames) noexcept
{
	ry_ = std::move(frames);
}

void
AnimationTransformComponent::setRotationZ(octoon::animation::AnimationCurve<octoon::math::float1>&& frames) noexcept
{
	rz_ = std::move(frames);
}

void
AnimationTransformComponent::setScale(const octoon::animation::AnimationCurve<octoon::math::float3>& frames) noexcept
{
	scale_ = frames;
}

void
AnimationTransformComponent::setAnchorPoint(const octoon::animation::AnimationCurve<octoon::math::float3>& frames) noexcept
{
	anchor_ = frames;
}

void
AnimationTransformComponent::setTranslate(const octoon::animation::AnimationCurve<octoon::math::float3>& frames) noexcept
{
	pos_ = frames;
}

void
AnimationTransformComponent::setOrientation(const octoon::animation::AnimationCurve<octoon::math::float3>& frames) noexcept
{
	orientation_ = frames;
}

void
AnimationTransformComponent::setRotationX(const octoon::animation::AnimationCurve<octoon::math::float1>& frames) noexcept
{
	rx_ = frames;
}

void
AnimationTransformComponent::setRotationY(const octoon::animation::AnimationCurve<octoon::math::float1>& frames) noexcept
{
	ry_ = frames;
}

void
AnimationTransformComponent::setRotationZ(const octoon::animation::AnimationCurve<octoon::math::float1>& frames) noexcept
{
	rz_ = frames;
}

octoon::GameComponentPtr
AnimationTransformComponent::clone() const noexcept
{
	auto instance = std::make_shared<AnimationTransformComponent>();
	instance->setName(instance->getName());
	instance->setScale(this->pos_);
	instance->setOrientation(this->scale_);
	instance->setTranslate(this->orientation_);
	instance->setAnchorPoint(this->anchor_);
	instance->setRotationX(this->rx_);
	instance->setRotationY(this->ry_);
	instance->setRotationZ(this->rz_);

	return instance;
}

void
AnimationTransformComponent::onActivate()
{
	this->addComponentDispatch(octoon::GameDispatchType::Frame);
}

void
AnimationTransformComponent::onDeactivate() noexcept
{
	this->removeComponentDispatchs();
}

void
AnimationTransformComponent::onFrame() except
{
	auto transform = this->getGameObject()->getComponent<octoon::TransformComponent>();
	if (transform)
	{
		float step = 1.0f / 60.0f;
		step = this->getGameObject()->getGameScene()->getFeature<octoon::TimerFeature>()->delta() * 23.9f;

		if (!pos_.empty())
			transform->setTranslate(pos_.evaluate(step));

		if (!scale_.empty())
			transform->setScale(scale_.evaluate(step));

		octoon::math::float3 rotation = octoon::math::float3::Zero;
		if (!rx_.empty()) rotation.x += rx_.evaluate(step);
		if (!ry_.empty()) rotation.y += ry_.evaluate(step);
		if (!rz_.empty()) rotation.z += rz_.evaluate(step);
		if (!orientation_.empty()) rotation += orientation_.evaluate(step);

		octoon::math::Quaternion quat = octoon::math::Quaternion::Zero;
		if (rotation.x != 0.0f) quat = octoon::math::cross(quat, octoon::math::Quaternion(octoon::math::float3::UnitX, rotation.x));
		if (rotation.y != 0.0f) quat = octoon::math::cross(quat, octoon::math::Quaternion(octoon::math::float3::UnitY, rotation.y));
		if (rotation.z != 0.0f) quat = octoon::math::cross(quat, octoon::math::Quaternion(octoon::math::float3::UnitZ, rotation.z));

		if (!anchor_.empty())
		{
			auto hasCamera = this->getGameObject()->getComponent<octoon::CameraComponent>();
			if (hasCamera)
			{
				auto target = anchor_.evaluate(step);
				auto camera = transform->getTranslate();
				auto angle = octoon::math::normalize(target - camera);

				quat = octoon::math::cross(quat, octoon::math::Quaternion(octoon::math::float3(angle.y, angle.x, 0.0f)));
			}
			else
			{
				if (pos_.empty())
					transform->setTranslate(octoon::math::rotate(quat, -anchor_.evaluate(step)) * transform->getScale());
				else
					transform->setTranslateAccum(octoon::math::rotate(quat, -anchor_.evaluate(step)) * transform->getScale());
			}
		}

		transform->setQuaternion(quat);
	}
}