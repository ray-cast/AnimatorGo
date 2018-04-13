#ifndef OCTOON_POINT_RENDERER_H_
#define OCTOON_POINT_RENDERER_H_

#include <octoon/renderer.h>
#include <octoon/video/geometry.h>

namespace octoon
{
	class OCTOON_EXPORT PointRenderer final : public Renderer
	{
		OctoonDeclareSubClass(PointRenderer, Renderer)
	public:
		PointRenderer() noexcept;
		PointRenderer(video::MaterialPtr&& material) noexcept;
		PointRenderer(const video::MaterialPtr& material) noexcept;
		virtual ~PointRenderer() noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void on_activate() noexcept override;
		virtual void on_deactivate() noexcept override;

		virtual void on_move_before() noexcept override;
		virtual void on_move_after() noexcept override;

	private:
		PointRenderer(const PointRenderer&) = delete;
		PointRenderer& operator=(const PointRenderer&) = delete;

	private:
		video::GeometryPtr geometry_;
	};
}

#endif