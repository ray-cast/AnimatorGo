#ifndef OCTOON_LINE_RENDERER_H_
#define OCTOON_LINE_RENDERER_H_

#include <octoon/renderer.h>
#include <octoon/video/geometry.h>

namespace octoon
{
	class OCTOON_EXPORT LineRenderer final : public Renderer
	{
		OctoonDeclareSubClass(LineRenderer, Renderer)
	public:
		LineRenderer() noexcept;
		LineRenderer(video::MaterialPtr&& material) noexcept;
		LineRenderer(const video::MaterialPtr& material) noexcept;
		virtual ~LineRenderer() noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void on_activate() noexcept override;
		virtual void on_deactivate() noexcept override;

		virtual void on_move_before() noexcept override;
		virtual void on_move_after() noexcept override;

	private:
		LineRenderer(const LineRenderer&) = delete;
		LineRenderer& operator=(const LineRenderer&) = delete;

	private:
		video::GeometryPtr geometry_;
	};
}

#endif