#ifndef OCTOON_VIDEO_FORWARD_PIPELINE_H_
#define OCTOON_VIDEO_FORWARD_PIPELINE_H_

#include <octoon/hal/graphics_context.h>
#include <octoon/light/light.h>
#include <octoon/geometry/geometry.h>
#include <octoon/video/forward_buffer.h>
#include <octoon/video/forward_material.h>

#include "output.h"
#include "pipeline.h"
#include "forward_scene.h"

namespace octoon::video
{
	class ForwardPipeline : public Pipeline
	{
	public:
		ForwardPipeline(const hal::GraphicsContextPtr& context) noexcept;
		virtual ~ForwardPipeline() noexcept;

		void setupFramebuffers(std::uint32_t w, std::uint32_t h) except;

		void prepareShadowMaps(ForwardScene& scene, const std::vector<light::Light*>& lights, const std::vector<geometry::Geometry*>& geometries) noexcept;

		void renderObject(ForwardScene& scene, const geometry::Geometry& geometry, const camera::Camera& camera, const std::shared_ptr<material::Material>& overrideMaterial) noexcept;
		void renderObjects(ForwardScene& scene, const std::vector<geometry::Geometry*>& objects, const camera::Camera& camera, const std::shared_ptr<material::Material>& overrideMaterial = nullptr) noexcept;

		void render(CompiledScene& scene) noexcept;

		const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

	private:
		bool setBuffer(ForwardScene& scene, const std::shared_ptr<mesh::Mesh>& geometry, std::size_t subset);
		bool setProgram(ForwardScene& scene, const std::shared_ptr<material::Material>& material, const camera::Camera& camera, const geometry::Geometry& geometry);

	private:
		hal::GraphicsContextPtr context_;

		hal::GraphicsFramebufferPtr fbo_;
		hal::GraphicsTexturePtr colorTexture_;
		hal::GraphicsTexturePtr depthTexture_;

		std::shared_ptr<ForwardBuffer> currentBuffer_;
		std::shared_ptr<material::Material> depthMaterial_;
		std::shared_ptr<material::Material> overrideMaterial_;

		std::unordered_map<std::intptr_t, std::shared_ptr<ForwardBuffer>> buffers_;
		std::unordered_map<std::intptr_t, std::shared_ptr<ForwardMaterial>> materials_;
		std::unordered_map<std::intptr_t, std::shared_ptr<hal::GraphicsTexture>> lightTextures_;
	};
}

#endif