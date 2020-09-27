#ifndef OCTOON_TRANSFORM_COMPONENT_H_
#define OCTOON_TRANSFORM_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT TransformComponent final : public GameComponent
	{
		OctoonDeclareSubClass(TransformComponent, GameComponent)
	public:
		TransformComponent() noexcept;
		~TransformComponent();

		void setTranslate(const math::float3& v) noexcept;
		void setTranslateAccum(const math::float3& v) noexcept;
		const math::float3& getTranslate() const noexcept;

		void setScale(const math::float3& v) noexcept;
		void setScaleAccum(const math::float3& v) noexcept;
		const math::float3& getScale() const noexcept;

		void setQuaternion(const math::Quaternion& quat) noexcept;
		void setQuaternionAccum(const math::Quaternion& quat) noexcept;
		const math::Quaternion& getQuaternion() const noexcept;

		void setEulerAngles(const math::float3& euler) noexcept;
		const math::float3& getEulerAngles() const noexcept;

		void setTransform(const math::float4x4& transform) noexcept;
		void setTransform(const math::float3& translate, const math::Quaternion& quat, const math::float3& scale = math::float3::One) noexcept;
		void setTransformOnlyRotate(const math::float4x4& transform) noexcept;
		const math::float4x4& getTransform() const noexcept;
		const math::float4x4& getTransformInverse() const noexcept;

		const math::float3& getRight() const noexcept;
		const math::float3& getUp() const noexcept;
		const math::float3& getForward() const noexcept;

		void setLocalTranslate(const math::float3& v) noexcept;
		void setLocalTranslateAccum(const math::float3& v) noexcept;
		const math::float3& getLocalTranslate() const noexcept;

		void setLocalScale(const math::float3& v) noexcept;
		void setLocalScaleAll(const float v) noexcept;
		void setLocalScaleAccum(const math::float3& v) noexcept;
		const math::float3& getLocalScale() const noexcept;

		void setLocalQuaternion(const math::Quaternion& quat) noexcept;
		void setLocalQuaternionAccum(const math::Quaternion& quat) noexcept;
		const math::Quaternion& getLocalQuaternion() const noexcept;

		void setLocalEulerAngles(const math::float3& euler) noexcept;
		const math::float3& getLocalEulerAngles() const noexcept;

		void setLocalTransform(const math::float4x4& transform) noexcept;
		void setLocalTransformOnlyRotate(const math::float4x4& transform) noexcept;
		const math::float4x4& getLocalTransform() const noexcept;
		const math::float4x4& getLocalTransformInverse() const noexcept;

		const math::float3& getLocalRight() const noexcept;
		const math::float3& getLocalUp() const noexcept;
		const math::float3& getLocalForward() const noexcept;

		void up(float speed) noexcept;
		void up(const math::float3& speed) noexcept;

		void yaw(float speed) noexcept;
		void yaw(const math::float3& speed) noexcept;

		void move(float speed) noexcept;
		void move(const math::float3& speed) noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onMoveBefore() except override;
		void onMoveAfter() except override;

	private:
		friend GameObject;
		void updateLocalChildren() const noexcept;
		void updateWorldChildren() const noexcept;
		void updateLocalTransform() const noexcept;
		void updateWorldTransform() const noexcept;
		void updateParentTransform() const noexcept;

	private:
		mutable math::float3 translate_;
		mutable math::float3 scaling_;
		mutable math::float3 euler_angles_;
		mutable math::Quaternion rotation_;

		mutable math::float3 local_translate_;
		mutable math::float3 local_scaling_;
		mutable math::float3 local_euler_angles_;
		mutable math::Quaternion local_rotation_;

		mutable math::float4x4 transform_;
		mutable math::float4x4 transform_inverse_;

		mutable math::float4x4 local_transform_;
		mutable math::float4x4 local_transform_inverse_;

		mutable bool local_need_updates_;
		mutable bool world_need_updates_;
	};
}

#endif 