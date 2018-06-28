#ifndef OCTOON_TRANSFORM_ANIM_COMPONENT_H_
#define OCTOON_TRANSFORM_ANIM_COMPONENT_H_

#include <octoon/model/animation_curve.h>
#include <octoon/game_component.h>

class TransformAnimComponent final : public octoon::GameComponent
{
	OctoonDeclareSubClass(TransformAnimComponent, octoon::GameComponent)
public:
	TransformAnimComponent() noexcept;
	virtual ~TransformAnimComponent() noexcept;

	void setScale(octoon::model::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setTranslate(octoon::model::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setOrientation(octoon::model::Keyframes<octoon::math::float3>&& frames) noexcept;
	void setRotationX(octoon::model::Keyframes<octoon::math::float1>&& frames) noexcept;
	void setRotationY(octoon::model::Keyframes<octoon::math::float1>&& frames) noexcept;
	void setRotationZ(octoon::model::Keyframes<octoon::math::float1>&& frames) noexcept;

	void setScale(const octoon::model::Keyframes<octoon::math::float3>& frames) noexcept;
	void setTranslate(const octoon::model::Keyframes<octoon::math::float3>& frames) noexcept;
	void setOrientation(const octoon::model::Keyframes<octoon::math::float3>& frames) noexcept;
	void setRotationX(const octoon::model::Keyframes<octoon::math::float1>& frames) noexcept;
	void setRotationY(const octoon::model::Keyframes<octoon::math::float1>& frames) noexcept;
	void setRotationZ(const octoon::model::Keyframes<octoon::math::float1>& frames) noexcept;

	void setScale(octoon::model::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setTranslate(octoon::model::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setOrientation(octoon::model::AnimationCurve<octoon::math::float3>&& frames) noexcept;
	void setRotationX(octoon::model::AnimationCurve<octoon::math::float1>&& frames) noexcept;
	void setRotationY(octoon::model::AnimationCurve<octoon::math::float1>&& frames) noexcept;
	void setRotationZ(octoon::model::AnimationCurve<octoon::math::float1>&& frames) noexcept;

	void setScale(const octoon::model::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setTranslate(const octoon::model::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setOrientation(const octoon::model::AnimationCurve<octoon::math::float3>& frames) noexcept;
	void setRotationX(const octoon::model::AnimationCurve<octoon::math::float1>& frames) noexcept;
	void setRotationY(const octoon::model::AnimationCurve<octoon::math::float1>& frames) noexcept;
	void setRotationZ(const octoon::model::AnimationCurve<octoon::math::float1>& frames) noexcept;

	virtual octoon::GameComponentPtr clone() const noexcept override;

private:
	virtual void onActivate() override;
	virtual void onDeactivate() noexcept override;
		
	virtual void onFrame() except;

private:
	TransformAnimComponent(const TransformAnimComponent&) = delete;
	TransformAnimComponent& operator=(const TransformAnimComponent&) = delete;

private:
	octoon::model::AnimationCurve<octoon::math::float3> pos_;
	octoon::model::AnimationCurve<octoon::math::float3> scale_;
	octoon::model::AnimationCurve<octoon::math::float3> orientation_;
	octoon::model::AnimationCurve<octoon::math::float1> rx_;
	octoon::model::AnimationCurve<octoon::math::float1> ry_;
	octoon::model::AnimationCurve<octoon::math::float1> rz_;
};

#endif