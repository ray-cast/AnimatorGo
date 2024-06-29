#ifndef OCTOON_SCRIPTABLE_RENDER_CONTEXT_H_
#define OCTOON_SCRIPTABLE_RENDER_CONTEXT_H_

#include <octoon/hal/render_state.h>
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
		ScriptableRenderContext(const GraphicsContextPtr& context);
		~ScriptableRenderContext();

		void setGraphicsContext(const GraphicsContextPtr& context) noexcept(false);
		const GraphicsContextPtr& getGraphicsContext() const noexcept(false);

		GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept;
		GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
		std::shared_ptr<GraphicsTexture> createTexture(const GraphicsTextureDesc& desc) noexcept;
		GraphicsSamplerPtr createSampler(const GraphicsSamplerDesc& desc) noexcept;
		GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept;
		GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept;
		GraphicsShaderPtr createShader(const GraphicsShaderDesc& desc) noexcept;
		GraphicsProgramPtr createProgram(const GraphicsProgramDesc& desc) noexcept;
		std::shared_ptr<RenderState> createRenderState(const RenderStateDesc& desc) noexcept;
		GraphicsPipelinePtr createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept;
		GraphicsDescriptorSetPtr createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept;
		GraphicsDescriptorSetLayoutPtr createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept;
		GraphicsDescriptorPoolPtr createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept;

		void generateMipmap(const std::shared_ptr<GraphicsTexture>& texture) noexcept;

		void setViewport(std::uint32_t i, const math::float4& viewport) noexcept;
		const math::float4& getViewport(std::uint32_t i) const noexcept;

		void setScissor(std::uint32_t i, const math::uint4& scissor) noexcept;
		const math::uint4& getScissor(std::uint32_t i) const noexcept;

		void setStencilCompareMask(StencilFaceFlags face, std::uint32_t mask) noexcept;
		std::uint32_t getStencilCompareMask(StencilFaceFlags face) noexcept;

		void setStencilReference(StencilFaceFlags face, std::uint32_t reference) noexcept;
		std::uint32_t getStencilReference(StencilFaceFlags face) noexcept;

		void setStencilWriteMask(StencilFaceFlags face, std::uint32_t mask) noexcept;
		std::uint32_t getStencilWriteMask(StencilFaceFlags face) noexcept;

		void setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept;
		GraphicsPipelinePtr getRenderPipeline() const noexcept;

		void setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept;
		GraphicsDescriptorSetPtr getDescriptorSet() const noexcept;

		void setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept;
		GraphicsDataPtr getVertexBufferData(std::uint32_t i) const noexcept;

		void setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, IndexFormat indexType) noexcept;
		GraphicsDataPtr getIndexBufferData() const noexcept;

		void configureTarget(const GraphicsFramebufferPtr& target) noexcept;
		void configureClear(ClearFlags flags, const math::float4& color, float depth, std::int32_t stencil) noexcept;
		void discardFramebuffer(const GraphicsFramebufferPtr& src, ClearFlags flags = ClearFlagBits::AllBit) noexcept;
		void blitFramebuffer(const GraphicsFramebufferPtr& src, const math::float4& v1, const GraphicsFramebufferPtr& dest, const math::float4& v2, SamplerFilter filter) noexcept;
		void readFramebuffer(std::uint32_t i, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept;
		void readFramebufferToCube(std::uint32_t i, std::uint32_t face, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept;
		GraphicsFramebufferPtr getFramebuffer() const noexcept;

		void draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;
		void drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;
		void drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept;
		void drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept;

		void drawMesh(const std::shared_ptr<Mesh>& mesh, std::size_t subset, const RenderingData& renderingData);
		void drawRenderers(const Geometry& geometry, const Camera& camera, const RenderingData& renderingData, const std::shared_ptr<Material>& overrideMaterial = nullptr) noexcept;
		void drawRenderers(const std::vector<Geometry*>& objects, const Camera& camera, const RenderingData& renderingData, const std::shared_ptr<Material>& overrideMaterial = nullptr) noexcept;

		void compileMaterial(const std::shared_ptr<Material>& material, const RenderingData& renderingData);
		void setMaterial(const std::shared_ptr<Material>& material, const RenderingData& renderingData, const Camera& camera, const Geometry& geometry);

	private:
		GraphicsContextPtr context_;

		std::unordered_map<void*, std::shared_ptr<class ScriptableRenderBuffer>> buffers_;
		std::unordered_map<void*, std::shared_ptr<class ScriptableRenderMaterial>> materials_;
	};
}

#endif
