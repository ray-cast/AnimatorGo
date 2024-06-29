#ifndef OCTOON_RENDER_OBJECT_H_
#define OCTOON_RENDER_OBJECT_H_

#include <octoon/math/math.h>
#include <octoon/runtime/rtti.h>
#include <octoon/runtime/object.h>

namespace octoon
{
	class Camera;
}

namespace octoon
{
	class OCTOON_EXPORT RenderListener
	{
	public:
		RenderListener() noexcept;
		virtual ~RenderListener() noexcept;

		virtual void onPreRender(const Camera& camera) noexcept = 0;
		virtual void onPostRender(const Camera& camera) noexcept = 0;
	};

	class OCTOON_EXPORT RenderObject : public Object
	{
		OctoonDeclareSubInterface(RenderObject, Object)
	public:
		RenderObject() noexcept;
		virtual ~RenderObject() noexcept;

		void setLayer(std::uint8_t layer) noexcept;
		std::uint8_t getLayer() const noexcept;

		void setRendererPriority(std::int32_t priority) noexcept;
		std::int32_t getRendererPriority() const noexcept;

		void setVisible(bool enable) noexcept;
		bool getVisible() const noexcept;

		void setDirty(bool dirty) noexcept;
		bool isDirty() const noexcept;

		void setOwnerListener(RenderListener* listener) noexcept;
		RenderListener* getOwnerListener() noexcept;

		void setBoundingBox(const math::BoundingBox& bound) noexcept;
		const math::BoundingBox& getBoundingBox() const noexcept;

		void setTransform(const math::float4x4& transform) noexcept;
		void setTransform(const math::float4x4& transform, const math::float4x4& transformInverse) noexcept;
		const math::float4x4& getTransform() const noexcept;
		const math::float4x4& getTransformInverse() const noexcept;

		const math::float3& getRight() const noexcept;
		const math::float3& getUp() const noexcept;
		const math::float3& getForward() const noexcept;
		const math::float3& getTranslate() const noexcept;

	public:
		virtual void onMoveBefore() noexcept;
		virtual void onMoveAfter() noexcept;

		virtual void onRenderBefore(const Camera& camera) noexcept;
		virtual void onRenderAfter(const Camera& camera) noexcept;

	private:
		bool visible_;
		bool dirty_;

		std::uint8_t layer_;
		std::int32_t priority_;

		math::float4x4 transform_;
		math::float4x4 transformInverse_;
		math::BoundingBox boundingBox_;

		RenderListener* renderListener_;
	};
}

#endif