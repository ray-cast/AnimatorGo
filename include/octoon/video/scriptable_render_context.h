#ifndef OCTOON_SCRIPTABLE_RENDER_CONTEXT_H_
#define OCTOON_SCRIPTABLE_RENDER_CONTEXT_H_

#include <octoon/hal/graphics_context.h>
#include <octoon/mesh/mesh.h>
#include <octoon/video/collector.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/rendering_data.h>

#include <unordered_map>

namespace octoon
{
	class OCTOON_EXPORT ScriptableRenderContext
	{
	public:
		ScriptableRenderContext();
		ScriptableRenderContext(const hal::GraphicsContextPtr& context);
		~ScriptableRenderContext();

		void setGraphicsContext(const hal::GraphicsContextPtr& context) noexcept(false);
		const hal::GraphicsContextPtr& getGraphicsContext() const noexcept(false);

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

		void generateMipmap(const hal::GraphicsTexturePtr& texture) noexcept;

		void setViewport(std::uint32_t i, const math::float4& viewport) noexcept;
		const math::float4& getViewport(std::uint32_t i) const noexcept;

		void setScissor(std::uint32_t i, const math::uint4& scissor) noexcept;
		const math::uint4& getScissor(std::uint32_t i) const noexcept;

		void setStencilCompareMask(hal::GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept;
		std::uint32_t getStencilCompareMask(hal::GraphicsStencilFaceFlags face) noexcept;

		void setStencilReference(hal::GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept;
		std::uint32_t getStencilReference(hal::GraphicsStencilFaceFlags face) noexcept;

		void setStencilWriteMask(hal::GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept;
		std::uint32_t getStencilWriteMask(hal::GraphicsStencilFaceFlags face) noexcept;

		void setRenderPipeline(const hal::GraphicsPipelinePtr& pipeline) noexcept;
		hal::GraphicsPipelinePtr getRenderPipeline() const noexcept;

		void setDescriptorSet(const hal::GraphicsDescriptorSetPtr& descriptorSet) noexcept;
		hal::GraphicsDescriptorSetPtr getDescriptorSet() const noexcept;

		void setVertexBufferData(std::uint32_t i, const hal::GraphicsDataPtr& data, std::intptr_t offset) noexcept;
		hal::GraphicsDataPtr getVertexBufferData(std::uint32_t i) const noexcept;

		void setIndexBufferData(const hal::GraphicsDataPtr& data, std::intptr_t offset, hal::GraphicsIndexType indexType) noexcept;
		hal::GraphicsDataPtr getIndexBufferData() const noexcept;

		void setFramebuffer(const hal::GraphicsFramebufferPtr& target) noexcept;
		void clearFramebuffer(std::uint32_t i, hal::GraphicsClearFlags flags, const math::float4& color, float depth, std::int32_t stencil) noexcept;
		void discardFramebuffer(const hal::GraphicsFramebufferPtr& src, hal::GraphicsClearFlags flags = hal::GraphicsClearFlagBits::AllBit) noexcept;
		void blitFramebuffer(const hal::GraphicsFramebufferPtr& src, const math::float4& v1, const hal::GraphicsFramebufferPtr& dest, const math::float4& v2) noexcept;
		void readFramebuffer(std::uint32_t i, const hal::GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept;
		void readFramebufferToCube(std::uint32_t i, std::uint32_t face, const hal::GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept;
		hal::GraphicsFramebufferPtr getFramebuffer() const noexcept;

		void draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;
		void drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;
		void drawIndirect(const hal::GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept;
		void drawIndexedIndirect(const hal::GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept;

		void drawMesh(const std::shared_ptr<Mesh>& mesh, std::size_t subset);
		void drawRenderers(const Geometry& geometry, const Camera& camera, const std::shared_ptr<Material>& overrideMaterial = nullptr) noexcept;
		void drawRenderers(const std::vector<Geometry*>& objects, const Camera& camera, const std::shared_ptr<Material>& overrideMaterial = nullptr) noexcept;

		void setMaterial(const std::shared_ptr<Material>& material, const Camera& camera, const Geometry& geometry);

		void cleanCache() noexcept;
		void compileScene(const std::shared_ptr<RenderScene>& scene) noexcept;
		RenderingData& getRenderingData() const noexcept(false);

	private:
		void updateCamera(const std::shared_ptr<RenderScene>& scene, class RenderingData& out, bool force = false);
		void updateLights(const std::shared_ptr<RenderScene>& scene, class RenderingData& out, bool force = false);
		void updateMaterials(const std::shared_ptr<RenderScene>& scene, class RenderingData& out, bool force = false);
		void updateShapes(const std::shared_ptr<RenderScene>& scene, class RenderingData& out, bool force = false);

	private:
		Collector materialCollector;

		hal::GraphicsContextPtr context_;
		std::unique_ptr<class RenderingData> renderingData_;

		std::unordered_map<void*, std::shared_ptr<class ScriptableRenderBuffer>> buffers_;
		std::unordered_map<void*, std::shared_ptr<class ScriptableRenderMaterial>> materials_;
	};
}

#endif
