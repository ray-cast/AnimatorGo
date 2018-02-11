#include "ogl_framebuffer.h"
#include "ogl_texture.h"
#include "ogl_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLFramebuffer, GraphicsFramebuffer, "OGLFramebuffer")
		OctoonImplementSubClass(OGLFramebufferLayout, GraphicsFramebufferLayout, "OGLFramebufferLayout")

		OGLFramebufferLayout::OGLFramebufferLayout() noexcept
		{
		}

		OGLFramebufferLayout::~OGLFramebufferLayout() noexcept
		{
			this->close();
		}

		bool
		OGLFramebufferLayout::setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept
		{
			_framebufferLayoutDesc = framebufferDesc;
			return true;
		}

		void
		OGLFramebufferLayout::close() noexcept
		{
		}

		const GraphicsFramebufferLayoutDesc&
		OGLFramebufferLayout::getGraphicsFramebufferLayoutDesc() const noexcept
		{
			return _framebufferLayoutDesc;
		}

		void
		OGLFramebufferLayout::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLFramebufferLayout::getDevice() noexcept
		{
			return _device.lock();
		}

		OGLFramebuffer::OGLFramebuffer() noexcept
			: _fbo(GL_NONE)
		{
		}

		OGLFramebuffer::~OGLFramebuffer() noexcept
		{
			this->close();
		}

		bool
		OGLFramebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
		{
			assert(GL_NONE == _fbo);
			assert(framebufferDesc.getGraphicsFramebufferLayout());
			assert(framebufferDesc.getGraphicsFramebufferLayout()->isInstanceOf<OGLFramebufferLayout>());
			assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

			glGenFramebuffers(1, &_fbo);
			if (_fbo == GL_NONE)
			{
				this->getDevice()->downcast<OGLDevice>()->message("glCreateFramebuffers() fail.");
				return false;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

			GLenum drawCount = 0;
			GLenum drawBuffers[GL_COLOR_ATTACHMENT15 - GL_COLOR_ATTACHMENT0];

			const auto& textureComponents = framebufferDesc.getGraphicsFramebufferLayout()->getGraphicsFramebufferLayoutDesc().getComponents();
			const auto& colorAttachments = framebufferDesc.getColorAttachments();
			if (colorAttachments.size() > (sizeof(drawBuffers) / sizeof(drawBuffers[0])))
			{
				this->getDevice()->downcast<OGLDevice>()->message("The color attachment in framebuffer is out of range.");
				return false;
			}

			for (std::size_t i = 0; i < textureComponents.size(); i++)
			{
				auto type = textureComponents[i].getAttachType();
				switch (type)
				{
				case GraphicsImageLayout::GraphicsImageLayoutGeneral:
					break;
				case GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal:
				{
					GLint slot = GL_COLOR_ATTACHMENT0 + textureComponents[i].getAttachSlot();
					GLint mipLevel = colorAttachments[drawCount].getBindingLevel();
					GLint layer = colorAttachments[drawCount].getBindingLayer();

					if (!this->bindRenderTexture(colorAttachments[drawCount].getBindingTexture(), slot, mipLevel, layer))
						return false;

					drawBuffers[drawCount++] = slot;
				}
				break;
				case GraphicsImageLayout::GraphicsImageLayoutDepthStencilAttachmentOptimal:
				case GraphicsImageLayout::GraphicsImageLayoutDepthStencilReadOnlyOptimal:
				{
					const auto& depthStencilAttachment = framebufferDesc.getDepthStencilAttachment();
					if (!depthStencilAttachment.getBindingTexture())
					{
						this->getDevice()->downcast<OGLDevice>()->message("Need depth or stencil texture.");
						return false;
					}

					auto texture = depthStencilAttachment.getBindingTexture();
					auto format = texture->getGraphicsTextureDesc().getTexFormat();
					auto level = depthStencilAttachment.getBindingLevel();
					auto layer = depthStencilAttachment.getBindingLayer();

					if (OGLTypes::isDepthStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else if (OGLTypes::isDepthFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT, level, layer))
							return false;
					}
					else if (OGLTypes::isStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else
					{
						this->getDevice()->downcast<OGLDevice>()->message("Invalid texture format");
						return false;
					}
				}
				case GraphicsImageLayout::GraphicsImageLayoutShaderReadOnlyOptimal:
					break;
				case GraphicsImageLayout::GraphicsImageLayoutTransferSrcOptimal:
					break;
				case GraphicsImageLayout::GraphicsImageLayoutTransferDstOptimal:
					break;
				case GraphicsImageLayout::GraphicsImageLayoutPreinitialized:
					break;
				case GraphicsImageLayout::GraphicsImageLayoutPresentSrcKhr:
					break;
				default:
					break;
				}
			}

			glDrawBuffers(drawCount, drawBuffers);
			glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

			_framebufferDesc = framebufferDesc;
			return OGLCheck::checkError();
		}

		void
		OGLFramebuffer::close() noexcept
		{
			if (_fbo != GL_NONE)
			{
				glDeleteFramebuffers(1, &_fbo);
				_fbo = GL_NONE;
			}
		}

		GLuint
		OGLFramebuffer::getInstanceID() noexcept
		{
			return _fbo;
		}

		bool
		OGLFramebuffer::bindRenderTexture(GraphicsTexturePtr renderTexture, GLenum attachment, GLint level, GLint layer) noexcept
		{
			assert(renderTexture);

			auto texture = renderTexture->downcast<OGLTexture>();
			auto textureID = texture->getInstanceID();
			auto textureTarget = texture->getTarget();
			auto& textureDesc = renderTexture->getGraphicsTextureDesc();

			if (textureDesc.getTexDim() == GraphicsTextureDim::GraphicsTextureDim2DArray)
				glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, textureID, level, layer);
			else if (textureDesc.getTexDim() == GraphicsTextureDim::GraphicsTextureDimCube)
				glFramebufferTexture(GL_FRAMEBUFFER, attachment, textureID, level);
			else if (textureDesc.getTexDim() == GraphicsTextureDim::GraphicsTextureDimCubeArray)
				glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, textureTarget, textureID, level, layer);
			else
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textureTarget, textureID, level);

			return OGLCheck::checkError();
		}

		const GraphicsFramebufferDesc&
		OGLFramebuffer::getGraphicsFramebufferDesc() const noexcept
		{
			return _framebufferDesc;
		}

		void
		OGLFramebuffer::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLFramebuffer::getDevice() noexcept
		{
			return _device.lock();
		}
}
}