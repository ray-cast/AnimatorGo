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

		void set_translate(const math::float3& v) noexcept;
		void set_translate_accum(const math::float3& v) noexcept;
		const math::float3& get_translate() const noexcept;

		void set_scale(const math::float3& v) noexcept;
		void set_scale_accum(const math::float3& v) noexcept;
		const math::float3& get_scale() const noexcept;

		void set_quaternion(const math::Quaternion& quat) noexcept;
		void set_quaternion_accum(const math::Quaternion& quat) noexcept;
		const math::Quaternion& get_quaternion() const noexcept;

		void set_transform(const math::float4x4& transform) noexcept;
		void set_transform_only_rotate(const math::float4x4& transform) noexcept;
		const math::float4x4& get_transform() const noexcept;
		const math::float4x4& get_transform_inverse() const noexcept;

		const math::float3& get_right() const noexcept;
		const math::float3& get_up() const noexcept;
		const math::float3& get_forward() const noexcept;

		void set_local_translate(const math::float3& v) noexcept;
		void set_local_translate_accum(const math::float3& v) noexcept;
		const math::float3& get_local_translate() const noexcept;

		void set_local_scale(const math::float3& v) noexcept;
		void set_local_scale_all(const float v) noexcept;
		void set_local_scale_accum(const math::float3& v) noexcept;
		const math::float3& get_local_scale() const noexcept;

		void set_local_quaternion(const math::Quaternion& quat) noexcept;
		void set_local_quaternion_accum(const math::Quaternion& quat) noexcept;
		const math::Quaternion& get_local_quaternion() const noexcept;

		void set_local_transform(const math::float4x4& transform) noexcept;
		void set_local_transform_only_rotate(const math::float4x4& transform) noexcept;
		const math::float4x4& get_local_transform() const noexcept;
		const math::float4x4& get_local_transform_inverse() const noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		void on_move_before() except override;
		void on_move_after() except override;

	private:
		void update_local_children() const noexcept;
		void update_world_children() const noexcept;
		void update_local_transform() const noexcept;
		void update_world_transform() const noexcept;
		void update_parent_transform() const noexcept;

	private:
		mutable math::float3 translate_;
		mutable math::float3 scaling_;
		mutable math::Quaternion rotation_;

		mutable math::float3 local_translate_;
		mutable math::float3 local_scaling_;
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