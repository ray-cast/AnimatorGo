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

	void setScale(octoon::model::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setAnchorPoint(octoon::model::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setTranslate(octoon::model::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setOrientation(octoon::model::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setRotationX(octoon::model::AnimationCurve<octoon::math::float1>&& frames) noexcept;
	void setRotationY(octoon::model::AnimationCurve<octoon::math::float1>&& frames) noexcept;
	void setRotationZ(octoon::model::AnimationCurve<octoon::math::float1>&& frames) noexcept;

	void setScale(const octoon::model::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setAnchorPoint(const octoon::model::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setTranslate(const octoon::model::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setOrientation(const octoon::model::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setRotationX(const octoon::model::AnimationCurve<octoon::math::float1>& frames) noexcept;
	void setRotationY(const octoon::model::AnimationCurve<octoon::math::float1>& frames) noexcept;
	void setRotationZ(const octoon::model::AnimationCurve<octoon::math::float1>& frames) noexcept;

	void setScale(octoon::model::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setAnchorPoint(octoon::model::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setTranslate(octoon::model::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setOrientation(octoon::model::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setRotationX(octoon::model::Keyframes<octoon::math::float1>&& frames) noexcept;
	void setRotationY(octoon::model::Keyframes<octoon::math::float1>&& frames) noexcept;
	void setRotationZ(octoon::model::Keyframes<octoon::math::float1>&& frames) noexcept;

	void setScale(const octoon::model::Keyframes<octoon::math::float3>& frames) noexcept;
	void setAnchorPoint(const octoon::model::Keyframes<octoon::math::float3>& frames) noexcept;
	void setTranslate(const octoon::model::Keyframes<octoon::math::float3>& frames) noexcept;
	void setOrientation(const octoon::model::Keyframes<octoon::math::float3>& frames) noexcept;
	void setRotationX(const octoon::model::Keyframes<octoon::math::float1>& frames) noexcept;
	void setRotationY(const octoon::model::Keyframes<octoon::math::float1>& frames) noexcept;
	void setRotationZ(const octoon::model::Keyframes<octoon::math::float1>& frames) noexcept;

	virtual octoon::GameComponentPtr clone() const noexcept override;

private:
	virtual void onActivate() override;
	virtual void onDeactivate() noexcept override;

	virtual void onFrame() except;

private:
	AnimationTransformComponent(const AnimationTransformComponent&) = delete;
	AnimationTransformComponent& operator=(const AnimationTransformComponent&) = delete;

private:
	octoon::model::AnimationCurve<octoon::math::float3> pos_;
	octoon::model::AnimationCurve<octoon::math::float3> scale_;
	octoon::model::AnimationCurve<octoon::math::float3> anchor_;
	octoon::model::AnimationCurve<octoon::math::float3> orientation_;
	octoon::model::AnimationCurve<octoon::math::float1> rx_;
	octoon::model::AnimationCurve<octoon::math::float1> ry_;
	octoon::model::AnimationCurve<octoon::math::float1> rz_;
};

#endif