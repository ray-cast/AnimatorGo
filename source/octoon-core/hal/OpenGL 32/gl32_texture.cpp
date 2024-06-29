#include "gl32_texture.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL32Texture, GraphicsTexture, "GL32Texture")

		GL32Texture::GL32Texture() noexcept
			: _texture(GL_NONE)
			, _target(GL_INVALID_ENUM)
			, _pbo(GL_NONE)
			, _pboSize(0)
		{
		}

		GL32Texture::~GL32Texture() noexcept
		{
			this->close();
		}

		bool
		GL32Texture::setup(const GraphicsTextureDesc& textureDesc) noexcept
		{
			assert(_texture == GL_NONE);

			GLenum target = GL32Types::asTextureTarget(textureDesc.getTexDim());
			if (target == GL_INVALID_ENUM)
				return false;

			GLenum internalFormat = GL32Types::asTextureInternalFormat(textureDesc.getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
				return false;

			glGenTextures(1, &_texture);
			if (_texture == GL_NONE)
			{
				GL_PLATFORM_LOG("glGenTextures() fail");
				return false;
			}

			glBindTexture(target, _texture);

			GLsizei width = (GLsizei)textureDesc.getWidth();
			GLsizei height = (GLsizei)textureDesc.getHeight();
			GLsizei depth = (GLsizei)textureDesc.getDepth();

			GLsizei mipBase = textureDesc.getMipBase();
			GLsizei mipLevel = std::max((GLsizei)textureDesc.getMipNums(), 1);

			if (target == GL_TEXTURE_2D)
				glTexStorage2D(target, mipLevel, internalFormat, width, height);
			else if (target == GL_TEXTURE_2D_MULTISAMPLE)
				glTexStorage2DMultisample(target, textureDesc.getTexMultisample(), internalFormat, width, height, GL_FALSE);
			else if (target == GL_TEXTURE_2D_ARRAY)
				glTexStorage3D(target, mipLevel, internalFormat, width, height, depth);
			else if (target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
				glTexStorage3DMultisample(target, mipLevel, internalFormat, width, height, textureDesc.getLayerNums(), GL_FALSE);
			else if (target == GL_TEXTURE_3D)
				glTexStorage3D(target, mipLevel, internalFormat, width, height, depth);
			else if (target == GL_TEXTURE_CUBE_MAP)
				glTexStorage2D(target, mipLevel, internalFormat, width, height);

			auto stream = textureDesc.getStream();
			if (stream)
			{
				if (GL32Types::isCompressedTexture(textureDesc.getTexFormat()))
				{
					GLsizei offset = 0;

					GLint oldPackStore = 1;
					glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
					glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

					for (GLint mip = mipBase; mip < mipBase + mipLevel; mip++)
					{
						GLsizei w = std::max(width / (1 << mip), 1);
						GLsizei h = std::max(height / (1 << mip), 1);
						GLsizei mipSize = GL32Types::getCompressedTextureSize(w, h, 1, internalFormat);

						glCompressedTexSubImage2D(target, mip, 0, 0, w, h, internalFormat, mipSize, (char*)stream + offset);

						offset += mipSize;
					}

					glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
				}
				else
				{
					GLenum format = GL32Types::asTextureFormat(textureDesc.getTexFormat());
					GLenum type = GL32Types::asTextureType(textureDesc.getTexFormat());

					GLsizei offset = 0;
					GLsizei pixelSize = GL32Types::getFormatNum(format, type);

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
								glTexSubImage2D(target, mip, 0, 0, w, h, format, type, (char*)stream + offset);
								offset += mipSize;
							}
							else
							{
								if (target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_CUBE_MAP_ARRAY)
								{
									for (std::size_t i = 0; i < 6; i++)
									{
										if (target == GL_TEXTURE_CUBE_MAP)
											glTexSubImage2D(cubeFace[i], mip, 0, 0, w, h, format, type, (char*)stream + offset);
										else
											glTexSubImage3D(cubeFace[i], mip, 0, 0, 0, w, h, layer, format, type, (char*)stream + offset);
										offset += mipSize;
									}
								}
								else
								{
									glTexSubImage3D(target, mip, 0, 0, 0, w, h, depth * layer, format, type, (char*)stream + offset);
									offset += mipSize * depth;
								}
							}
						}
					}

					glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
				}
			}

			glBindTexture(target, GL_NONE);

			_target = target;
			_textureDesc = textureDesc;

			return GL32Check::checkError();
		}

		void
		GL32Texture::close() noexcept
		{
			if (_texture != GL_NONE)
			{
				glDeleteTextures(1, &_texture);
				_texture = GL_NONE;
			}
		}

		GLenum
		GL32Texture::getTarget() const noexcept
		{
			return _target;
		}

		GLuint
		GL32Texture::getInstanceID() const noexcept
		{
			return _texture;
		}

		bool
		GL32Texture::map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept
		{
			assert(data);

			GLenum format = GL32Types::asTextureFormat(_textureDesc.getTexFormat());
			if (format == GL_INVALID_ENUM)
				return false;

			GLenum type = GL32Types::asTextureType(_textureDesc.getTexFormat());
			if (type == GL_INVALID_ENUM)
				return false;

			GLsizei num = GL32Types::getFormatNum(format, type);
			if (num == 0)
				return false;

			if (_pbo == GL_NONE)
				glGenBuffers(1, &_pbo);

			glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo);

			GLsizei mapSize = w * h * num;
			if (_pboSize < mapSize)
			{
				glBufferData(GL_PIXEL_PACK_BUFFER, mapSize, nullptr, GL_STREAM_READ);
				_pboSize = mapSize;
			}

			glBindTexture(_target, _texture);
			glReadPixels(x, y, w, h, format, type, 0);

			*data = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, mapSize, GL_MAP_READ_BIT);
			return *data != nullptr;
		}

		void
		GL32Texture::unmap() noexcept
		{
			glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo);
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		}

		bool
		GL32Texture::applyMipmapLimit(GLenum target, std::uint32_t min, std::uint32_t count) noexcept
		{
			glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, min);
			glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, min + count - 1);
			return true;
		}

		bool
		GL32Texture::applySamplerWrap(GLenum target, SamplerWrap wrap) noexcept
		{
			GLenum glwrap = GL32Types::asSamplerWrap(wrap);
			if (glwrap != GL_INVALID_ENUM)
			{
				glTexParameteri(target, GL_TEXTURE_WRAP_S, glwrap);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, glwrap);
				glTexParameteri(target, GL_TEXTURE_WRAP_R, glwrap);

				return true;
			}

			return false;
		}

		bool
		GL32Texture::applySamplerFilter(GLenum target, SamplerFilter filter) noexcept
		{
			GLenum min = GL32Types::asSamplerMinFilter(filter);
			GLenum mag = GL32Types::asSamplerMagFilter(filter);

			if (min != GL_INVALID_ENUM && mag != GL_INVALID_ENUM)
			{
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min);
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag);
				return true;
			}

			return false;
		}

		const std::uint64_t
		GL32Texture::handle() const noexcept
		{
			return _texture;
		}

		const GraphicsTextureDesc&
		GL32Texture::getTextureDesc() const noexcept
		{
			return _textureDesc;
		}

		void
		GL32Texture::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL32Texture::getDevice() const noexcept
		{
			return _device.lock();
		}
	}
}