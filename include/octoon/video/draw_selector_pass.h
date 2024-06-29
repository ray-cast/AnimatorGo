#ifndef OCTOON_DRAW_SELECTOR_PASS_H_
#define OCTOON_DRAW_SELECTOR_PASS_H_

#include <octoon/video/scriptable_render_pass.h>
#include <octoon/material/edge_material.h>
#include <octoon/material/mesh_color_material.h>
#include <octoon/material/mesh_copy_material.h>

namespace octoon
{
	class DrawSelectorPass : public ScriptableRenderPass
	{
	public:
		DrawSelectorPass() noexcept;

		void Execute(ScriptableRenderContext& context, const RenderingData& renderingData) noexcept(false) override;

	private:
		void setupFramebuffers(ScriptableRenderContext& context, std::uint32_t w, std::uint32_t h) except;

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		std::shared_ptr<GraphicsTexture> depthTexture_;
		std::shared_ptr<GraphicsTexture> colorTexture_;

		GraphicsFramebufferPtr colorFramebuffer_;

		std::shared_ptr<EdgeMaterial> edgeMaterial_;
		std::shared_ptr<MeshCopyMaterial> copyMaterial_;
	};
}

#endif