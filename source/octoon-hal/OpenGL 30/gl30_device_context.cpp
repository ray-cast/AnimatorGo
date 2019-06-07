#include "gl30_device_context.h"
#include "gl30_state.h"
#include "gl30_shader.h"
#include "gl30_texture.h"
#include "gl30_framebuffer.h"
#include "gl30_input_layout.h"
#include "gl30_sampler.h"
#include "gl30_descriptor_set.h"
#include "gl30_pipeline.h"
#include "gl30_graphics_data.h"
#include "gl30_device.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL30DeviceContext, GraphicsContext, "GL30DeviceContext")

		GL30DeviceContext::GL30DeviceContext() noexcept
			: _clearColor(0.0f, 0.0f, 0.0f, 0.0f)
			, _clearDepth(1.0f)
			, _clearStencil(0)
			, _inputLayout(GL_NONE)
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
		{
			_stateDefault = std::make_shared<GL30GraphicsState>();
			_stateDefault->setup(GraphicsStateDesc());
		}

		GL30DeviceContext::~GL30DeviceContext() noexcept
		{
			this->close();
		}

		bool
		GL30DeviceContext::setup(const GraphicsContextDesc& desc) noexcept
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
		GL30DeviceContext::close() noexcept
		{
			_framebuffer = nullptr;
			_program = nullptr;
			_pipeline = nullptr;
			_descriptorSet = nullptr;
			_state = nullptr;
			_glcontext = nullptr;
			_indexBuffer.reset();
			_vertexBuffers.clear();

			if (_inputLayout)
			{
				glDeleteVertexArrays(1, &_inputLayout);
				_inputLayout = GL_NONE;
			}
		}

		void
		GL30DeviceContext::renderBegin() noexcept
		{
			assert(_glcontext);
			_glcontext->setActive(true);
		}

		void
		GL30DeviceContext::renderEnd() noexcept
		{
		}

		void
		GL30DeviceContext::setViewport(std::uint32_t i, const float4& view) noexcept
		{
			assert(_glcontext->getActive());

			if (_viewports[i] != view)
			{
				if (i == 0)
					glViewport((GLint)view.left, (GLint)view.top, (GLsizei)view.width, (GLsizei)view.height);
				else
					this->getDevice()->downcast<OGLDevice>()->message("Cannot support glViewportIndexedf.");

				_viewports[i] = view;
			}
		}

		const float4&
		GL30DeviceContext::getViewport(std::uint32_t i) const noexcept
		{
			return _viewports[i];
		}

		void
		GL30DeviceContext::setScissor(std::uint32_t i, const uint4& scissor) noexcept
		{
			assert(_glcontext->getActive());

			if (_scissors[i] != scissor)
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

				_scissors[i] = scissor;
			}
		}

		const uint4&
		GL30DeviceContext::getScissor(std::uint32_t i) const noexcept
		{
			return _scissors[i];
		}
		void
		GL30DeviceContext::setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontReadMask() != mask)
				{
					GLenum frontfunc = GL30Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
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
					GLenum backfunc = GL30Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
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
		GL30DeviceContext::getStencilCompareMask(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontReadMask();
			else
				return _stateCaptured.getStencilBackReadMask();
		}

		void
		GL30DeviceContext::setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontRef() != reference)
				{
					GLenum frontfunc = GL30Types::asCompareFunction(_stateCaptured.getStencilFrontFunc());
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
					GLenum backfunc = GL30Types::asCompareFunction(_stateCaptured.getStencilBackFunc());
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
		GL30DeviceContext::getStencilReference(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontRef();
			else
				return _stateCaptured.getStencilBackRef();
		}

		void
		GL30DeviceContext::setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontWriteMask() != mask)
				{
					glStencilMaskSeparate(GL_FRONT, mask);
					_stateCaptured.setStencilFrontWriteMask(mask);
				}
			}
			if (face & GraphicsStencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackWriteMask() != mask)
				{
					glStencilMaskSeparate(GL_BACK, mask);
					_stateCaptured.setStencilBackWriteMask(mask);
				}
			}
		}

		std::uint32_t
		GL30DeviceContext::getStencilWriteMask(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontWriteMask();
			else
				return _stateCaptured.getStencilBackWriteMask();
		}

		void
		GL30DeviceContext::setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept
		{
			assert(!pipeline || pipeline && pipeline->isInstanceOf<GL30Pipeline>());
			assert(_glcontext->getActive());

			if (pipeline)
			{
				auto glpipeline = pipeline->downcast_pointer<GL30Pipeline>();

				if (_pipeline != glpipeline)
				{
					auto& pipelineDesc = pipeline->getPipelineDesc();

					auto glstate = pipelineDesc.getGraphicsState()->downcast_pointer<GL30GraphicsState>();
					if (_state != glstate)
					{
						glstate->apply(_stateCaptured);
						_state = glstate;
					}

					auto glprogram = pipelineDesc.getGraphicsProgram()->downcast_pointer<GL30Program>();
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
		GL30DeviceContext::getRenderPipeline() const noexcept
		{
			return _pipeline;
		}

		void
		GL30DeviceContext::setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept
		{
			assert(descriptorSet);
			assert(descriptorSet->isInstanceOf<GL30DescriptorSet>());
			assert(_glcontext->getActive());

			_descriptorSet = descriptorSet->downcast_pointer<GL30DescriptorSet>();
			_needUpdateDescriptor = true;
		}

		GraphicsDescriptorSetPtr
		GL30DeviceContext::getDescriptorSet() const noexcept
		{
			return _descriptorSet;
		}

		void
		GL30DeviceContext::setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept
		{
			assert(data);
			assert(data->isInstanceOf<GL30GraphicsData>());
			assert(data->getDataDesc().getType() == GraphicsDataType::StorageVertexBuffer);
			assert(_vertexBuffers.size() > i);
			assert(_glcontext->getActive());

			auto vbo = data->downcast_pointer<GL30GraphicsData>();
			if (_vertexBuffers[i].vbo != vbo || _vertexBuffers[i].offset != offset)
			{
				_vertexBuffers[i].vbo = vbo;
				_vertexBuffers[i].offset = offset;
				_vertexBuffers[i].needUpdate = true;
				_needUpdateVertexBuffers = true;
			}
		}

		GraphicsDataPtr
		GL30DeviceContext::getVertexBufferData(std::uint32_t i) const noexcept
		{
			assert(_vertexBuffers.size() > i);
			return _vertexBuffers[i].vbo;
		}

		void
		GL30DeviceContext::setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, GraphicsIndexType indexType) noexcept
		{
			if (data)
			{
				assert(data->isInstanceOf<GL30GraphicsData>());
				assert(data->getDataDesc().getType() == GraphicsDataType::StorageIndexBuffer);
				assert(indexType == GraphicsIndexType::UInt16 || indexType == GraphicsIndexType::UInt32);
				assert(_glcontext->getActive());

				auto ibo = data->downcast_pointer<GL30GraphicsData>();
				if (_indexBuffer != ibo)
				{
					::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
					_indexBuffer = ibo;
				}

				_indexType = GL30Types::asIndexType(indexType);
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
		GL30DeviceContext::getIndexBufferData() const noexcept
		{
			return _indexBuffer;
		}

		void
		GL30DeviceContext::generateMipmap(const GraphicsTexturePtr& texture) noexcept
		{
			assert(texture);
			assert(texture->isInstanceOf<GL30Texture>());

			auto gltexture = texture->downcast<GL30Texture>();
			auto textureID = gltexture->getInstanceID();
			auto textureTarget = gltexture->getTarget();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(textureTarget, textureID);
			glGenerateMipmap(textureTarget);
		}

		void
		GL30DeviceContext::setFramebuffer(const GraphicsFramebufferPtr& target) noexcept
		{
			assert(_glcontext->getActive());

			if (_framebuffer != target)
			{
				if (target)
				{
					auto framebuffer = target->downcast_pointer<GL30Framebuffer>();
					glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->getInstanceID());

					auto& framebufferDesc = framebuffer->getGraphicsFramebufferDesc();
					auto& colorAttachment = framebufferDesc.getColorAttachments();

					std::uint32_t viewportCount = std::max<std::uint32_t>(1, static_cast<std::uint32_t>(colorAttachment.size()));
					for (std::uint32_t i = 0; i < viewportCount; i++)
					{
						this->setViewport(i, float4(0, 0, (float)framebufferDesc.getWidth(), (float)framebufferDesc.getHeight()));
						this->setScissor(i, _scissors[i]);
					}

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

		void
		GL30DeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
		{
			assert(i == 0);
			assert(_glcontext->getActive());

			GLint buffer = i;

			if (_stateCaptured.getScissorTestEnable())
			{
				if (_framebuffer)
					glScissor(0, 0, _framebuffer->getGraphicsFramebufferDesc().getWidth(), _framebuffer->getGraphicsFramebufferDesc().getHeight());
				else
					glScissor(0, 0, _glcontext->getGraphicsSwapchainDesc().getWidth(), _glcontext->getGraphicsSwapchainDesc().getHeight());
			}

			if (flags & GraphicsClearFlagBits::DepthBit)
			{
				auto depthWriteEnable = _stateCaptured.getDepthWriteEnable();
				if (!depthWriteEnable)
				{
					glDepthMask(GL_TRUE);
				}

				if (flags & GraphicsClearFlagBits::StencilBit)
				{
					GLint s = stencil;
					GLfloat f = depth;
					glClearBufferfi(GL_DEPTH_STENCIL, buffer, f, s);
				}
				else
				{
					GLfloat f = depth;
					glClearBufferfv(GL_DEPTH, buffer, &f);
				}

				if (!depthWriteEnable)
				{
					glDepthMask(GL_FALSE);
				}
			}
			else if (flags & GraphicsClearFlagBits::StencilBit)
			{
				GLint s = stencil;
				glClearBufferiv(GL_STENCIL, buffer, &s);
			}

			if (flags & GraphicsClearFlagBits::ColorBit)
			{
				auto colorWriteFlags = _stateCaptured.getColorBlends()[buffer].getColorWriteMask();
				if (colorWriteFlags != GraphicsColorMaskFlagBits::RGBABit)
				{
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				}

				glClearBufferfv(GL_COLOR, buffer, color.ptr());

				if (colorWriteFlags != GraphicsColorMaskFlagBits::RGBABit)
				{
					GLboolean r = colorWriteFlags & GraphicsColorMaskFlagBits::RedBit ? GL_TRUE : GL_FALSE;
					GLboolean g = colorWriteFlags & GraphicsColorMaskFlagBits::GreendBit ? GL_TRUE : GL_FALSE;
					GLboolean b = colorWriteFlags & GraphicsColorMaskFlagBits::BlurBit ? GL_TRUE : GL_FALSE;
					GLboolean a = colorWriteFlags & GraphicsColorMaskFlagBits::AlphaBit ? GL_TRUE : GL_FALSE;
					glColorMask(r, g, b, a);
				}
			}

			if (_stateCaptured.getScissorTestEnable())
			{
				if (_framebuffer)
				{
					auto w = _framebuffer->getGraphicsFramebufferDesc().getWidth();
					auto h = _framebuffer->getGraphicsFramebufferDesc().getWidth();
					glScissor(0, 0, w, h);
				}
				else
				{
					glScissor(_scissors[0].left, _scissors[0].top, _scissors[0].width, _scissors[0].height);
				}
			}
		}

		void
		GL30DeviceContext::blitFramebuffer(const GraphicsFramebufferPtr& src, const float4& v1, const GraphicsFramebufferPtr& dest, const float4& v2) noexcept
		{
			assert(src);
			assert(src->isInstanceOf<GL30Framebuffer>());
			assert(!dest || (dest && dest->isInstanceOf<GL30Framebuffer>()));
			assert(_glcontext->getActive());

			auto readFramebuffer = src->downcast<GL30Framebuffer>()->getInstanceID();
			auto drawFramebuffer = dest ? dest->downcast<GL30Framebuffer>()->getInstanceID() : GL_NONE;

			glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);

			glBlitFramebuffer((GLint)v1.left, (GLint)v1.top, (GLint)v1.width, (GLint)v1.height, (GLint)v2.left, (GLint)v2.top, (GLint)v2.width, (GLint)v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);

			_framebuffer = nullptr;
		}

		void
		GL30DeviceContext::discardFramebuffer(std::uint32_t i) noexcept
		{
			assert(_framebuffer);
			assert(_glcontext->getActive());

			const auto& layoutDesc = _framebuffer->getGraphicsFramebufferDesc().getFramebufferLayout()->getFramebufferLayoutDesc();
			if (layoutDesc.getComponents().size() > i)
			{
				auto& attachment = layoutDesc.getComponents().at(i);
				switch (attachment.getAttachType())
				{
				case GraphicsImageLayout::ColorAttachmentOptimal:
				{
					GLenum attachments = GL_COLOR_ATTACHMENT0 + i;
					glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
				}
				break;
				case GraphicsImageLayout::DepthStencilReadOnlyOptimal:
				case GraphicsImageLayout::DepthStencilAttachmentOptimal:
				{
					auto format = attachment.getAttachFormat();
					if (format == GraphicsFormat::S8UInt)
					{
						GLenum attachments = GL_STENCIL_ATTACHMENT;
						glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
					}
					else if (format == GraphicsFormat::D16UNorm || format == GraphicsFormat::X8_D24UNormPack32 || format == GraphicsFormat::D32_SFLOAT)
					{
						GLenum attachments = GL_DEPTH_ATTACHMENT;
						glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
					}
					else
					{
						GLenum attachments = GL_DEPTH_STENCIL_ATTACHMENT;
						glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);
					}
				}
				break;
				default:
					break;
				}
			}
		}

		void
		GL30DeviceContext::readFramebuffer(std::uint32_t i, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
			GLenum internalFormat = GL30Types::asTextureFormat(texture->getTextureDesc().getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid texture format");
				return;
			}

			glReadBuffer(GL_COLOR_ATTACHMENT0 + i);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(texture->downcast<GL30Texture>()->getTarget(), texture->downcast<GL30Texture>()->getInstanceID());
			glCopyTexImage2D(texture->downcast<GL30Texture>()->getTarget(), miplevel, internalFormat, x, y, width, height, 0);
		}

		void
		GL30DeviceContext::readFramebufferToCube(std::uint32_t i, std::uint32_t face, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
			GLenum internalFormat = GL30Types::asTextureFormat(texture->getTextureDesc().getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid texture format");
				return;
			}

			glReadBuffer(GL_COLOR_ATTACHMENT0 + i);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(texture->downcast<GL30Texture>()->getTarget(), texture->downcast<GL30Texture>()->getInstanceID());
			glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, miplevel, internalFormat, x, y, width, height, 0);
		}

		GraphicsFramebufferPtr
		GL30DeviceContext::getFramebuffer() const noexcept
		{
			return _framebuffer;
		}

		void
		GL30DeviceContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
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

			if (numVertices > 0)
			{
				GLenum drawType = GL30Types::asVertexType(_stateCaptured.getPrimitiveType());
				if (drawType != GL_INVALID_ENUM)
				{
					if (numInstances == 1)
						glDrawArrays(drawType, startVertice, numVertices);
					else
						glDrawArraysInstanced(drawType, startVertice, numVertices, numInstances);
				}
				else
					this->getDevice()->downcast<OGLDevice>()->message("Invalid vertex type");
			}
		}

		void
		GL30DeviceContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(_indexBuffer);
			assert(_indexType == GL_UNSIGNED_INT || _indexType == GL_UNSIGNED_SHORT);
			assert(startInstances == 0);

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
				
				GLenum drawType = GL30Types::asVertexType(_stateCaptured.getPrimitiveType());
				if (drawType != GL_INVALID_ENUM)
				{
					if (numInstances == 1)
						glDrawElements(drawType, numIndices, _indexType, offsetIndices);
					else
						glDrawElementsInstanced(drawType, numIndices, _indexType, offsetIndices, numInstances);
				}
				else
				{
					this->getDevice()->downcast<OGLDevice>()->message("Invalid vertex type");
				}
			}
		}

		void
		GL30DeviceContext::drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
		}

		void
		GL30DeviceContext::drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
		}

		void
		GL30DeviceContext::present() noexcept
		{
			assert(_glcontext->getActive());
			_glcontext->present();
		}

		void
		GL30DeviceContext::startDebugControl() noexcept
		{
		}

		void
		GL30DeviceContext::stopDebugControl() noexcept
		{
		}

		bool
		GL30DeviceContext::initStateSystem() noexcept
		{
			glClearDepthf(1.0f);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearStencil(0);

			glViewport(0, 0, 0, 0);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LEQUAL);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CW);

			glDisable(GL_STENCIL_TEST);
			glStencilMask(0xFFFFFFFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);

			glDisable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glGenVertexArrays(1, &_inputLayout);
			glBindVertexArray(_inputLayout);

			auto& deviceProperties = this->getDevice()->getDeviceProperty().getDeviceProperties();
			_vertexBuffers.resize(deviceProperties.maxVertexInputBindings);
			_viewports.resize(deviceProperties.maxViewports, float4(0, 0, 0, 0));
			_scissors.resize(deviceProperties.maxViewports, uint4(0, 0, 0, 0));

			GraphicsColorBlends blends(deviceProperties.maxFramebufferColorAttachments);
			_stateCaptured.setColorBlends(blends);

			return true;
		}

		void
		GL30DeviceContext::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL30DeviceContext::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}