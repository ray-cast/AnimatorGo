#include "gl45_framebuffer.h"
#include "gl45_texture.h"
#include "gl33_framebuffer.h"
#include "gl33_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL45Framebuffer, GraphicsFramebuffer, "GL45Framebuffer")

		GL45Framebuffer::GL45Framebuffer() noexcept
			: _fbo(GL_NONE)
		{
		}

		GL45Framebuffer::~GL45Framebuffer() noexcept
		{
			this->close();
		}

		bool
		GL45Framebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
		{
			assert(GL_NONE == _fbo);
			assert(framebufferDesc.getFramebufferLayout());
			assert(framebufferDesc.getFramebufferLayout()->isInstanceOf<GL33FramebufferLayout>());
			assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

			glCreateFramebuffers(1, &_fbo);
			if (_fbo == GL_NONE)
			{
				this->getDevice()->downcast<GL33Device>()->message("glCreateFramebuffers() fail");
				return false;
			}

			GLenum drawCount = 0;
			GLenum drawBuffers[GL_COLOR_ATTACHMENT15 - GL_COLOR_ATTACHMENT0];

			const auto& textureComponents = framebufferDesc.getFramebufferLayout()->getFramebufferLayoutDesc().getComponents();
			const auto& colorAttachments = framebufferDesc.getColorAttachments();
			if (colorAttachments.size() > (sizeof(drawBuffers) / sizeof(drawBuffers[0])))
			{
				this->getDevice()->downcast<GL33Device>()->message("The color attachment in framebuffer is out of range.");
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
						this->getDevice()->downcast<GL33Device>()->message("Need depth or stencil texture.");
						return false;
					}

					auto texture = depthStencilAttachment.getBindingTexture();
					auto format = texture->getTextureDesc().getTexFormat();
					auto level = depthStencilAttachment.getBindingLevel();
					auto layer = depthStencilAttachment.getBindingLayer();

					if (GL33Types::isDepthStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else if (GL33Types::isDepthFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT, level, layer))
							return false;
					}
					else if (GL33Types::isStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else
					{
						this->getDevice()->downcast<GL33Device>()->message("Invalid texture format");
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
			return GL33Check::checkError();
		}

		void
		GL45Framebuffer::close() noexcept
		{
			if (_fbo != GL_NONE)
			{
				glDeleteFramebuffers(1, &_fbo);
				_fbo = GL_NONE;
			}
		}

		GLuint
		GL45Framebuffer::getInstanceID() noexcept
		{
			return _fbo;
		}

		bool
		GL45Framebuffer::bindRenderTexture(GraphicsTexturePtr renderTexture, GLenum attachment, GLint level, GLint layer) noexcept
		{
			assert(renderTexture);

			auto texture = renderTexture->downcast<GL45Texture>();
			auto textureID = texture->getInstanceID();
			auto& textureDesc = renderTexture->getTextureDesc();

			if (textureDesc.getTexDim() == GraphicsTextureDim::Texture2DArray ||
				textureDesc.getTexDim() == GraphicsTextureDim::Texture2DArrayMultisample ||
				textureDesc.getTexDim() == GraphicsTextureDim::Cube ||
				textureDesc.getTexDim() == GraphicsTextureDim::CubeArray)
			{
				glNamedFramebufferTextureLayer(_fbo, attachment, textureID, level, layer);
			}
			else
			{
				glNamedFramebufferTexture(_fbo, attachment, textureID, level);
			}

			return GL33Check::checkError();
		}

		const GraphicsFramebufferDesc&
		GL45Framebuffer::getGraphicsFramebufferDesc() const noexcept
		{
			return _framebufferDesc;
		}

		void
		GL45Framebuffer::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL45Framebuffer::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}