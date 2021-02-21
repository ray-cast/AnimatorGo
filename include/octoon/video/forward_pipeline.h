#ifndef OCTOON_VIDEO_FORWARD_PIPELINE_H_
#define OCTOON_VIDEO_FORWARD_PIPELINE_H_

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
		ForwardPipeline() noexcept;
		virtual ~ForwardPipeline() noexcept;

		void setupFramebuffers(std::uint32_t w, std::uint32_t h) except;

		void clear(const math::float4& val) override;

		void render(const std::shared_ptr<ScriptableRenderContext>& context, const CompiledScene& scene) override;
		void renderTile(const CompiledScene& scene, const math::int2& tileOrigin, const math::int2& tileSize) override;

		const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

	private:
		void setMaterial(const ForwardScene& scene, const std::shared_ptr<material::Material>& material, const camera::Camera& camera, const geometry::Geometry& geometry);

		void renderBuffer(const ForwardScene& scene, const std::shared_ptr<mesh::Mesh>& mesh, std::size_t subset);
		void renderObject(const ForwardScene& scene, const geometry::Geometry& geometry, const camera::Camera& camera, const std::shared_ptr<material::Material>& overrideMaterial = nullptr) noexcept;
		void renderObjects(const ForwardScene& scene, const std::vector<geometry::Geometry*>& objects, const camera::Camera& camera, const std::shared_ptr<material::Material>& overrideMaterial = nullptr) noexcept;
		void renderShadowMaps(const ForwardScene& scene, const std::vector<light::Light*>& lights, const std::vector<geometry::Geometry*>& geometries) noexcept;

	private:
		ForwardPipeline(const ForwardPipeline&) = delete;
		ForwardPipeline& operator=(const ForwardPipeline&) = delete;

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		hal::GraphicsFramebufferPtr fbo_;
		hal::GraphicsFramebufferPtr fbo2_;
		hal::GraphicsTexturePtr colorTexture_;
		hal::GraphicsTexturePtr depthTexture_;
		hal::GraphicsTexturePtr colorTexture2_;
		hal::GraphicsTexturePtr depthTexture2_;

		std::shared_ptr<geometry::Geometry> screenGeometry_;
		std::shared_ptr<material::Material> overrideMaterial_;
		std::shared_ptr<ScriptableRenderContext> cmd_;

		std::unordered_map<std::intptr_t, std::shared_ptr<hal::GraphicsTexture>> lightTextures_;
	};
}

#endif