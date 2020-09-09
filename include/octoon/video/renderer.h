#ifndef OCTOON_RENDERER_H_
#define OCTOON_RENDERER_H_

#include <octoon/runtime/singleton.h>

#include <octoon/hal/graphics.h>

#include <octoon/camera/camera.h>
#include <octoon/light/light.h>
#include <octoon/geometry/geometry.h>

#include <octoon/video/render_object.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/forward_buffer.h>
#include <octoon/video/forward_material.h>
#include <octoon/video/forward_scene.h>

#include <octoon/lightmap/lightmap.h>

#include <unordered_map>

namespace octoon::video
{
	enum class ShadowMode : std::uint8_t
	{
		ShadowModeNone,
		ShadowModeHard,
		ShadowModeSoft,
		ShadowModeBeginRange = ShadowModeNone,
		ShadowModeEndRange = ShadowModeSoft,
		ShadowModeRangeSize = (ShadowModeEndRange - ShadowModeBeginRange + 1),
	};

	enum class ShadowQuality : std::uint8_t
	{
		ShadowQualityNone,
		ShadowQualityLow,
		ShadowQualityMedium,
		ShadowQualityHigh,
		ShadowQualityVeryHigh,
		ShadowQualityBeginRange = ShadowQualityNone,
		ShadowQualityEndRange = ShadowQualityVeryHigh,
		ShadowQualityRangeSize = (ShadowQualityEndRange - ShadowQualityBeginRange + 1),
	};

	class OCTOON_EXPORT Renderer final
	{
		OctoonDeclareSingleton(Renderer)
	public:
		Renderer() noexcept;
		~Renderer() noexcept;

		void setup(const hal::GraphicsContextPtr& context, std::uint32_t w, std::uint32_t h) except;
		void close() noexcept;

		void setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept;
		void getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept;

		void setSortObjects(bool sortObject) noexcept;
		bool getSortObject() const noexcept;

		void setGlobalIllumination(bool enable) noexcept;
		bool getGlobalIllumination() const noexcept;

		void setOverrideMaterial(const std::shared_ptr<material::Material>& material) noexcept;
		const std::shared_ptr<material::Material>& getOverrideMaterial() const noexcept;

		const hal::GraphicsFramebufferPtr& getFramebuffer() const noexcept;

		hal::GraphicsInputLayoutPtr createInputLayout(const hal::GraphicsInputLayoutDesc& desc) noexcept;
		hal::GraphicsDataPtr createGraphicsData(const hal::GraphicsDataDesc& desc) noexcept;
		hal::GraphicsTexturePtr createTexture(const hal::GraphicsTextureDesc& desc) noexcept;
		hal::GraphicsSamplerPtr createSampler(const hal::GraphicsSamplerDesc& desc) noexcept;
		hal::GraphicsFramebufferPtr createFramebuffer(const hal::GraphicsFramebufferDesc& desc) noexcept;
		hal::GraphicsFramebufferLayoutPtr createFramebufferLayout(const hal::GraphicsFramebufferLayoutDesc& desc) noexcept;
		hal::GraphicsShaderPtr createShader(const hal::GraphicsShaderDesc& desc) noexcept;
		hal::GraphicsProgramPtr createProgram(const hal::GraphicsProgramDesc& desc) noexcept;
		hal::GraphicsStatePtr createRenderState(const hal::GraphicsStateDesc& desc) noexcept;
		hal::GraphicsPipelinePtr createRenderPipeline(const hal::GraphicsPipelineDesc& desc) noexcept;
		hal::GraphicsDescriptorSetPtr createDescriptorSet(const hal::GraphicsDescriptorSetDesc& desc) noexcept;
		hal::GraphicsDescriptorSetLayoutPtr createDescriptorSetLayout(const hal::GraphicsDescriptorSetLayoutDesc& desc) noexcept;
		hal::GraphicsDescriptorPoolPtr createDescriptorPool(const hal::GraphicsDescriptorPoolDesc& desc) noexcept;

		void setViewport(const math::float4& viewport) noexcept(false);
		void setFramebuffer(const hal::GraphicsFramebufferPtr& framebuffer) noexcept(false);
		void clearFramebuffer(std::uint32_t i, hal::GraphicsClearFlags flags, const math::float4& color = math::float4::Zero, float depth = 1.0f, std::int32_t stencil = 0) noexcept;

		void generateMipmap(const hal::GraphicsTexturePtr& texture) noexcept;

		void render(RenderScene& scene) noexcept;
		void renderObject(const geometry::Geometry& geometry, const camera::Camera& camera, const std::shared_ptr<material::Material>& overrideMaterial = nullptr) noexcept;
		void renderObjects(const std::vector<geometry::Geometry*>& objects, const camera::Camera& camera, const std::shared_ptr<material::Material>& overrideMaterial = nullptr) noexcept;

	private:
		void prepareShadowMaps(const std::vector<light::Light*>& light, const std::vector<geometry::Geometry*>& geometries) noexcept;

	private:
		bool setBuffer(const std::shared_ptr<mesh::Mesh>& geometry, std::size_t subset);
		bool setProgram(const std::shared_ptr<material::Material>& material, const camera::Camera& camera, const geometry::Geometry& geometry);

	private:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

	private:
		bool sortObjects_;
		bool enableGlobalIllumination_;

		std::uint32_t width_, height_;

		hal::GraphicsContextPtr context_;

		ForwardScene profile_;
		std::unique_ptr<class OfflineRenderer> baikalRenderer_;
		std::unique_ptr<class RtxManager> rtxManager_;
		std::unique_ptr<class ForwardRenderer> forwardRenderer_;

		std::shared_ptr<ForwardBuffer> currentBuffer_;
		std::shared_ptr<material::Material> depthMaterial_;
		std::shared_ptr<material::Material> overrideMaterial_;

		std::unordered_map<std::intptr_t, std::shared_ptr<ForwardBuffer>> buffers_;
		std::unordered_map<std::intptr_t, std::shared_ptr<ForwardMaterial>> materials_;
		std::unordered_map<std::intptr_t, std::shared_ptr<hal::GraphicsTexture>> lightTextures_;
	};
}

#endif