#ifndef OCTOON_GL33_DEVICE_CONTEXT_H_
#define OCTOON_GL33_DEVICE_CONTEXT_H_

#include "gl33_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL33DeviceContext final : public GraphicsContext
		{
			OctoonDeclareSubClass(GL33DeviceContext, GraphicsContext)
		public:
			GL33DeviceContext() noexcept;
			~GL33DeviceContext() noexcept;

			bool setup(const GraphicsContextDesc& desc) noexcept;
			void close() noexcept;

			void renderBegin() noexcept;
			void renderEnd() noexcept;

			void setViewport(std::uint32_t i, const float4& viewport) noexcept;
			const float4& getViewport(std::uint32_t i) const noexcept;

			void setScissor(std::uint32_t i, const uint4& scissor) noexcept;
			const uint4& getScissor(std::uint32_t i) const noexcept;

			void setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept;
			std::uint32_t getStencilCompareMask(GraphicsStencilFaceFlags face) noexcept;

			void setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept;
			std::uint32_t getStencilReference(GraphicsStencilFaceFlags face) noexcept;

			void setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept;
			std::uint32_t getStencilWriteMask(GraphicsStencilFaceFlags face) noexcept;

			void setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept;
			GraphicsPipelinePtr getRenderPipeline() const noexcept;

			void setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept;
			GraphicsDescriptorSetPtr getDescriptorSet() const noexcept;

			void setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept;
			GraphicsDataPtr getVertexBufferData(std::uint32_t i) const noexcept;

			void setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, GraphicsIndexType indexType) noexcept;
			GraphicsDataPtr getIndexBufferData() const noexcept;

			void generateMipmap(const GraphicsTexturePtr& texture) noexcept;

			void setFramebuffer(const GraphicsFramebufferPtr& target) noexcept;
			void clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept;
			void discardFramebuffer(std::uint32_t i) noexcept;
			void blitFramebuffer(const GraphicsFramebufferPtr& src, const float4& v1, const GraphicsFramebufferPtr& dest, const float4& v2) noexcept;
			void readFramebuffer(std::uint32_t i, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept;
			void readFramebufferToCube(std::uint32_t i, std::uint32_t face, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept;
			GraphicsFramebufferPtr getFramebuffer() const noexcept;

			void draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept override;
			void drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept;
			void drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept;
			void drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept;

			void present() noexcept;

			void enableDebugControl(bool enable) noexcept;
			void startDebugControl() noexcept;
			void stopDebugControl() noexcept;

		private:
			bool checkSupport() noexcept;
			bool initStateSystem() noexcept;

			static void GLAPIENTRY debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GL33DeviceContext(const GL33DeviceContext&) noexcept = delete;
			GL33DeviceContext& operator=(const GL33DeviceContext&) noexcept = delete;

		private:
			float4 _clearColor;
			GLfloat _clearDepth;
			GLint   _clearStencil;
			GLuint _globalVao;

			std::vector<float4> _viewports;
			std::vector<uint4> _scissors;

			GLenum  _indexType;
			GLintptr _indexOffset;

			GL33PipelinePtr _pipeline;
			GL33DescriptorSetPtr _descriptorSet;
			GraphicsFramebufferPtr _framebuffer;
			GL33VertexBuffers _vertexBuffers;
			GL33GraphicsDataPtr _indexBuffer;
			GL33ProgramPtr _program;
			GL33SwapchainPtr _glcontext;
			GL33GraphicsStatePtr _state;
			GL33GraphicsStatePtr _stateDefault;

			GraphicsStateDesc _stateCaptured;

			bool _needUpdatePipeline;
			bool _needUpdateDescriptor;
			bool _needUpdateVertexBuffers;
			bool _needEnableDebugControl;
			bool _needDisableDebugControl;

			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif