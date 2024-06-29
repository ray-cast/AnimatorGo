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
			_stateDefault->setup(RenderStateDesc());
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

			this->setRenderPipeline(nullptr);
			this->setIndexBufferData(nullptr, 0, IndexFormat::UInt32);
		}

		void
		GL30DeviceContext::renderEnd() noexcept
		{
		}

		void
		GL30DeviceContext::setViewport(std::uint32_t i, const math::float4& view) noexcept
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

		const math::float4&
		GL30DeviceContext::getViewport(std::uint32_t i) const noexcept
		{
			return _viewports[i];
		}

		void
		GL30DeviceContext::setScissor(std::uint32_t i, const math::uint4& scissor) noexcept
		{
			assert(_glcontext->getActive());

			if (_scissors[i] != scissor)
			{
				std::uint32_t height;
				if (_framebuffer)
					height = _framebuffer->getFramebufferDesc().getHeight();
				else
					height = _glcontext->getGraphicsSwapchainDesc().getHeight();

				if (i == 0)
					glScissor(scissor.left, height - scissor.height - scissor.top, scissor.width, scissor.height);
				else
					this->getDevice()->downcast<OGLDevice>()->message("Cannot support glScissorIndexed.");

				_scissors[i] = scissor;
			}
		}

		const math::uint4&
		GL30DeviceContext::getScissor(std::uint32_t i) const noexcept
		{
			return _scissors[i];
		}
		void
		GL30DeviceContext::setStencilCompareMask(StencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & StencilFaceFlagBits::FrontBit)
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
			if (face & StencilFaceFlagBits::BackBit)
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
		GL30DeviceContext::getStencilCompareMask(StencilFaceFlags face) noexcept
		{
			assert(face == StencilFaceFlagBits::FrontBit || face == StencilFaceFlagBits::BackBit);

			if (face == StencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontReadMask();
			else
				return _stateCaptured.getStencilBackReadMask();
		}

		void
		GL30DeviceContext::setStencilReference(StencilFaceFlags face, std::uint32_t reference) noexcept
		{
			if (face & StencilFaceFlagBits::FrontBit)
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
			if (face & StencilFaceFlagBits::BackBit)
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
		GL30DeviceContext::getStencilReference(StencilFaceFlags face) noexcept
		{
			assert(face == StencilFaceFlagBits::FrontBit || face == StencilFaceFlagBits::BackBit);

			if (face == StencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontRef();
			else
				return _stateCaptured.getStencilBackRef();
		}

		void
		GL30DeviceContext::setStencilWriteMask(StencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & StencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontWriteMask() != mask)
				{
					glStencilMaskSeparate(GL_FRONT, mask);
					_stateCaptured.setStencilFrontWriteMask(mask);
				}
			}
			if (face & StencilFaceFlagBits::BackBit)
			{
				if (_stateCaptured.getStencilBackWriteMask() != mask)
				{
					glStencilMaskSeparate(GL_BACK, mask);
					_stateCaptured.setStencilBackWriteMask(mask);
				}
			}
		}

		std::uint32_t
		GL30DeviceContext::getStencilWriteMask(StencilFaceFlags face) noexcept
		{
			assert(face == StencilFaceFlagBits::FrontBit || face == StencilFaceFlagBits::BackBit);

			if (face == StencilFaceFlagBits::FrontBit)
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

					auto glprogram = pipelineDesc.getProgram()->downcast_pointer<GL30Program>();
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
				_vertexBuffers[i].setDirty = true;
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
		GL30DeviceContext::setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, IndexFormat indexType) noexcept
		{
			if (data)
			{
				assert(data->isInstanceOf<GL30GraphicsData>());
				assert(data->getDataDesc().getType() == GraphicsDataType::StorageIndexBuffer);
				assert(indexType == IndexFormat::UInt16 || indexType == IndexFormat::UInt32);
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
		GL30DeviceContext::generateMipmap(const std::shared_ptr<GraphicsTexture>& texture) noexcept
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

					auto& framebufferDesc = framebuffer->getFramebufferDesc();
					auto& colorAttachment = framebufferDesc.getColorAttachments();

					std::uint32_t viewportCount = std::max<std::uint32_t>(1, static_cast<std::uint32_t>(colorAttachment.size()));
					for (std::uint32_t i = 0; i < viewportCount; i++)
					{
						this->setViewport(i, math::float4(0, 0, (float)framebufferDesc.getWidth(), (float)framebufferDesc.getHeight()));
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
		GL30DeviceContext::clearFramebuffer(std::uint32_t i, ClearFlags flags, const math::float4& color, float depth, std::int32_t stencil) noexcept
		{
			assert(i == 0);
			assert(_glcontext->getActive());

			GLint buffer = i;

			if (_stateCaptured.getScissorTestEnable())
			{
				if (_framebuffer)
					glScissor(0, 0, _framebuffer->getFramebufferDesc().getWidth(), _framebuffer->getFramebufferDesc().getHeight());
				else
					glScissor(0, 0, _glcontext->getGraphicsSwapchainDesc().getWidth(), _glcontext->getGraphicsSwapchainDesc().getHeight());
			}

			if (flags & ClearFlagBits::DepthBit)
			{
				auto depthWriteEnable = _stateCaptured.getDepthWriteEnable();
				if (!depthWriteEnable)
				{
					glDepthMask(GL_TRUE);
				}

				if (flags & ClearFlagBits::StencilBit)
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
			else if (flags & ClearFlagBits::StencilBit)
			{
				GLint s = stencil;
				glClearBufferiv(GL_STENCIL, buffer, &s);
			}

			if (flags & ClearFlagBits::ColorBit)
			{
				auto colorWriteFlags = _stateCaptured.getColorWriteMask();
				if (colorWriteFlags != ColorWriteMask::RGBABit)
				{
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				}

				glClearBufferfv(GL_COLOR, buffer, color.ptr());

				if (colorWriteFlags != ColorWriteMask::RGBABit)
				{
					GLboolean r = colorWriteFlags & ColorWriteMask::RedBit ? GL_TRUE : GL_FALSE;
					GLboolean g = colorWriteFlags & ColorWriteMask::GreendBit ? GL_TRUE : GL_FALSE;
					GLboolean b = colorWriteFlags & ColorWriteMask::BlurBit ? GL_TRUE : GL_FALSE;
					GLboolean a = colorWriteFlags & ColorWriteMask::AlphaBit ? GL_TRUE : GL_FALSE;
					glColorMask(r, g, b, a);
				}
			}

			if (_stateCaptured.getScissorTestEnable())
			{
				if (_framebuffer)
				{
					auto w = _framebuffer->getFramebufferDesc().getWidth();
					auto h = _framebuffer->getFramebufferDesc().getWidth();
					glScissor(0, 0, w, h);
				}
				else
				{
					glScissor(_scissors[0].left, _scissors[0].top, _scissors[0].width, _scissors[0].height);
				}
			}
		}

		void
		GL30DeviceContext::blitFramebuffer(const GraphicsFramebufferPtr& src, const math::float4& v1, const GraphicsFramebufferPtr& dest, const math::float4& v2, SamplerFilter filter) noexcept
		{
			assert(src);
			assert(src->isInstanceOf<GL30Framebuffer>());
			assert(!dest || (dest && dest->isInstanceOf<GL30Framebuffer>()));
			assert(_glcontext->getActive());

			this->_readFramebuffer = src ? src->downcast_pointer<GL30Framebuffer>() : nullptr;
			this->_drawFramebuffer = dest ? dest->downcast_pointer<GL30Framebuffer>() : nullptr;

			auto readFramebuffer = this->_readFramebuffer->getInstanceID();
			auto drawFramebuffer = this->_drawFramebuffer ? this->_drawFramebuffer->getInstanceID() : GL_NONE;

			glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);

			GLenum mode = (filter == SamplerFilter::Linear ? GL_LINEAR : GL_NEAREST);
			glBlitFramebuffer((GLint)v1.left, (GLint)v1.top, (GLint)v1.width, (GLint)v1.height, (GLint)v2.left, (GLint)v2.top, (GLint)v2.width, (GLint)v2.height, GL_COLOR_BUFFER_BIT, mode);

			_framebuffer = nullptr;
		}

		void
		GL30DeviceContext::discardFramebuffer(const GraphicsFramebufferPtr& framebuffer, ClearFlags flags) noexcept
		{
			assert(framebuffer);
			assert(_glcontext->getActive());

			if (framebuffer)
			{
				auto fbo = framebuffer->downcast_pointer<GL30Framebuffer>();

				GLenum target = GL_FRAMEBUFFER;
				if (fbo == _readFramebuffer)
					target = GL_READ_FRAMEBUFFER;
				else if (fbo == _drawFramebuffer)
					target = GL_DRAW_FRAMEBUFFER;

				_attachments.clear();

				const auto& layoutDesc = fbo->getFramebufferDesc().getFramebufferLayout()->getFramebufferLayoutDesc();
				auto count = layoutDesc.getComponents().size();
				for (GLenum i = 0; i < count; i++)
				{
					auto& attachment = layoutDesc.getComponents().at(i);
					if (attachment.getAttachType() == GraphicsImageLayout::ColorAttachmentOptimal)
					{
						if (flags & ClearFlagBits::ColorBit)
							_attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
					}
					else if (attachment.getAttachType() == GraphicsImageLayout::DepthStencilReadOnlyOptimal ||
						attachment.getAttachType() == GraphicsImageLayout::DepthStencilAttachmentOptimal)
					{
						auto format = attachment.getAttachFormat();
						if (format == GraphicsFormat::S8UInt)
						{
							if (flags & ClearFlagBits::StencilBit)
								_attachments.push_back(GL_STENCIL_ATTACHMENT);
						}
						else if (format == GraphicsFormat::D16UNorm || format == GraphicsFormat::X8_D24UNormPack32 || format == GraphicsFormat::D32_SFLOAT)
						{
							if (flags & ClearFlagBits::DepthBit)
								_attachments.push_back(GL_DEPTH_ATTACHMENT);
						}
						else
						{
							if (flags & ClearFlagBits::DepthStencilBit)
								_attachments.push_back(GL_DEPTH_STENCIL_ATTACHMENT);
						}
					}
				}

				if (!_attachments.empty())
					glInvalidateFramebuffer(target, (GLsizei)_attachments.size(), _attachments.data());
			}
		}

		void
		GL30DeviceContext::readFramebuffer(std::uint32_t i, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
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
		GL30DeviceContext::readFramebufferToCube(std::uint32_t i, std::uint32_t face, const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
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

			auto& deviceProperties = this->getDevice()->getSystemInfo();
			_vertexBuffers.resize(deviceProperties.maxVertexInputBindings);
			_viewports.resize(deviceProperties.maxViewports, math::float4(0, 0, 0, 0));
			_scissors.resize(deviceProperties.maxViewports, math::uint4(0, 0, 0, 0));

			return true;
		}

		void
		GL30DeviceContext::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL30DeviceContext::getDevice() const noexcept
		{
			return _device.lock();
		}
	}
}