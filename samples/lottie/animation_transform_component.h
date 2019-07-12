#ifndef OCTOON_ANIMATION_TRANSFORM_COMPONENT_H_
#define OCTOON_ANIMATION_TRANSFORM_COMPONENT_H_

#include <octoon/animation/animation_curve.h>
#include <octoon/game_component.h>

class AnimationTransformComponent final : public octoon::GameComponent
{
	OctoonDeclareSubClass(AnimationTransformComponent, octoon::GameComponent)
public:
	AnimationTransformComponent() noexcept;
	virtual ~AnimationTransformComponent() noexcept;

	void setScale(octoon::animation::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setAnchorPoint(octoon::animation::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setTranslate(octoon::animation::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setOrientation(octoon::animation::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setRotationX(octoon::animation::AnimationCurve<octoon::math::float1>&& frames) noexcept;
	void setRotationY(octoon::animation::AnimationCurve<octoon::math::float1>&& frames) noexcept;
	void setRotationZ(octoon::animation::AnimationCurve<octoon::math::float1>&& frames) noexcept;

	void setScale(const octoon::animation::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setAnchorPoint(const octoon::animation::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setTranslate(const octoon::animation::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setOrientation(const octoon::animation::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setRotationX(const octoon::animation::AnimationCurve<octoon::math::float1>& frames) noexcept;
	void setRotationY(const octoon::animation::AnimationCurve<octoon::math::float1>& frames) noexcept;
	void setRotationZ(const octoon::animation::AnimationCurve<octoon::math::float1>& frames) noexcept;

	void setScale(octoon::animation::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setAnchorPoint(octoon::animation::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setTranslate(octoon::animation::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setOrientation(octoon::animation::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setRotationX(octoon::animation::Keyframes<octoon::math::float1>&& frames) noexcept;
	void setRotationY(octoon::animation::Keyframes<octoon::math::float1>&& frames) noexcept;
	void setRotationZ(octoon::animation::Keyframes<octoon::math::float1>&& frames) noexcept;

	void setScale(const octoon::animation::Keyframes<octoon::math::float3>& frames) noexcept;
	void setAnchorPoint(const octoon::animation::Keyframes<octoon::math::float3>& frames) noexcept;
	void setTranslate(const octoon::animation::Keyframes<octoon::math::float3>& frames) noexcept;
	void setOrientation(const octoon::animation::Keyframes<octoon::math::float3>& frames) noexcept;
	void setRotationX(const octoon::animation::Keyframes<octoon::math::float1>& frames) noexcept;
	void setRotationY(const octoon::animation::Keyframes<octoon::math::float1>& frames) noexcept;
	void setRotationZ(const octoon::animation::Keyframes<octoon::math::float1>& frames) noexcept;

	virtual octoon::GameComponentPtr clone() const noexcept override;

private:
	virtual void onActivate() override;
	virtual void onDeactivate() noexcept override;

	virtual void onFrame() except;

private:
	AnimationTransformComponent(const AnimationTransformComponent&) = delete;
	AnimationTransformComponent& operator=(const AnimationTransformComponent&) = delete;

private:
	octoon::animation::AnimationCurve<octoon::math::float3> pos_;
	octoon::animation::AnimationCurve<octoon::math::float3> scale_;
	octoon::animation::AnimationCurve<octoon::math::float3> anchor_;
	octoon::animation::AnimationCurve<octoon::math::float3> orientation_;
	octoon::animation::AnimationCurve<octoon::math::float1> rx_;
	octoon::animation::AnimationCurve<octoon::math::float1> ry_;
	octoon::animation::AnimationCurve<octoon::math::float1> rz_;
};

#endif