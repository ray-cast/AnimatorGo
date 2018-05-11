#include "egl2_device_context.h"
#include "egl2_state.h"
#include "egl2_shader.h"
#include "egl2_texture.h"
#include "egl2_input_layout.h"
#include "egl2_sampler.h"
#include "egl2_framebuffer.h"
#include "egl2_graphics_data.h"
#include "egl2_swapchain.h"
#include "egl2_pipeline.h"
#include "egl2_descriptor.h"

#include <iostream>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(EGL2DeviceContext, GraphicsContext, "EGL2DeviceContext")

		EGL2DeviceContext::EGL2DeviceContext() noexcept
			: _clearDepth(1.0)
			, _clearStencil(0)
			, _clearColor(0.0, 0.0, 0.0, 0.0)
			, _viewport(0, 0, 0, 0)
			, _scissor(0, 0, 0, 0)
			, _inputLayout(GL_NONE)
			, _indexType(GL_UNSIGNED_INT)
			, _indexOffset(0)
			, _needUpdatePipeline(false)
			, _needUpdateDescriptor(false)
			, _needUpdateVertexBuffers(false)
			, _needEnableDebugControl(false)
			, _needDisableDebugControl(false)
		{
		}

		EGL2DeviceContext::~EGL2DeviceContext() noexcept
		{
			this->close();
		}

		bool
		EGL2DeviceContext::setup(const GraphicsContextDesc& desc) noexcept
		{
			assert(desc.getSwapchain());
			assert(desc.getSwapchain()->isInstanceOf<EGL2Swapchain>());

			_glcontext = desc.getSwapchain()->downcast_pointer<EGL2Swapchain>();
			_glcontext->setActive(true);

			if (!_glcontext->getActive())
				return false;

			if (GraphicsSystem::instance()->enableDebugControl())
				this->startDebugControl();

			if (!this->initStateSystem())
				return false;

			return true;
		}

		void
		EGL2DeviceContext::close() noexcept
		{
			_framebuffer.reset();
			_program.reset();
			_pipeline.reset();
			_descriptorSet.reset();
			_state.reset();
			_indexBuffer.reset();
			_glcontext.reset();
			_vertexBuffers.clear();
		}

		void
		EGL2DeviceContext::renderBegin() noexcept
		{
			assert(_glcontext);
			_glcontext->setActive(true);

			if (_needEnableDebugControl)
			{
				this->startDebugControl();
				_needEnableDebugControl = false;
			}

			if (_needDisableDebugControl)
			{
				this->stopDebugControl();
				_needDisableDebugControl = false;
			}
		}

		void
		EGL2DeviceContext::renderEnd() noexcept
		{
		}

		void
		EGL2DeviceContext::setViewport(std::uint32_t i, const float4& view) noexcept
		{
			if (_viewport.left != view.left ||
				_viewport.top != view.top ||
				_viewport.width != view.width ||
				_viewport.height != view.height)
			{
				GL_CHECK(glViewport(view.left, view.top, view.width, view.height));
				_viewport = view;
			}
		}

		const float4&
		EGL2DeviceContext::getViewport(std::uint32_t i) const noexcept
		{
			return _viewport;
		}

		void
		EGL2DeviceContext::setScissor(std::uint32_t i, const uint4& scissor) noexcept
		{
			if (_scissor != scissor)
			{
				std::uint32_t height;
				if (_framebuffer)
					height = _framebuffer->getGraphicsFramebufferDesc().getHeight();
				else
					height = _glcontext->getGraphicsSwapchainDesc().getHeight();

				glScissor(scissor.left, height - scissor.top, scissor.width, scissor.height);
				_scissor = scissor;
			}
		}

		const uint4&
		EGL2DeviceContext::getScissor(std::uint32_t i) const noexcept
		{
			return _scissor;
		}

		void
		EGL2DeviceContext::setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontReadMask() != mask)
				{
					GLenum frontfunc = EGL2Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
					GL_CHECK(glStencilFuncSeparate(GL_FRONT, frontfunc, _stateCaptured.getStencilFrontRef(), mask));
					_stateCaptured.setStencilFrontReadMask(mask);
				}
			}
			if (face & GraphicsStencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackReadMask() != mask)
				{
					GLenum backfunc = EGL2Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
					GL_CHECK(glStencilFuncSeparate(GL_BACK, backfunc, _stateCaptured.getStencilBackRef(), mask));
					_stateCaptured.setStencilBackReadMask(mask);
				}
			}
		}

		std::uint32_t
		EGL2DeviceContext::getStencilCompareMask(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontReadMask();
			else
				return _stateCaptured.getStencilBackReadMask();
		}

		void
		EGL2DeviceContext::setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontRef() != reference)
				{
					GLenum frontfunc = EGL2Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
					GL_CHECK(glStencilFuncSeparate(GL_FRONT, frontfunc, reference, _stateCaptured.getStencilFrontReadMask()));
					_stateCaptured.setStencilFrontRef(reference);
				}
			}
			if (face & GraphicsStencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackRef() != reference)
				{
					GLenum backfunc = EGL2Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
					GL_CHECK(glStencilFuncSeparate(GL_BACK, backfunc, reference, _stateCaptured.getStencilBackReadMask()));
					_stateCaptured.setStencilBackRef(reference);
				}
			}
		}

		std::uint32_t
		EGL2DeviceContext::getStencilReference(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontRef();
			else
				return _stateCaptured.getStencilBackRef();
		}

		void
		EGL2DeviceContext::setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontWriteMask() != mask)
				{
					GL_CHECK(glStencilMaskSeparate(GL_FRONT, mask));
					_stateCaptured.setStencilFrontWriteMask(mask);
				}
			}
			if (face & GraphicsStencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackWriteMask() != mask)
				{
					GL_CHECK(glStencilMaskSeparate(GL_BACK, mask));
					_stateCaptured.setStencilBackWriteMask(mask);
				}
			}
		}

		std::uint32_t
		EGL2DeviceContext::getStencilWriteMask(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontWriteMask();
			else
				return _stateCaptured.getStencilBackWriteMask();
		}

		void
		EGL2DeviceContext::setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept
		{
			assert(pipeline);
			assert(pipeline->isInstanceOf<EGL2Pipeline>());
			assert(_glcontext->getActive());

			auto glpipeline = pipeline->downcast_pointer<EGL2Pipeline>();
			if (_pipeline != glpipeline)
			{
				auto& pipelineDesc = pipeline->getGraphicsPipelineDesc();

				auto glstate = pipelineDesc.getGraphicsState()->downcast_pointer<EGL2GraphicsState>();
				if (_state != glstate)
				{
					glstate->apply(_stateCaptured);
					_state = glstate;
				}

				auto glshader = pipelineDesc.getGraphicsProgram()->downcast_pointer<EGL2Program>();
				if (_program != glshader)
				{
					_program = glshader;
					_program->apply();
				}

				if (_pipeline != glpipeline)
				{
					_pipeline = glpipeline;
					_pipeline->apply();
					_needUpdatePipeline = true;
				}
			}
		}

		GraphicsPipelinePtr
		EGL2DeviceContext::getRenderPipeline() const noexcept
		{
			return _pipeline;
		}

		void
		EGL2DeviceContext::setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept
		{
			assert(descriptorSet);
			assert(descriptorSet->isInstanceOf<EGL2DescriptorSet>());
			assert(_glcontext->getActive());

			_descriptorSet = descriptorSet->downcast_pointer<EGL2DescriptorSet>();
			_needUpdateDescriptor = true;
		}

		GraphicsDescriptorSetPtr
		EGL2DeviceContext::getDescriptorSet() const noexcept
		{
			return _descriptorSet;
		}

		void
		EGL2DeviceContext::setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept
		{
			assert(data);
			assert(data->isInstanceOf<EGL2GraphicsData>());
			assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::StorageVertexBuffer);
			assert(_vertexBuffers.size() > i);
			assert(_glcontext->getActive());

			auto vbo = data->downcast_pointer<EGL2GraphicsData>();
			if (_vertexBuffers[i].vbo != vbo || _vertexBuffers[i].offset != offset)
			{
				_vertexBuffers[i].vbo = vbo;
				_vertexBuffers[i].offset = offset;
				_vertexBuffers[i].needUpdate = true;
				_needUpdateVertexBuffers = true;
			}
		}

		GraphicsDataPtr
		EGL2DeviceContext::getVertexBufferData(std::uint32_t i) const noexcept
		{
			return _vertexBuffers[i].vbo;
		}

		void
		EGL2DeviceContext::setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, GraphicsIndexType indexType) noexcept
		{
			assert(data);
			assert(data->isInstanceOf<EGL2GraphicsData>());
			assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::StorageIndexBuffer);
			assert(indexType == GraphicsIndexType::UInt16 || indexType == GraphicsIndexType::UInt32);
			assert(_glcontext->getActive());

			auto ibo = data->downcast_pointer<EGL2GraphicsData>();
			if (_indexBuffer != ibo)
			{
				::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
				_indexBuffer = ibo;
			}

			_indexType = EGL2Types::asIndexType(indexType);
			_indexOffset = offset;
		}

		GraphicsDataPtr
		EGL2DeviceContext::getIndexBufferData() const noexcept
		{
			return _indexBuffer;
		}

		void
		EGL2DeviceContext::generateMipmap(const GraphicsTexturePtr& texture) noexcept
		{
			assert(texture);
			assert(texture->isInstanceOf<EGL2Texture>());

			auto gltexture = texture->downcast<EGL2Texture>();
			auto textureID = gltexture->getInstanceID();
			auto textureTarget = gltexture->getTarget();

			GL_CHECK(glActiveTexture(GL_TEXTURE0));
			GL_CHECK(glBindTexture(textureTarget, textureID));
			GL_CHECK(glGenerateMipmap(textureTarget));
		}

		void
		EGL2DeviceContext::setFramebuffer(const GraphicsFramebufferPtr& target) noexcept
		{
			assert(_glcontext->getActive());

			if (target)
			{
				auto framebuffer = target->downcast_pointer<EGL2Framebuffer>();
				if (_framebuffer != framebuffer)
				{
					_framebuffer = framebuffer;
					glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->getInstanceID());

					auto& framebufferDesc = _framebuffer->getGraphicsFramebufferDesc();
					this->setViewport(0, float4(0, 0, framebufferDesc.getWidth(), framebufferDesc.getHeight()));

					glScissor(_scissor.left, framebufferDesc.getHeight() - _scissor.height - _scissor.top, _scissor.width, _scissor.height);
				}
			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
				_framebuffer = nullptr;
			}
		}

		GraphicsFramebufferPtr
		EGL2DeviceContext::getFramebuffer() const noexcept
		{
			return _framebuffer;
		}

		void
		EGL2DeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
		{
			assert(i == 0);

			GLbitfield mode = 0;

			if (flags & GraphicsClearFlagBits::ColorBit)
			{
				mode |= GL_COLOR_BUFFER_BIT;

				if (_clearColor != color)
				{
					GL_CHECK(glClearColor(color.x, color.y, color.z, color.w));
					_clearColor = color;
				}
			}

			if (flags & GraphicsClearFlagBits::DepthBit)
			{
				mode |= GL_DEPTH_BUFFER_BIT;

				if (_clearDepth != depth)
				{
					GL_CHECK(glClearDepthf(depth));
					_clearDepth = depth;
				}
			}

			if (flags & GraphicsClearFlagBits::StencilBit)
			{
				mode |= GL_STENCIL_BUFFER_BIT;

				if (_clearStencil != stencil)
				{
					GL_CHECK(glClearStencil(stencil));
					_clearStencil = stencil;
				}
			}

			if (_stateCaptured.getScissorTestEnable())
			{
				if (_framebuffer)
					glScissor(0, 0, _framebuffer->getGraphicsFramebufferDesc().getWidth(), _framebuffer->getGraphicsFramebufferDesc().getHeight());
				else
					glScissor(0, 0, _glcontext->getGraphicsSwapchainDesc().getWidth(), _glcontext->getGraphicsSwapchainDesc().getHeight());
			}

			if (mode != 0)
			{
				auto depthWriteEnable = _stateCaptured.getDepthWriteEnable();
				if (!depthWriteEnable && flags & GraphicsClearFlagBits::DepthBit)
				{
					GL_CHECK(glDepthMask(GL_TRUE));
				}

				auto colorWriteFlags = _stateCaptured.getColorBlends()[0].getColorWriteMask();
				if (colorWriteFlags != GraphicsColorMaskFlagBits::RGBABit)
				{
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				}

				GL_CHECK(glClear(mode));

				if (colorWriteFlags != GraphicsColorMaskFlagBits::RGBABit)
				{
					GLboolean r = colorWriteFlags & GraphicsColorMaskFlagBits::RedBit ? GL_TRUE : GL_FALSE;
					GLboolean g = colorWriteFlags & GraphicsColorMaskFlagBits::GreendBit ? GL_TRUE : GL_FALSE;
					GLboolean b = colorWriteFlags & GraphicsColorMaskFlagBits::BlurBit ? GL_TRUE : GL_FALSE;
					GLboolean a = colorWriteFlags & GraphicsColorMaskFlagBits::AlphaBit ? GL_TRUE : GL_FALSE;
					glColorMask(r, g, b, a);
				}

				if (!depthWriteEnable && flags & GraphicsClearFlagBits::DepthBit)
				{
					GL_CHECK(glDepthMask(GL_FALSE));
				}
			}

			if (_stateCaptured.getScissorTestEnable())
			{
				glScissor(_scissor.left, _scissor.top, _scissor.width, _scissor.height);
			}
		}

		void
		EGL2DeviceContext::discardFramebuffer(std::uint32_t i) noexcept
		{
		}

		void
		EGL2DeviceContext::blitFramebuffer(const GraphicsFramebufferPtr& src, const float4& v1, const GraphicsFramebufferPtr& dest, const float4& v2) noexcept
		{
		}

		void
		EGL2DeviceContext::readFramebuffer(std::uint32_t i, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
		}

		void
		EGL2DeviceContext::readFramebufferToCube(std::uint32_t i, std::uint32_t face, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
		}

		void
		EGL2DeviceContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(numInstances <= 1 && startInstances == 0);

			if (_needUpdatePipeline || _needUpdateVertexBuffers)
			{
				_pipeline->bindVertexBuffers(_vertexBuffers, _needUpdatePipeline);
				_needUpdatePipeline = false;
				_needUpdateVertexBuffers = false;
			}

			if (_needUpdateDescriptor)
			{
				_descriptorSet->apply(*_program);
				_needUpdateDescriptor = false;
			}

			auto primitiveType = _stateCaptured.getPrimitiveType();
			if (numVertices > 0)
			{
				GLenum drawType = EGL2Types::asVertexType(primitiveType);
				GL_CHECK(glDrawArrays(drawType, 0, numVertices));
			}
		}

		void
		EGL2DeviceContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(_indexBuffer);
			assert(_indexType == GL_UNSIGNED_INT || _indexType == GL_UNSIGNED_SHORT);
			assert(numInstances <= 1 && startInstances == 0);

			if (_needUpdatePipeline || _needUpdateVertexBuffers)
			{
				_pipeline->bindVertexBuffers(_vertexBuffers, _needUpdatePipeline);
				_needUpdatePipeline = false;
				_needUpdateVertexBuffers = false;
			}

			if (_needUpdateDescriptor)
			{
				_descriptorSet->apply(*_program);
				_needUpdateDescriptor = false;
			}

			if (numIndices > 0)
			{
				GLbyte* offsetIndices = nullptr;
				if (_indexType == GL_UNSIGNED_INT)
					offsetIndices += _indexOffset + sizeof(std::uint32_t) * startIndice;
				else
					offsetIndices += _indexOffset + sizeof(std::uint16_t) * startIndice;

				GLenum drawType = EGL2Types::asVertexType(_stateCaptured.getPrimitiveType());
				glDrawElements(drawType, numIndices, _indexType, offsetIndices);
			}
		}

		void
		EGL2DeviceContext::drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
		}

		void
		EGL2DeviceContext::drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
		}

		void
		EGL2DeviceContext::present() noexcept
		{
			assert(_glcontext);
			_glcontext->present();
		}

		void
		EGL2DeviceContext::enableDebugControl(bool enable) noexcept
		{
			if (enable)
			{
				_needEnableDebugControl = true;
				_needDisableDebugControl = false;
			}
			else
			{
				_needEnableDebugControl = false;
				_needDisableDebugControl = true;
			}
		}

		void
		EGL2DeviceContext::startDebugControl() noexcept
		{
		}

		void
		EGL2DeviceContext::stopDebugControl() noexcept
		{
		}

		bool
		EGL2DeviceContext::initStateSystem() noexcept
		{
			GL_CHECK(glClearDepthf(1.0));
			GL_CHECK(glClearColor(0.0, 0.0, 0.0, 0.0));
			GL_CHECK(glClearStencil(0));

			GL_CHECK(glViewport(0, 0, 0, 0));

			GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

			GL_CHECK(glEnable(GL_DEPTH_TEST));
			GL_CHECK(glDepthMask(GL_TRUE));
			GL_CHECK(glDepthFunc(GL_LEQUAL));

			GL_CHECK(glEnable(GL_CULL_FACE));
			GL_CHECK(glCullFace(GL_BACK));
			GL_CHECK(glFrontFace(GL_CW));
			GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

			GL_CHECK(glDisable(GL_STENCIL_TEST));
			GL_CHECK(glStencilMask(0xFFFFFFFF));
			GL_CHECK(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));
			GL_CHECK(glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF));

			GL_CHECK(glDisable(GL_BLEND));
			GL_CHECK(glBlendEquation(GL_FUNC_ADD));
			GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

			auto& deviceProperties = this->getDevice()->getDeviceProperty().getDeviceProperties();
			_vertexBuffers.resize(deviceProperties.maxVertexInputBindings);

			GraphicsColorBlends blends(deviceProperties.maxFramebufferColorAttachments);
			_stateCaptured.setColorBlends(blends);

			return true;
		}

		void
		EGL2DeviceContext::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2DeviceContext::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}