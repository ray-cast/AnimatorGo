#ifndef OCTOON_GL33_CORE_DEVICE_CONTEXT_H_
#define OCTOON_GL33_CORE_DEVICE_CONTEXT_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL45DeviceContext final : public GraphicsContext
		{
			OctoonDeclareSubClass(GL45DeviceContext, GraphicsContext)
		public:
			GL45DeviceContext() noexcept;
			~GL45DeviceContext() noexcept;

			bool setup(const GraphicsContextDesc& desc) noexcept;
			void close() noexcept;

			void renderBegin() noexcept;
			void renderEnd() noexcept;

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

			void setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset = 0, IndexFormat indexType = IndexFormat::UInt32) noexcept;
			GraphicsDataPtr getIndexBufferData() const noexcept;

			void generateMipmap(const std::shared_ptr<GraphicsTexture>& texture) noexcept;

			void setFramebuffer(const GraphicsFramebufferPtr& target) noexcept;
			void clearFramebuffer(std::uint32_t i, ClearFlags flags, const math::float4& color, float depth, std::int32_t stencil) noexcept;
			void discardFramebuffer(const GraphicsFramebufferPtr& src, ClearFlags flags) noexcept;
			void blitFramebuffer(const GraphicsFramebufferPtr& src, const math::float4& v1, const GraphicsFramebufferPtr& dest, const math::float4& v2, SamplerFilter filter) noexcept;
			void readFramebuffer(std::uint32_t i, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept;
			void readFramebufferToCube(std::uint32_t i, std::uint32_t face, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept;
			GraphicsFramebufferPtr getFramebuffer() const noexcept;

			void draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;
			void drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;
			void drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept;
			void drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept;

			void startDebugControl() noexcept;
			void stopDebugControl() noexcept;

			void present() noexcept;

		private:
			bool checkSupport() noexcept;
			bool initStateSystem() noexcept;

			static void GLAPIENTRY debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL45DeviceContext(const GL45DeviceContext&) noexcept = delete;
			GL45DeviceContext& operator=(const GL45DeviceContext&) noexcept = delete;

		private:
			std::vector<GLenum> _attachments;

			GL33SwapchainPtr _glcontext;
			GL33ProgramPtr _program;
			GL45GraphicsDataPtr _indexBuffer;
			GL45PipelinePtr _pipeline;
			GL45FramebufferPtr _framebuffer;
			GL45FramebufferPtr _drawFramebuffer;
			GL45FramebufferPtr _readFramebuffer;
			GL45DescriptorSetPtr _descriptorSet;
			GL45VertexBuffers _vertexBuffers;
			GL33GraphicsStatePtr _state;
			GL33GraphicsStatePtr _stateDefault;

			RenderStateDesc _stateCaptured;

			bool _needUpdatePipeline;
			bool _needUpdateDescriptor;
			bool _needUpdateVertexBuffers;

			GLfloat _clearDepth;
			GLint   _clearStencil;
			GLuint _inputLayout;
			GLenum  _indexType;
			GLintptr _indexOffset;
			std::vector<math::float4> _clearColor;
			std::vector<math::float4> _viewports;
			std::vector<math::uint4> _scissors;

			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif