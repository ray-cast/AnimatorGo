#include "gl20_device_context.h"
#include "gl20_state.h"
#include "gl20_shader.h"
#include "gl20_texture.h"
#include "gl20_framebuffer.h"
#include "gl20_input_layout.h"
#include "gl20_sampler.h"
#include "gl20_descriptor_set.h"
#include "gl20_pipeline.h"
#include "gl20_graphics_data.h"
#include "gl20_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL20DeviceContext, GraphicsContext, "GL20DeviceContext")

		GL20DeviceContext::GL20DeviceContext() noexcept
			: _clearColor(0.0f, 0.0f, 0.0f, 0.0f)
			, _clearDepth(1.0f)
			, _clearStencil(0)
			, _framebuffer(nullptr)
			, _program(nullptr)
			, _pipeline(nullptr)
			, _descriptorSet(nullptr)
			, _indexType(GL_UNSIGNED_INT)
			, _indexOffset(0)
			, _state(nullptr)
			, _glcontext(nullptr)
			, _needUpdatePipeline(false)
			, _needUpdateDescriptor(false)
			, _needUpdateVertexBuffers(false)
			, _viewport(0, 0, 0, 0)
			, _scissor(0, 0, 0, 0)
		{
			_stateDefault = std::make_shared<GL20GraphicsState>();
			_stateDefault->setup(GraphicsStateDesc());
		}

		GL20DeviceContext::~GL20DeviceContext() noexcept
		{
			this->close();
		}

		bool
		GL20DeviceContext::setup(const GraphicsContextDesc& desc) noexcept
		{
			assert(desc.getSwapchain());
			assert(desc.getSwapchain()->isInstanceOf<OGLSwapchain>());

			_glcontext = desc.getSwapchain()->downcast_pointer<OGLSwapchain>();
			_glcontext->setActive(true);

			if (!_glcontext->getActive())
				return false;

			if (this->getDevice()->getDeviceDesc().isEnabledDebugControl())
				this->startDebugControl();

			if (!this->initStateSystem())
				return false;

			return true;
		}

		void
		GL20DeviceContext::close() noexcept
		{
			_framebuffer = nullptr;
			_program = nullptr;
			_pipeline = nullptr;
			_descriptorSet = nullptr;
			_state = nullptr;
			_glcontext = nullptr;
			_indexBuffer.reset();
			_vertexBuffers.clear();
		}

		void
		GL20DeviceContext::renderBegin() noexcept
		{
			assert(_glcontext);
			_glcontext->setActive(true);
		}

		void
		GL20DeviceContext::renderEnd() noexcept
		{
		}

		void
		GL20DeviceContext::setViewport(std::uint32_t i, const float4& view) noexcept
		{
			assert(_glcontext->getActive());

			if (_viewport != view)
			{
				if (i == 0)
					glViewport((GLint)view.left, (GLint)view.top, (GLsizei)view.width, (GLsizei)view.height);
				else
					this->getDevice()->downcast<OGLDevice>()->message("Cannot support glViewportIndexedf.");

				_viewport = view;
			}
		}

		const float4&
		GL20DeviceContext::getViewport(std::uint32_t i) const noexcept
		{
			return _viewport;
		}

		void
		GL20DeviceContext::setScissor(std::uint32_t i, const uint4& scissor) noexcept
		{
			assert(_glcontext->getActive());

			if (_scissor != scissor)
			{
				std::uint32_t height;
				if (_framebuffer)
					height = _framebuffer->getGraphicsFramebufferDesc().getHeight();
				else
					height = _glcontext->getGraphicsSwapchainDesc().getHeight();

				if (i == 0)
					glScissor(scissor.left, height - scissor.height - scissor.top, scissor.width, scissor.height);
				else
					this->getDevice()->downcast<OGLDevice>()->message("Cannot support glScissorIndexed.");

				_scissor = scissor;
			}
		}

		const uint4&
		GL20DeviceContext::getScissor(std::uint32_t i) const noexcept
		{
			return _scissor;
		}

		void
		GL20DeviceContext::setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontReadMask() != mask)
				{
					GLenum frontfunc = GL20Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
					if (frontfunc == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("Invalid compare function");
						return;
					}

					glStencilFuncSeparate(GL_FRONT, frontfunc, _stateCaptured.getStencilFrontRef(), mask);
					_stateCaptured.setStencilFrontReadMask(mask);
				}
			}
			if (face & GraphicsStencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackReadMask() != mask)
				{
					GLenum backfunc = GL20Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
					if (backfunc == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("Invalid compare function");
						return;
					}

					glStencilFuncSeparate(GL_BACK, backfunc, _stateCaptured.getStencilBackRef(), mask);
					_stateCaptured.setStencilBackReadMask(mask);
				}
			}
		}

		std::uint32_t
		GL20DeviceContext::getStencilCompareMask(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontReadMask();
			else
				return _stateCaptured.getStencilBackReadMask();
		}

		void
		GL20DeviceContext::setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontRef() != reference)
				{
					GLenum frontfunc = GL20Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
					if (frontfunc == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("Invalid compare function");
						return;
					}

					glStencilFuncSeparate(GL_FRONT, frontfunc, reference, _stateCaptured.getStencilFrontReadMask());
					_stateCaptured.setStencilFrontRef(reference);
				}
			}
			if (face & GraphicsStencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackRef() != reference)
				{
					GLenum backfunc = GL20Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
					if (backfunc == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<OGLDevice>()->message("Invalid compare function");
						return;
					}

					glStencilFuncSeparate(GL_BACK, backfunc, reference, _stateCaptured.getStencilBackReadMask());
					_stateCaptured.setStencilBackRef(reference);
				}
			}
		}

		std::uint32_t
		GL20DeviceContext::getStencilReference(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontRef();
			else
				return _stateCaptured.getStencilBackRef();
		}

		void
		GL20DeviceContext::setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
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
		GL20DeviceContext::getStencilWriteMask(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontWriteMask();
			else
				return _stateCaptured.getStencilBackWriteMask();
		}

		void
		GL20DeviceContext::setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept
		{
			assert(!pipeline || pipeline && pipeline->isInstanceOf<GL20Pipeline>());
			assert(_glcontext->getActive());

			if (pipeline)
			{
				auto glpipeline = pipeline->downcast_pointer<GL20Pipeline>();

				if (_pipeline != glpipeline)
				{
					auto& pipelineDesc = pipeline->getPipelineDesc();

					auto glstate = pipelineDesc.getGraphicsState()->downcast_pointer<GL20GraphicsState>();
					if (_state != glstate)
					{
						glstate->apply(_stateCaptured);
						_state = glstate;
					}

					auto glprogram = pipelineDesc.getGraphicsProgram()->downcast_pointer<GL20Program>();
					if (_program != glprogram)
					{
						_program = glprogram;
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
			else
			{
				if (_pipeline != pipeline)
				{
					if (_state != _stateDefault)
					{
						_stateDefault->apply(_stateCaptured);
						_state = _stateDefault;
					}

					if (_program)
					{
						glUseProgram(GL_NONE);
						_program = nullptr;
					}

					_pipeline = nullptr;
				}
			}
		}

		GraphicsPipelinePtr
		GL20DeviceContext::getRenderPipeline() const noexcept
		{
			return _pipeline;
		}

		void
		GL20DeviceContext::setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept
		{
			assert(descriptorSet);
			assert(descriptorSet->isInstanceOf<GL20DescriptorSet>());
			assert(_glcontext->getActive());

			_descriptorSet = descriptorSet->downcast_pointer<GL20DescriptorSet>();
			_needUpdateDescriptor = true;
		}

		GraphicsDescriptorSetPtr
		GL20DeviceContext::getDescriptorSet() const noexcept
		{
			return _descriptorSet;
		}

		void
		GL20DeviceContext::setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept
		{
			assert(data);
			assert(data->isInstanceOf<GL20GraphicsData>());
			assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::StorageVertexBuffer);
			assert(_vertexBuffers.size() > i);
			assert(_glcontext->getActive());

			auto vbo = data->downcast_pointer<GL20GraphicsData>();
			if (_vertexBuffers[i].vbo != vbo || _vertexBuffers[i].offset != offset)
			{
				_vertexBuffers[i].vbo = vbo;
				_vertexBuffers[i].offset = offset;
				_vertexBuffers[i].needUpdate = true;
				_needUpdateVertexBuffers = true;
			}
		}

		GraphicsDataPtr
		GL20DeviceContext::getVertexBufferData(std::uint32_t i) const noexcept
		{
			assert(_vertexBuffers.size() > i);
			return _vertexBuffers[i].vbo;
		}

		void
		GL20DeviceContext::setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, GraphicsIndexType indexType) noexcept
		{
			if (data)
			{
				assert(data->isInstanceOf<GL20GraphicsData>());
				assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::StorageIndexBuffer);
				assert(indexType == GraphicsIndexType::UInt16 || indexType == GraphicsIndexType::UInt32);
				assert(_glcontext->getActive());

				auto ibo = data->downcast_pointer<GL20GraphicsData>();
				if (_indexBuffer != ibo)
				{
					::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
					_indexBuffer = ibo;
				}

				_indexType = GL20Types::asIndexType(indexType);
				_indexOffset = (GLintptr)offset;

				if (_indexType == GL_INVALID_ENUM) this->getDevice()->downcast<OGLDevice>()->message("Invalid index type");
			}
			else
			{
				::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
				_indexBuffer = nullptr;
			}
		}

		GraphicsDataPtr
		GL20DeviceContext::getIndexBufferData() const noexcept
		{
			return _indexBuffer;
		}

		void
		GL20DeviceContext::generateMipmap(const GraphicsTexturePtr& texture) noexcept
		{
			assert(texture);
			assert(texture->isInstanceOf<GL20Texture>());

			auto gltexture = texture->downcast<GL20Texture>();
			auto textureID = gltexture->getInstanceID();
			auto textureTarget = gltexture->getTarget();

			GL_CHECK(glActiveTexture(GL_TEXTURE0));
			GL_CHECK(glBindTexture(textureTarget, textureID));
			GL_CHECK(glGenerateMipmap(textureTarget));
		}

		void
		GL20DeviceContext::setFramebuffer(const GraphicsFramebufferPtr& target) noexcept
		{
			assert(_glcontext->getActive());

			if (_framebuffer != target)
			{
				if (target)
				{
					auto framebuffer = target->downcast_pointer<GL20Framebuffer>();
					glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getInstanceID());

					auto& framebufferDesc = framebuffer->getGraphicsFramebufferDesc();

					this->setViewport(0, float4(0, 0, (float)framebufferDesc.getWidth(), (float)framebufferDesc.getHeight()));
					this->setScissor(0, _scissor);

					_framebuffer = framebuffer;
				}
				else
				{
					glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
					this->setRenderPipeline(nullptr);

					_framebuffer = nullptr;
				}
			}
		}

		GraphicsFramebufferPtr
		GL20DeviceContext::getFramebuffer() const noexcept
		{
			return _framebuffer;
		}

		void
		GL20DeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
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
		GL20DeviceContext::discardFramebuffer(std::uint32_t i) noexcept
		{
		}

		void
		GL20DeviceContext::blitFramebuffer(const GraphicsFramebufferPtr& src, const float4& v1, const GraphicsFramebufferPtr& dest, const float4& v2) noexcept
		{
			assert(src);
			assert(src->isInstanceOf<GL20Framebuffer>());
			assert(!dest || (dest && dest->isInstanceOf<GL20Framebuffer>()));
			assert(_glcontext->getActive());

			auto readFramebuffer = src->downcast<GL20Framebuffer>()->getInstanceID();
			auto drawFramebuffer = dest ? dest->downcast<GL20Framebuffer>()->getInstanceID() : GL_NONE;

			glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);

			glBlitFramebuffer((GLint)v1.left, (GLint)v1.top, (GLint)v1.width, (GLint)v1.height, (GLint)v2.left, (GLint)v2.top, (GLint)v2.width, (GLint)v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);

			_framebuffer = nullptr;
		}

		void
		GL20DeviceContext::readFramebuffer(std::uint32_t i, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
			GLenum internalFormat = GL20Types::asTextureFormat(texture->getTextureDesc().getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid texture format");
				return;
			}

			glReadBuffer(GL_COLOR_ATTACHMENT0 + i);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(texture->downcast<GL20Texture>()->getTarget(), texture->downcast<GL20Texture>()->getInstanceID());
			glCopyTexImage2D(texture->downcast<GL20Texture>()->getTarget(), miplevel, internalFormat, x, y, width, height, 0);
		}

		void
		GL20DeviceContext::readFramebufferToCube(std::uint32_t i, std::uint32_t face, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
			GLenum internalFormat = GL20Types::asTextureFormat(texture->getTextureDesc().getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid texture format");
				return;
			}

			glReadBuffer(GL_COLOR_ATTACHMENT0 + i);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(texture->downcast<GL20Texture>()->getTarget(), texture->downcast<GL20Texture>()->getInstanceID());
			glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, miplevel, internalFormat, x, y, width, height, 0);
		}

		void
		GL20DeviceContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(startInstances == 0);

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
				GLenum drawType = GL20Types::asVertexType(primitiveType);
				GL_CHECK(glDrawArrays(drawType, startVertice, numVertices));
			}
		}

		void
		GL20DeviceContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(_indexBuffer);
			assert(_indexType == GL_UNSIGNED_INT || _indexType == GL_UNSIGNED_SHORT);
			assert(numInstances <= 1 && startInstances == 0);

			_pipeline->bindVertexBuffers(_vertexBuffers, _needUpdatePipeline, startVertice);

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
				else if (_indexType == GL_UNSIGNED_SHORT)
					offsetIndices += _indexOffset + sizeof(std::uint16_t) * startIndice;
				else
					offsetIndices += _indexOffset + sizeof(std::uint8_t) * startIndice;

				GLenum drawType = GL20Types::asVertexType(_stateCaptured.getPrimitiveType());
				glDrawElements(drawType, numIndices, _indexType, offsetIndices);
			}
		}

		void
		GL20DeviceContext::drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
		}

		void
		GL20DeviceContext::drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
		}

		void
		GL20DeviceContext::present() noexcept
		{
			assert(_glcontext->getActive());
			_glcontext->present();
		}

		void
		GL20DeviceContext::startDebugControl() noexcept
		{
		}

		void
		GL20DeviceContext::stopDebugControl() noexcept
		{
		}

		bool
		GL20DeviceContext::initStateSystem() noexcept
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
		GL20DeviceContext::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL20DeviceContext::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}