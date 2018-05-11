#include "egl2_framebuffer.h"
#include "egl2_texture.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(EGL2Framebuffer, GraphicsFramebuffer, "EGL2Framebuffer")
		OctoonImplementSubClass(EGL2FramebufferLayout, GraphicsFramebufferLayout, "EGL2FramebufferLayout")

		EGL2FramebufferLayout::EGL2FramebufferLayout() noexcept
		{
		}

		EGL2FramebufferLayout::~EGL2FramebufferLayout() noexcept
		{
			this->close();
		}

		bool
		EGL2FramebufferLayout::setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept
		{
			_framebufferLayoutDesc = framebufferDesc;
			return true;
		}

		void
		EGL2FramebufferLayout::close() noexcept
		{
		}

		const GraphicsFramebufferLayoutDesc&
		EGL2FramebufferLayout::getFramebufferLayoutDesc() const noexcept
		{
			return _framebufferLayoutDesc;
		}

		void
		EGL2FramebufferLayout::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2FramebufferLayout::getDevice() noexcept
		{
			return _device.lock();
		}

		EGL2Framebuffer::EGL2Framebuffer() noexcept
			: _fbo(GL_NONE)
		{
		}

		EGL2Framebuffer::~EGL2Framebuffer() noexcept
		{
			this->close();
		}

		bool
		EGL2Framebuffer::setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept
		{
			assert(GL_NONE == _fbo);
			assert(framebufferDesc.getFramebufferLayout());
			assert(framebufferDesc.getWidth() > 0 && framebufferDesc.getHeight() > 0);

			std::uint32_t numAttachment = framebufferDesc.getColorAttachments().size();
			if (numAttachment > 1)
			{
				GL_PLATFORM_LOG("Can't support multi framebuffer");
				return false;
			}

			GL_CHECK(glGenFramebuffers(1, &_fbo));
			if (_fbo == GL_NONE)
			{
				GL_PLATFORM_LOG("glCreateFramebuffers() fail");
				return false;
			}

			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));

			const auto& textureComponents = framebufferDesc.getFramebufferLayout()->getFramebufferLayoutDesc().getComponents();
			const auto& colorAttachments = framebufferDesc.getColorAttachments();

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
					GLint mipLevel = colorAttachments[0].getBindingLevel();
					GLint layer = colorAttachments[0].getBindingLayer();

					if (!this->bindRenderTexture(colorAttachments[0].getBindingTexture(), slot, mipLevel, layer))
						return false;
				}
				break;
				case GraphicsImageLayout::DepthStencilAttachmentOptimal:
				case GraphicsImageLayout::DepthStencilReadOnlyOptimal:
				{
					const auto& depthStencilAttachment = framebufferDesc.getDepthStencilAttachment();
					if (!depthStencilAttachment.getBindingTexture())
					{
						GL_PLATFORM_LOG("Need depth or stencil texture.");
						return false;
					}

					auto texture = depthStencilAttachment.getBindingTexture();
					auto format = texture->getGraphicsTextureDesc().getTexFormat();
					auto level = depthStencilAttachment.getBindingLevel();
					auto layer = depthStencilAttachment.getBindingLayer();

					if (EGL2Types::isDepthFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT, level, layer))
							return false;
					}
					else if (EGL2Types::isStencilFormat(format))
					{
						if (!this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT, level, layer))
							return false;
					}
					else
					{
						GL_PLATFORM_LOG("Invalid texture format");
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

			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE));

			_framebufferDesc = framebufferDesc;

			return EGL2Check::checkError();
		}

		void
		EGL2Framebuffer::close() noexcept
		{
			if (_fbo != GL_NONE)
			{
				glDeleteFramebuffers(1, &_fbo);
				_fbo = GL_NONE;
			}
		}

		GLuint
		EGL2Framebuffer::getInstanceID() noexcept
		{
			return _fbo;
		}

		bool
		EGL2Framebuffer::bindRenderTexture(GraphicsTexturePtr texture, GLenum attachment, GLint level, GLint layer) noexcept
		{
			assert(texture);

			auto gltexture = texture->downcast<EGL2Texture>();
			auto handle = gltexture->getInstanceID();
			auto target = gltexture->getTarget();

			if (target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP)
			{
				GL_PLATFORM_LOG("Invalid texture target");
				return false;
			}

			GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, handle, 0));

			return true;
		}

		const GraphicsFramebufferDesc&
		EGL2Framebuffer::getGraphicsFramebufferDesc() const noexcept
		{
			return _framebufferDesc;
		}

		void
		EGL2Framebuffer::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2Framebuffer::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}