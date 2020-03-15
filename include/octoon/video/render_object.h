#ifndef OCTOON_RENDER_OBJECT_H_
#define OCTOON_RENDER_OBJECT_H_

#include <octoon/video/render_types.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT RenderListener
		{
		public:
			RenderListener() noexcept;
			virtual ~RenderListener() noexcept;

			virtual void onPreRender(const camera::Camera& camera) noexcept = 0;
			virtual void onPostRender(const camera::Camera& camera) noexcept = 0;
		};

		class OCTOON_EXPORT RenderObject : public runtime::RttiInterface
		{
			OctoonDeclareSubInterface(RenderObject, runtime::RttiInterface)
		public:
			RenderObject() noexcept;
			virtual ~RenderObject() noexcept;

			void setActive(bool active) noexcept;
			bool getActive() const noexcept;

			void setLayer(std::uint8_t layer) noexcept;
			std::uint8_t getLayer() const noexcept;

			void setVisible(bool enable) noexcept;
			bool getVisible() const noexcept;

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
			virtual void onActivate() noexcept;
			virtual void onDeactivate() noexcept;

			virtual void onMoveBefore() noexcept;
			virtual void onMoveAfter() noexcept;

			virtual void onRenderBefore(const camera::Camera& camera) noexcept;
			virtual void onRenderAfter(const camera::Camera& camera) noexcept;

		private:
			bool active_;
			bool visible_;

			std::uint8_t layer_;

			math::float4x4 transform_;
			math::float4x4 transformInverse_;
			math::BoundingBox boundingBox_;

			RenderListener* renderListener_;
			RenderScenePtr  renderScene_;
		};
	}
}

#endif