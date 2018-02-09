#ifndef OCTOON_TRANSFORM_COMPONENT
#define OCTOON_TRANSFORM_COMPONENT

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
		void setScaleAll(const float v) noexcept;
		void setScaleAccum(const math::float3& v) noexcept;
		const math::float3& getScale() const noexcept;

		void setQuaternion(const math::Quaternion& quat) noexcept;
		void setQuaternionAccum(const math::Quaternion& quat) noexcept;
		const math::Quaternion& getQuaternion() const noexcept;

		void setTransform(const math::float4x4& transform) noexcept;
		void setTransformOnlyRotate(const math::float4x4& transform) noexcept;
		const math::float4x4& getTransform() const noexcept;
		const math::float4x4& getTransformInverse() const noexcept;

		void setWorldTranslate(const math::float3& v) noexcept;
		void setWorldTranslateAccum(const math::float3& v) noexcept;
		const math::float3& getWorldTranslate() const noexcept;

		void setWorldScale(const math::float3& v) noexcept;
		void setWorldScaleAccum(const math::float3& v) noexcept;
		const math::float3& getWorldScale() const noexcept;

		void setWorldQuaternion(const math::Quaternion& quat) noexcept;
		void setWorldQuaternionAccum(const math::Quaternion& quat) noexcept;
		const math::Quaternion& getWorldQuaternion() const noexcept;

		const math::float3& getRight() const noexcept;
		const math::float3& getUpVector() const noexcept;
		const math::float3& getForward() const noexcept;

		void setWorldTransform(const math::float4x4& transform) noexcept;
		void setWorldTransformOnlyRotate(const math::float4x4& transform) noexcept;
		const math::float4x4& getWorldTransform() const noexcept;
		const math::float4x4& getWorldTransformInverse() const noexcept;

		virtual GameComponentPtr clone() const noexcept;

	private:
		void _onMoveBefore() except;
		void _onMoveAfter() except;

	private:
		void _updateLocalChildren() const noexcept;
		void _updateWorldChildren() const noexcept;
		void _updateLocalTransform() const noexcept;
		void _updateWorldTransform() const noexcept;
		void _updateParentTransform() const noexcept;

	private:

		mutable math::float3 localTranslate_;
		mutable math::float3 localScaling_;
		mutable math::Quaternion localRotation_;

		mutable math::float3 worldTranslate_;
		mutable math::float3 worldScaling_;
		mutable math::Quaternion worldRotation_;

		mutable math::float4x4 localTransform_;
		mutable math::float4x4 localTransformInverse_;

		mutable math::float4x4 worldTransform_;
		mutable math::float4x4 worldTransformInverse_;

		mutable bool localNeedUpdates_;
		mutable bool worldNeedUpdates_;
	};
}

#endif 