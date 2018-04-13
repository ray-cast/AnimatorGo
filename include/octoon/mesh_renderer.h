#ifndef OCTOON_MESH_RENDERER_H_
#define OCTOON_MESH_RENDERER_H_

#include <octoon/renderer.h>
#include <octoon/video/geometry.h>

namespace octoon
{
	class OCTOON_EXPORT MeshRenderer final : public Renderer
	{
		OctoonDeclareSubClass(MeshRenderer, Renderer)
	public:
		MeshRenderer() noexcept;
		MeshRenderer(video::MaterialPtr&& material) noexcept;
		MeshRenderer(const video::MaterialPtr& material) noexcept;
		virtual ~MeshRenderer() noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void on_activate() noexcept override;
		virtual void on_deactivate() noexcept override;

		virtual void on_move_before() noexcept override;
		virtual void on_move_after() noexcept override;

	private:
		MeshRenderer(const MeshRenderer&) = delete;
		MeshRenderer& operator=(const MeshRenderer&) = delete;

	private:
		video::GeometryPtr geometry_;
	};
}

#endif