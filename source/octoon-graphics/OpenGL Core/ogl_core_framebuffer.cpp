#include "ogl_core_framebuffer.h"
#include "ogl_core_texture.h"
#include "ogl_framebuffer.h"
#include "ogl_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLCoreFramebuffer, GraphicsFramebuffer, "OGLCoreFramebuffer")

		OGLCoreFramebuffer::OGLCoreFramebuffer() noexcept
			: _fbo(GL_NONE)
		{
		}

		OGLCoreFramebuffer::~OGLCoreFramebuffer() noexcept
		{
			this->close();
		}

		bool
		OGLCoreFramebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
		{
			assert(GL_NONE == _fbo);
			assert(framebufferDesc.getGraphicsFramebufferLayout());
			assert(framebufferDesc.getGraphicsFramebufferLayout()->is_instance_of<OGLFramebufferLayout>());
			assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

			glCreateFramebuffers(1, &_fbo);
			if (_fbo == GL_NONE)
			{
				this->getDevice()->downcast<OGLDevice>()->message("glCreateFramebuffers() fail");
				return false;
			}

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
				case GraphicsImageLayout::General:
					break;
				case GraphicsImageLayout::ColorAttachmentOptimal:
				{
					GLint slot = GL_COLOR_ATTACHMENT0 + textureComponents[i].getAttachSlot();
					GLint mipLevel = colorAttachments[drawCount].getBindingLevel();
					GLint layer = colorAttachments[drawCount].getBindingLayer();

					if (!this->bindRenderTexture(colorAttachments[drawCount].getBindingTexture(), slot, mipLevel, layer))
						return false;

					drawBuffers[drawCount++] = slot;
				}
				break;
				case GraphicsImageLayout::DepthStencilAttachmentOptimal:
				case GraphicsImageLayout::DepthStencilReadOnlyOptimal:
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
				case GraphicsImageLayout::ShaderReadOnlyOptimal:
					break;
				case GraphicsImageLayout::TransferSrcOptimal:
					break;
				case GraphicsImageLayout::TransferDstOptimal:
					break;
				case GraphicsImageLayout::Preinitialized:
					break;
				case GraphicsImageLayout::PresentSrcKhr:
					break;
				default:
					break;
				}
			}

			glNamedFramebufferDrawBuffers(_fbo, drawCount, drawBuffers);

			_framebufferDesc = framebufferDesc;
			return OGLCheck::checkError();
		}

		void
		OGLCoreFramebuffer::close() noexcept
		{
			if (_fbo != GL_NONE)
			{
				glDeleteFramebuffers(1, &_fbo);
				_fbo = GL_NONE;
			}
		}

		GLuint
		OGLCoreFramebuffer::getInstanceID() noexcept
		{
			return _fbo;
		}

		bool
		OGLCoreFramebuffer::bindRenderTexture(GraphicsTexturePtr renderTexture, GLenum attachment, GLint level, GLint layer) noexcept
		{
			assert(renderTexture);

			auto texture = renderTexture->downcast<OGLCoreTexture>();
			auto textureID = texture->getInstanceID();
			auto& textureDesc = renderTexture->getGraphicsTextureDesc();

			if (textureDesc.getTexDim() == GraphicsTextureDim::Texture2DArray ||
				textureDesc.getTexDim() == GraphicsTextureDim::Cube ||
				textureDesc.getTexDim() == GraphicsTextureDim::CubeArray)
			{
				glNamedFramebufferTextureLayer(_fbo, attachment, textureID, level, layer);
			}
			else
			{
				glNamedFramebufferTexture(_fbo, attachment, textureID, level);
			}

			return OGLCheck::checkError();
		}

		const GraphicsFramebufferDesc&
		OGLCoreFramebuffer::getGraphicsFramebufferDesc() const noexcept
		{
			return _framebufferDesc;
		}

		void
		OGLCoreFramebuffer::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLCoreFramebuffer::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}