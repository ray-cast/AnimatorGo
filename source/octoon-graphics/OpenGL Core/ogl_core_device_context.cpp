#include "ogl_core_device_context.h"
#include "ogl_core_texture.h"
#include "ogl_core_framebuffer.h"
#include "ogl_core_graphics_data.h"
#include "ogl_core_descriptor.h"
#include "ogl_core_pipeline.h"
#include "ogl_input_layout.h"
#include "ogl_state.h"
#include "ogl_shader.h"
#include "ogl_sampler.h"
#include "ogl_swapchain.h"
#include "ogl_device.h"

#include <iostream>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLCoreDeviceContext, GraphicsContext, "OGLCoreDeviceContext")

		OGLCoreDeviceContext::OGLCoreDeviceContext() noexcept
			: _clearDepth(1.0f)
			, _clearStencil(0)
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

		OGLCoreDeviceContext::~OGLCoreDeviceContext() noexcept
		{
			this->close();
		}

		bool
		OGLCoreDeviceContext::setup(const GraphicsContextDesc& desc) noexcept
		{
			assert(desc.getSwapchain());
			assert(desc.getSwapchain()->isInstanceOf<OGLSwapchain>());

			_glcontext = desc.getSwapchain()->downcast_pointer<OGLSwapchain>();
			_glcontext->setActive(true);

			if (!_glcontext->getActive())
				return false;

			if (!this->checkSupport())
				return false;

			if (GraphicsSystem::instance()->enableDebugControl())
				this->startDebugControl();

			if (!this->initStateSystem())
				return false;

			return true;
		}

		void
		OGLCoreDeviceContext::close() noexcept
		{
			_framebuffer.reset();
			_program.reset();
			_pipeline.reset();
			_descriptorSet.reset();
			_state.reset();
			_indexBuffer.reset();
			_glcontext.reset();
			_vertexBuffers.clear();

			if (_inputLayout != GL_NONE)
			{
				glDeleteVertexArrays(1, &_inputLayout);
				_inputLayout = GL_NONE;
			}
		}

		void
		OGLCoreDeviceContext::renderBegin() noexcept
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
		OGLCoreDeviceContext::renderEnd() noexcept
		{
		}

		void
		OGLCoreDeviceContext::setViewport(std::uint32_t i, const float4& view) noexcept
		{
			assert(_glcontext->getActive());

			if (_viewports[i] != view)
			{
				glViewportIndexedf(i, view.left, view.top, view.width, view.height);
				_viewports[i] = view;
			}
		}

		const float4&
		OGLCoreDeviceContext::getViewport(std::uint32_t i) const noexcept
		{
			return _viewports[i];
		}

		void
		OGLCoreDeviceContext::setScissor(std::uint32_t i, const uint4& scissor) noexcept
		{
			assert(_glcontext->getActive());

			if (_scissors[i] != scissor)
			{
				std::uint32_t height;
				if (_framebuffer)
					height = _framebuffer->getGraphicsFramebufferDesc().getHeight();
				else
					height = _glcontext->getGraphicsSwapchainDesc().getHeight();

				glScissorIndexed(i, scissor.left, height - scissor.height - scissor.top, scissor.width, scissor.height);
				_scissors[i] = scissor;
			}
		}

		const uint4&
		OGLCoreDeviceContext::getScissor(std::uint32_t i) const noexcept
		{
			return _scissors[i];
		}

		void
		OGLCoreDeviceContext::setStencilCompareMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontReadMask() != mask)
				{
					GLenum frontfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilFrontFunc());
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
					GLenum backfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilBackFunc());
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
		OGLCoreDeviceContext::getStencilCompareMask(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontReadMask();
			else
				return _stateCaptured.getStencilBackReadMask();
		}

		void
		OGLCoreDeviceContext::setStencilReference(GraphicsStencilFaceFlags face, std::uint32_t reference) noexcept
		{
			if (face & GraphicsStencilFaceFlagBits::FrontBit)
			{
				if (_stateCaptured.getStencilFrontRef() != reference)
				{
					GLenum frontfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilFrontFunc());
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
					GLenum backfunc = OGLTypes::asCompareFunction(_stateCaptured.getStencilBackFunc());
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
		OGLCoreDeviceContext::getStencilReference(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontRef();
			else
				return _stateCaptured.getStencilBackRef();
		}

		void
		OGLCoreDeviceContext::setStencilWriteMask(GraphicsStencilFaceFlags face, std::uint32_t mask) noexcept
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
		OGLCoreDeviceContext::getStencilWriteMask(GraphicsStencilFaceFlags face) noexcept
		{
			assert(face == GraphicsStencilFaceFlagBits::FrontBit || face == GraphicsStencilFaceFlagBits::BackBit);

			if (face == GraphicsStencilFaceFlagBits::FrontBit)
				return _stateCaptured.getStencilFrontWriteMask();
			else
				return _stateCaptured.getStencilBackWriteMask();
		}

		void
		OGLCoreDeviceContext::setRenderPipeline(const GraphicsPipelinePtr& pipeline) noexcept
		{
			assert(pipeline);
			assert(pipeline->isInstanceOf<OGLCorePipeline>());
			assert(_glcontext->getActive());

			auto glpipeline = pipeline->downcast_pointer<OGLCorePipeline>();
			if (_pipeline != glpipeline)
			{
				auto& pipelineDesc = pipeline->getGraphicsPipelineDesc();

				auto glstate = pipelineDesc.getGraphicsState()->downcast_pointer<OGLGraphicsState>();
				if (_state != glstate)
				{
					glstate->apply(_stateCaptured);
					_state = glstate;
				}

				auto glprogram = pipelineDesc.getGraphicsProgram()->downcast_pointer<OGLProgram>();
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

		GraphicsPipelinePtr
		OGLCoreDeviceContext::getRenderPipeline() const noexcept
		{
			return _pipeline;
		}

		void
		OGLCoreDeviceContext::setDescriptorSet(const GraphicsDescriptorSetPtr& descriptorSet) noexcept
		{
			assert(descriptorSet);
			assert(descriptorSet->isInstanceOf<OGLCoreDescriptorSet>());
			assert(_glcontext->getActive());

			_descriptorSet = descriptorSet->downcast_pointer<OGLCoreDescriptorSet>();
			_needUpdateDescriptor = true;
		}

		GraphicsDescriptorSetPtr
		OGLCoreDeviceContext::getDescriptorSet() const noexcept
		{
			return _descriptorSet;
		}

		void
		OGLCoreDeviceContext::setVertexBufferData(std::uint32_t i, const GraphicsDataPtr& data, std::intptr_t offset) noexcept
		{
			assert(data);
			assert(data->isInstanceOf<OGLCoreGraphicsData>());
			assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::StorageVertexBuffer);
			assert(_vertexBuffers.size() > i);
			assert(_glcontext->getActive());

			auto vbo = data->downcast_pointer<OGLCoreGraphicsData>();
			if (_vertexBuffers[i].vbo != vbo || _vertexBuffers[i].offset != offset)
			{
				_vertexBuffers[i].vbo = vbo;
				_vertexBuffers[i].offset = offset;
				_vertexBuffers[i].needUpdate = true;
				_needUpdateVertexBuffers = true;
			}
		}

		GraphicsDataPtr
		OGLCoreDeviceContext::getVertexBufferData(std::uint32_t i) const noexcept
		{
			assert(_vertexBuffers.size() > i);
			return _vertexBuffers[i].vbo;
		}

		void
		OGLCoreDeviceContext::setIndexBufferData(const GraphicsDataPtr& data, std::intptr_t offset, GraphicsIndexType indexType) noexcept
		{
			assert(data);
			assert(data->isInstanceOf<OGLCoreGraphicsData>());
			assert(data->getGraphicsDataDesc().getType() == GraphicsDataType::StorageIndexBuffer);
			assert(indexType == GraphicsIndexType::UInt16 || indexType == GraphicsIndexType::UInt32);
			assert(_glcontext->getActive());

			auto ibo = data->downcast_pointer<OGLCoreGraphicsData>();
			if (_indexBuffer != ibo)
			{
				::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
				_indexBuffer = ibo;
			}

			_indexType = OGLTypes::asIndexType(indexType);
			_indexOffset = offset;

			if (_indexType == GL_INVALID_ENUM) this->getDevice()->downcast<OGLDevice>()->message("Invalid index type");
		}

		GraphicsDataPtr
		OGLCoreDeviceContext::getIndexBufferData() const noexcept
		{
			return _indexBuffer;
		}

		void
		OGLCoreDeviceContext::generateMipmap(const GraphicsTexturePtr& texture) noexcept
		{
			assert(texture);
			assert(texture->isInstanceOf<OGLCoreTexture>());

			auto gltexture = texture->downcast<OGLCoreTexture>();
			auto textureID = gltexture->getInstanceID();
			glGenerateTextureMipmap(textureID);
		}

		void
		OGLCoreDeviceContext::setFramebuffer(const GraphicsFramebufferPtr& target) noexcept
		{
			assert(_glcontext->getActive());

			if (target)
			{
				auto framebuffer = target->downcast_pointer<OGLCoreFramebuffer>();
				if (_framebuffer != framebuffer)
				{
					_framebuffer = framebuffer;
					glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer->getInstanceID());

					auto& framebufferDesc = _framebuffer->getGraphicsFramebufferDesc();
					auto& colorAttachment = framebufferDesc.getColorAttachments();

					std::uint32_t viewportCount = std::max<std::uint32_t>(1, static_cast<std::uint32_t>(colorAttachment.size()));
					for (std::uint32_t i = 0; i < viewportCount; i++)
					{
						this->setViewport(i, float4(0.0f, 0.0f, (float)framebufferDesc.getWidth(), (float)framebufferDesc.getHeight()));

						glScissorIndexed(i, _scissors[i].left, framebufferDesc.getHeight() - _scissors[i].height - _scissors[i].top, _scissors[i].width, _scissors[i].height);
					}
				}
			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

				if (_state)
				{
					_state->apply(_stateDefault);
					_state = nullptr;
				}

				_framebuffer = nullptr;
			}
		}

		void
		OGLCoreDeviceContext::clearFramebuffer(std::uint32_t i, GraphicsClearFlags flags, const float4& color, float depth, std::int32_t stencil) noexcept
		{
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
					glColorMaski(buffer, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				}

				glClearBufferfv(GL_COLOR, buffer, color.ptr());

				if (colorWriteFlags != GraphicsColorMaskFlagBits::RGBABit)
				{
					GLboolean r = colorWriteFlags & GraphicsColorMaskFlagBits::RedBit ? GL_TRUE : GL_FALSE;
					GLboolean g = colorWriteFlags & GraphicsColorMaskFlagBits::GreendBit ? GL_TRUE : GL_FALSE;
					GLboolean b = colorWriteFlags & GraphicsColorMaskFlagBits::BlurBit ? GL_TRUE : GL_FALSE;
					GLboolean a = colorWriteFlags & GraphicsColorMaskFlagBits::AlphaBit ? GL_TRUE : GL_FALSE;
					glColorMaski(buffer, r, g, b, a);
				}
			}

			if (_stateCaptured.getScissorTestEnable())
			{
				if (_framebuffer)
				{
					GLuint viewportCount = std::max<GLuint>(1, _framebuffer->getGraphicsFramebufferDesc().getColorAttachments().size());
					for (GLuint j = 0; j < viewportCount; j++)
					{
						glScissorIndexed(j, _scissors[j].left, _scissors[j].top, _scissors[j].width, _scissors[j].height);
					}
				}
				else
				{
					glScissorIndexed(0, _scissors[0].left, _scissors[0].top, _scissors[0].width, _scissors[0].height);
				}
			}
		}

		void
		OGLCoreDeviceContext::blitFramebuffer(const GraphicsFramebufferPtr& src, const float4& v1, const GraphicsFramebufferPtr& dest, const float4& v2) noexcept
		{
			assert(src);
			assert(src->isInstanceOf<OGLCoreFramebuffer>());
			assert(!dest || (dest && dest->isInstanceOf<OGLCoreFramebuffer>()));
			assert(_glcontext->getActive());

			auto readFramebuffer = src->downcast_pointer<OGLCoreFramebuffer>()->getInstanceID();
			auto drawFramebuffer = dest ? dest->downcast_pointer<OGLCoreFramebuffer>()->getInstanceID() : GL_NONE;

			glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, v1.left, v1.top, v1.width, v1.height, v2.left, v2.top, v2.width, v2.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		void
		OGLCoreDeviceContext::discardFramebuffer(std::uint32_t i) noexcept
		{
			assert(_framebuffer);
			assert(_glcontext->getActive());

			const auto& layoutDesc = _framebuffer->getGraphicsFramebufferDesc().getFramebufferLayout()->getGraphicsFramebufferLayoutDesc();
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
		OGLCoreDeviceContext::readFramebuffer(std::uint32_t i, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
			GLenum internalFormat = OGLTypes::asTextureInternalFormat(texture->getGraphicsTextureDesc().getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid texture internal format.");
				return;
			}

			glNamedFramebufferReadBuffer(_framebuffer->getInstanceID(), GL_COLOR_ATTACHMENT0 + i);
			glCopyTextureSubImage2D(texture->downcast<OGLCoreTexture>()->getInstanceID(), miplevel, 0, 0, x, y, width, height);
		}

		void
		OGLCoreDeviceContext::readFramebufferToCube(std::uint32_t i, std::uint32_t face, const GraphicsTexturePtr& texture, std::uint32_t miplevel, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) noexcept
		{
			GLenum internalFormat = OGLTypes::asTextureFormat(texture->getGraphicsTextureDesc().getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid texture format");
				return;
			}

			glNamedFramebufferReadBuffer(_framebuffer->getInstanceID(), GL_COLOR_ATTACHMENT0 + i);
			glCopyTextureSubImage2DEXT(texture->downcast<OGLCoreTexture>()->getInstanceID(), GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, miplevel, 0, 0, x, y, width, height);
		}

		GraphicsFramebufferPtr
		OGLCoreDeviceContext::getFramebuffer() const noexcept
		{
			return _framebuffer;
		}

		void
		OGLCoreDeviceContext::draw(std::uint32_t numVertices, std::uint32_t numInstances, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
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
				GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
				if (drawType != GL_INVALID_ENUM)
					glDrawArraysInstancedBaseInstance(drawType, startVertice, numVertices, numInstances, startInstances);
				else
					this->getDevice()->downcast<OGLDevice>()->message("Invalid vertex type");
			}
		}

		void
		OGLCoreDeviceContext::drawIndexed(std::uint32_t numIndices, std::uint32_t numInstances, std::uint32_t startIndice, std::uint32_t startVertice, std::uint32_t startInstances) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(_indexBuffer);
			assert(_indexType == GL_UNSIGNED_INT || _indexType == GL_UNSIGNED_SHORT);
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

			if (numIndices > 0)
			{
				GLbyte* offsetIndices = nullptr;
				if (_indexType == GL_UNSIGNED_INT)
					offsetIndices += _indexOffset + sizeof(std::uint32_t) * startIndice;
				else
					offsetIndices += _indexOffset + sizeof(std::uint16_t) * startIndice;

				GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
				if (drawType != GL_INVALID_ENUM)
					glDrawElementsInstancedBaseVertexBaseInstance(drawType, numIndices, _indexType, offsetIndices, numInstances, startVertice, startInstances);
				else
					this->getDevice()->downcast<OGLDevice>()->message("Invalid vertex type");
			}
		}

		void
		OGLCoreDeviceContext::drawIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(data && data->getGraphicsDataDesc().getType() == GraphicsDataType::IndirectBiffer);

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, data->downcast<OGLCoreGraphicsData>()->getInstanceID());

			if (drawCount > 0)
			{
				GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
				if (drawType != GL_INVALID_ENUM)
					glMultiDrawArraysIndirect(drawType, (char*)nullptr + offset, drawCount, stride);
				else
					this->getDevice()->downcast<OGLDevice>()->message("Invalid vertex type");
			}
		}

		void
		OGLCoreDeviceContext::drawIndexedIndirect(const GraphicsDataPtr& data, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) noexcept
		{
			assert(_pipeline);
			assert(_glcontext->getActive());
			assert(data && data->getGraphicsDataDesc().getType() == GraphicsDataType::IndirectBiffer);

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, data->downcast<OGLCoreGraphicsData>()->getInstanceID());

			if (drawCount > 0)
			{
				GLenum drawType = OGLTypes::asVertexType(_stateCaptured.getPrimitiveType());
				if (drawType != GL_INVALID_ENUM)
					glMultiDrawElementsIndirect(drawType, GL_UNSIGNED_INT, (char*)nullptr + offset, drawCount, stride);
				else
					this->getDevice()->downcast<OGLDevice>()->message("Invalid vertex type");
			}
		}

		void
		OGLCoreDeviceContext::present() noexcept
		{
			assert(_glcontext->getActive());
			_glcontext->present();
		}

		bool
		OGLCoreDeviceContext::checkSupport() noexcept
		{
			if (!GLEW_ARB_vertex_array_object)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support vertex array object");
				return false;
			}

			if (!GL_ARB_vertex_attrib_binding)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support vertex attrib binding");
				return false;
			}

			if (!GLEW_ARB_sampler_objects)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support sampler object");
				return false;
			}

			if (!GLEW_ARB_framebuffer_object)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support framebuffer object");
				return false;
			}

			if (!GLEW_ARB_uniform_buffer_object)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support uniform buffer object");
				return false;
			}

			if (!GLEW_ARB_texture_buffer_object)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support texture buffer object");
				return false;
			}

			if (!GL_ARB_buffer_storage)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support buffer storage");
				return false;
			}

			if (!GLEW_ARB_texture_storage)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support texture storage");
				return false;
			}

			if (!GLEW_EXT_texture_filter_anisotropic)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support anisotropic sampler");
				return false;
			}

			if (!GLEW_ARB_separate_shader_objects)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support separate shader objects");
				return false;
			}

			if (!GLEW_ARB_direct_state_access)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support direct state access");
				return false;
			}

			return true;
		}

		void
		OGLCoreDeviceContext::enableDebugControl(bool enable) noexcept
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
		OGLCoreDeviceContext::startDebugControl() noexcept
		{
			if (!GLEW_KHR_debug)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support GL_KHR_debug.");
				return;
			}

			// 131184 memory info
			// 131185 memory allocation info
			GLuint ids[] =
			{
				131076,
				131169,
				131184,
				131185,
				131218,
				131204
			};

			glEnable(GL_DEBUG_OUTPUT);

			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback(debugCallBack, this);

			// enable all
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);

			// disable ids
			glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 6, ids, GL_FALSE);
		}

		void
		OGLCoreDeviceContext::stopDebugControl() noexcept
		{
			if (GLEW_KHR_debug)
			{
				glDisable(GL_DEBUG_OUTPUT);

				glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			}
		}

		bool
		OGLCoreDeviceContext::initStateSystem() noexcept
		{
			glClearDepth(1.0f);
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

			if (GLEW_NV_vertex_buffer_unified_memory)
			{
				glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
			}

			auto& deviceProperties = this->getDevice()->getGraphicsDeviceProperty().getGraphicsDeviceProperties();
			_vertexBuffers.resize(deviceProperties.maxVertexInputBindings);
			_viewports.resize(deviceProperties.maxViewports, float4::Zero);
			_scissors.resize(deviceProperties.maxViewports, uint4::Zero);
			_clearColor.resize(deviceProperties.maxFramebufferColorAttachments, float4(0.0f, 0.0f, 0.0f, 0.0f));

			GraphicsColorBlends blends(deviceProperties.maxFramebufferColorAttachments);
			_stateCaptured.setColorBlends(blends);

			return true;
		}

		void
		OGLCoreDeviceContext::debugCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) noexcept
		{
			std::cerr << "source : ";
			switch (source)
			{
			case GL_DEBUG_SOURCE_API:
				std::cerr << "GL_DEBUG_SOURCE_API";
				break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
				std::cerr << "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
				break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER:
				std::cerr << "GL_DEBUG_SOURCE_SHADER_COMPILER";
				break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:
				std::cerr << "GL_DEBUG_SOURCE_THIRD_PARTY";
				break;
			case GL_DEBUG_SOURCE_APPLICATION:
				std::cerr << "GL_DEBUG_SOURCE_APPLICATION";
				break;
			case GL_DEBUG_SOURCE_OTHER:
				std::cerr << "GL_DEBUG_SOURCE_OTHER";
				break;
			}

			std::cerr << std::endl;

			std::cerr << "type : ";
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:
				std::cerr << "GL_DEBUG_TYPE_ERROR";
				break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
				std::cerr << "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
				break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				std::cerr << "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
				break;
			case GL_DEBUG_TYPE_PORTABILITY:
				std::cerr << "GL_DEBUG_TYPE_PORTABILITY";
				break;
			case GL_DEBUG_TYPE_PERFORMANCE:
				std::cerr << "GL_DEBUG_TYPE_PERFORMANCE";
				break;
			case GL_DEBUG_TYPE_OTHER:
				std::cerr << "GL_DEBUG_TYPE_OTHER";
				break;
			case GL_DEBUG_TYPE_MARKER:
				std::cerr << "GL_DEBUG_TYPE_MARKER";
				break;
			case GL_DEBUG_TYPE_PUSH_GROUP:
				std::cerr << "GL_DEBUG_TYPE_PUSH_GROUP";
				break;
			case GL_DEBUG_TYPE_POP_GROUP:
				std::cerr << "GL_DEBUG_TYPE_POP_GROUP";
				break;
			}

			std::cerr << std::endl;

			std::cerr << "id : " << id << std::endl;

			switch (severity)
			{
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				std::cerr << "notice";
				break;
			case GL_DEBUG_SEVERITY_LOW:
				std::cerr << "low";
				break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				std::cerr << "medium";
				break;
			case GL_DEBUG_SEVERITY_HIGH:
				std::cerr << "high";
				break;
			}

			std::cerr << std::endl;

			std::cerr << "message : " << message << std::endl;
		}

		void
		OGLCoreDeviceContext::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLCoreDeviceContext::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}