#include "egl3_texture.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(EGL3Texture, GraphicsTexture, "EGL3Texture")

		EGL3Texture::EGL3Texture() noexcept
			: _texture(GL_NONE)
			, _target(GL_INVALID_ENUM)
			, _pbo(GL_NONE)
			, _pboSize(0)
		{
		}

		EGL3Texture::~EGL3Texture() noexcept
		{
			this->close();
		}

		bool
		EGL3Texture::setup(const GraphicsTextureDesc& textureDesc) noexcept
		{
			assert(_texture == GL_NONE);

			GLenum target = EGL3Types::asTextureTarget(textureDesc.getTexDim());
			if (target == GL_INVALID_ENUM)
				return false;

			GLenum internalFormat = EGL3Types::asTextureInternalFormat(textureDesc.getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
				return false;

			GL_CHECK(glGenTextures(1, &_texture));
			if (_texture == GL_NONE)
			{
				GL_PLATFORM_LOG("glGenTextures() fail");
				return false;
			}

			GL_CHECK(glBindTexture(target, _texture));

			GLsizei width = (GLsizei)textureDesc.getWidth();
			GLsizei height = (GLsizei)textureDesc.getHeight();
			GLsizei depth = (GLsizei)textureDesc.getDepth();

			GLsizei mipBase = textureDesc.getMipBase();
			GLsizei mipLevel = std::max((GLsizei)textureDesc.getMipNums(), 1);

			if (!applySamplerWrap(target, textureDesc.getSamplerWrap()))
				return false;

			if (!applySamplerFilter(target, textureDesc.getSamplerMinFilter()))
				return false;

			if (!applyMipmapLimit(target, mipBase, mipLevel))
				return false;

			if (target == GL_TEXTURE_2D)
				GL_CHECK(glTexStorage2D(target, mipLevel, internalFormat, width, height));
			else if (target == GL_TEXTURE_2D_MULTISAMPLE)
				GL_CHECK(glTexStorage2DMultisample(target, mipLevel, internalFormat, width, height, GL_FALSE));
			else if (target == GL_TEXTURE_2D_ARRAY)
				GL_CHECK(glTexStorage3D(target, mipLevel, internalFormat, width, height, depth));
			else if (target == GL_TEXTURE_3D)
				GL_CHECK(glTexStorage3D(target, mipLevel, internalFormat, width, height, depth));
			else if (target == GL_TEXTURE_CUBE_MAP)
				GL_CHECK(glTexStorage2D(target, mipLevel, internalFormat, width, height));

			auto stream = textureDesc.getStream();
			if (stream)
			{
				if (EGL3Types::isCompressedTexture(textureDesc.getTexFormat()))
				{
					GLsizei offset = 0;

					GLint oldPackStore = 1;
					glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
					glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

					for (GLint mip = mipBase; mip < mipBase + mipLevel; mip++)
					{
						GLsizei w = std::max(width / (1 << mip), 1);
						GLsizei h = std::max(height / (1 << mip), 1);
						GLsizei mipSize = EGL3Types::getCompressedTextureSize(w, h, 1, internalFormat);

						glCompressedTexSubImage2D(target, mip, 0, 0, w, h, internalFormat, mipSize, (char*)stream + offset);

						offset += mipSize;
					}

					glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
				}
				else
				{
					GLenum format = EGL3Types::asTextureFormat(textureDesc.getTexFormat());
					GLenum type = EGL3Types::asTextureType(textureDesc.getTexFormat());

					GLsizei offset = 0;
					GLsizei pixelSize = EGL3Types::getFormatNum(format, type);

					GLenum cubeFace[] =
					{
						GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
						GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
						GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
					};

					GLint oldPackStore = 1;
					glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);

					if (pixelSize == 1)
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					else if (pixelSize == 2)
						glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
					else if (pixelSize == 4 || pixelSize == 12)
						glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
					else if (pixelSize == 8 || pixelSize == 16)
						glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
					else
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

					for (GLsizei mip = mipBase; mip < mipBase + mipLevel; mip++)
					{
						GLsizei w = std::max(width / (1 << mip), 1);
						GLsizei h = std::max(height / (1 << mip), 1);
						GLsizei mipSize = w * h * pixelSize;
						GLsizei layerBase = textureDesc.getLayerBase() + 1;
						GLsizei layerLevel = textureDesc.getLayerNums();

						for (GLsizei layer = layerBase; layer < layerBase + layerLevel; layer++)
						{
							if (target == GL_TEXTURE_2D || target == GL_TEXTURE_2D_MULTISAMPLE)
							{
								GL_CHECK(glTexSubImage2D(target, mip, 0, 0, w, h, format, type, (char*)stream + offset));
								offset += mipSize;
							}
							else
							{
								if (target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_CUBE_MAP_ARRAY)
								{
									for (std::size_t i = 0; i < 6; i++)
									{
										if (target == GL_TEXTURE_CUBE_MAP)
											GL_CHECK(glTexSubImage2D(cubeFace[i], mip, 0, 0, w, h, format, type, (char*)stream + offset));
										else
											GL_CHECK(glTexSubImage3D(cubeFace[i], mip, 0, 0, 0, w, h, layer, format, type, (char*)stream + offset));
										offset += mipSize;
									}
								}
								else
								{
									GL_CHECK(glTexSubImage3D(target, mip, 0, 0, 0, w, h, depth * layer, format, type, (char*)stream + offset));
									offset += mipSize * depth;
								}
							}
						}
					}

					glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
				}
			}

			GL_CHECK(glBindTexture(target, GL_NONE));

			_target = target;
			_textureDesc = textureDesc;

			return EGL3Check::checkError();
		}

		void
		EGL3Texture::close() noexcept
		{
			if (_texture != GL_NONE)
			{
				glDeleteTextures(1, &_texture);
				_texture = GL_NONE;
			}
		}

		GLenum
		EGL3Texture::getTarget() const noexcept
		{
			return _target;
		}

		GLuint
		EGL3Texture::getInstanceID() const noexcept
		{
			return _texture;
		}

		bool
		EGL3Texture::map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept
		{
			assert(data);

			GLenum format = EGL3Types::asTextureFormat(_textureDesc.getTexFormat());
			if (format == GL_INVALID_ENUM)
				return false;

			GLenum type = EGL3Types::asTextureType(_textureDesc.getTexFormat());
			if (type == GL_INVALID_ENUM)
				return false;

			GLsizei num = EGL3Types::getFormatNum(format, type);
			if (num == 0)
				return false;

			if (_pbo == GL_NONE)
				GL_CHECK(glGenBuffers(1, &_pbo));

			GL_CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo));

			GLsizei mapSize = w * h * num;
			if (_pboSize < mapSize)
			{
				GL_CHECK(glBufferData(GL_PIXEL_PACK_BUFFER, mapSize, nullptr, GL_STREAM_READ));
				_pboSize = mapSize;
			}

			GL_CHECK(glBindTexture(_target, _texture));
			GL_CHECK(glReadPixels(x, y, w, h, format, type, 0));

			*data = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, mapSize, GL_MAP_READ_BIT);
			return *data != nullptr;
		}

		void
		EGL3Texture::unmap() noexcept
		{
			GL_CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo));
			GL_CHECK(glUnmapBuffer(GL_PIXEL_PACK_BUFFER));
		}

		bool
		EGL3Texture::applyMipmapLimit(GLenum target, std::uint32_t min, std::uint32_t count) noexcept
		{
			GL_CHECK(glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, min));
			GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, min + count - 1));
			return true;
		}

		bool
		EGL3Texture::applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept
		{
			GLenum glwrap = EGL3Types::asSamplerWrap(wrap);
			if (glwrap != GL_INVALID_ENUM)
			{
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, glwrap));
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, glwrap));
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_R, glwrap));

				return true;
			}

			return false;
		}

		bool
		EGL3Texture::applySamplerFilter(GLenum target, GraphicsSamplerFilter filter) noexcept
		{
			GLenum min = EGL3Types::asSamplerMinFilter(filter);
			GLenum mag = EGL3Types::asSamplerMagFilter(filter);

			if (min != GL_INVALID_ENUM && mag != GL_INVALID_ENUM)
			{
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min));
				GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag));
				return true;
			}

			return false;
		}

		const GraphicsTextureDesc&
		EGL3Texture::getGraphicsTextureDesc() const noexcept
		{
			return _textureDesc;
		}

		void
		EGL3Texture::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL3Texture::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}