#include "gl45_texture.h"
#include "gl33_device.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL45Texture, GraphicsTexture, "GL45Texture")

		GL45Texture::GL45Texture() noexcept
			: _texture(GL_NONE)
			, _target(GL_INVALID_ENUM)
			, _pbo(GL_NONE)
			, _pboSize(0)
		{
		}

		GL45Texture::~GL45Texture() noexcept
		{
			this->close();
		}

		bool
		GL45Texture::setup(const GraphicsTextureDesc& textureDesc) noexcept
		{
			assert(_texture == GL_NONE);

			GLenum target = GL33Types::asTextureTarget(textureDesc.getTexDim());
			if (target == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid texture target.");
				return false;
			}

			GLenum internalFormat = GL33Types::asTextureInternalFormat(textureDesc.getTexFormat());
			if (internalFormat == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid texture internal format.");
				return false;
			}

			glCreateTextures(target, 1, &_texture);
			if (_texture == GL_NONE)
			{
				this->getDevice()->downcast<GL33Device>()->message("glCreateTextures fail");
				return false;
			}

			GLsizei width = (GLsizei)textureDesc.getWidth();
			GLsizei height = (GLsizei)textureDesc.getHeight();
			GLsizei depth = (GLsizei)textureDesc.getDepth();

			GLsizei mipBase = textureDesc.getMipBase();
			GLsizei mipLevel = textureDesc.getMipNums();

			if (target != GL_TEXTURE_2D_MULTISAMPLE && target != GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
			{
				if (!applyMipmapLimit(mipBase, mipLevel))
					return false;
			}

			if (target == GL_TEXTURE_2D)
				glTextureStorage2D(_texture, mipLevel, internalFormat, width, height);
			else if (target == GL_TEXTURE_2D_MULTISAMPLE)
				glTextureStorage2DMultisample(_texture, mipLevel, internalFormat, width, height, GL_FALSE);
			else if (target == GL_TEXTURE_2D_ARRAY)
				glTextureStorage3D(_texture, mipLevel, internalFormat, width, height, depth);
			else if (target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
				glTextureStorage3DMultisample(_texture, mipLevel, internalFormat, width, height, depth, GL_FALSE);
			else if (target == GL_TEXTURE_3D)
				glTextureStorage3D(_texture, mipLevel, internalFormat, width, height, depth);
			else if (target == GL_TEXTURE_CUBE_MAP)
				glTextureStorage2D(_texture, mipLevel, internalFormat, width, height);
			else if (target == GL_TEXTURE_CUBE_MAP_ARRAY)
				glTextureStorage3D(_texture, mipLevel, internalFormat, width, height, depth);

			auto stream = textureDesc.getStream();
			if (stream)
			{
				if (GL33Types::isCompressedTexture(textureDesc.getTexFormat()))
				{
					GLsizei offset = 0;
					GLsizei blockSize = internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

					GLint oldPackStore = 1;
					glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
					glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

					for (GLint mip = mipBase; mip < mipBase + mipLevel; mip++)
					{
						GLsizei w = std::max(width / (1 << mip), 1);
						GLsizei h = std::max(height / (1 << mip), 1);
						GLsizei mipSize = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

						glCompressedTextureSubImage2D(_texture, mip, 0, 0, w, h, internalFormat, mipSize, (char*)stream + offset);

						offset += mipSize;
					}

					glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
				}
				else
				{
					GLenum format = GL33Types::asTextureFormat(textureDesc.getTexFormat());
					if (format == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<GL33Device>()->message("Invalid texture format");
						return false;
					}

					GLenum type = GL33Types::asTextureType(textureDesc.getTexFormat());
					if (type == GL_INVALID_ENUM)
					{
						this->getDevice()->downcast<GL33Device>()->message("Invalid texture type");
						return false;
					}

					GLsizei offset = 0;
					GLsizei pixelSize = GL33Types::getFormatNum(format, type);

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

					if (target == GL_TEXTURE_2D_MULTISAMPLE || target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
					{
						if (target == GL_TEXTURE_2D_MULTISAMPLE)
							glTextureStorage2DMultisample(_texture, textureDesc.getTexMultisample(), internalFormat, width, height, GL_FALSE);
						else
						{
							GLsizei layerBase = textureDesc.getLayerBase() + 1;
							GLsizei layerLevel = textureDesc.getLayerNums();

							for (GLsizei layer = layerBase; layer < layerBase + layerLevel; layer++)
								glTextureStorage3DMultisample(_texture, textureDesc.getTexMultisample(), internalFormat, width, height, layer, GL_FALSE);
						}
					}
					else
					{
						for (GLsizei mip = mipBase; mip < mipBase + mipLevel; mip++)
						{
							GLsizei w = std::max(width / (1 << mip), 1);
							GLsizei h = std::max(height / (1 << mip), 1);
							GLsizei mipSize = w * h * pixelSize;
							GLsizei layerBase = textureDesc.getLayerBase() + 1;
							GLsizei layerLevel = textureDesc.getLayerNums();

							for (GLsizei layer = layerBase; layer < layerBase + layerLevel; layer++)
							{
								if (target == GL_TEXTURE_2D)
								{
									glTextureSubImage2D(_texture, mip, 0, 0, w, h, format, type, (char*)stream + offset);
									offset += mipSize;
								}
								else
								{
									if (target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_CUBE_MAP_ARRAY)
									{
										glTextureSubImage3D(_texture, mip, 0, 0, 0, w, h, 6 * layer, format, type, (char*)stream + offset);
										offset += mipSize * 6;
									}
									else
									{
										glTextureSubImage3D(_texture, mip, 0, 0, 0, w, h, depth * layer, format, type, (char*)stream + offset);
										offset += mipSize * depth;
									}
								}
							}
						}
					}

					glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
				}
			}

			_target = target;
			_textureDesc = textureDesc;

			return GL33Check::checkError();
		}

		void
		GL45Texture::close() noexcept
		{
			if (_texture != GL_NONE)
			{
				glDeleteTextures(1, &_texture);
				_texture = GL_NONE;
			}

			if (_pbo != GL_NONE)
			{
				glDeleteBuffers(1, &_pbo);
				_pbo = GL_NONE;
			}

			if (_upbo != GL_NONE)
			{
				glDeleteBuffers(1, &_upbo);
				_upbo = GL_NONE;
			}
		}

		bool
		GL45Texture::map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept
		{
			assert(data);

			GLenum format = GL33Types::asTextureFormat(_textureDesc.getTexFormat());
			if (format == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid texture format");
				return false;
			}

			GLenum type = GL33Types::asTextureType(_textureDesc.getTexFormat());
			if (type == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid texture type");
				return false;
			}

			if (type == GL_HALF_FLOAT)
				type = GL_FLOAT;

			GLsizei num = GL33Types::getFormatNum(format, type);
			if (num == 0)
				return false;

			if (_pbo == GL_NONE)
				glGenBuffers(1, &_pbo);

			glBindBuffer(GL_PIXEL_PACK_BUFFER, _pbo);

			GLuint mapSize = w * h * num;
			if (_pboSize < mapSize)
			{
				glBufferData(GL_PIXEL_PACK_BUFFER, mapSize, nullptr, GL_STREAM_READ);
				_pboSize = mapSize;
			}

			glGetTextureSubImage(_texture, mipLevel, x, y, 0, w, h, 0, format, type, mapSize, 0);

			*data = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, mapSize, GL_MAP_READ_BIT);
			return *data ? true : false;
		}

		void
		GL45Texture::unmap() noexcept
		{
			glUnmapNamedBuffer(_pbo);
		}

		GLenum
		GL45Texture::getTarget() const noexcept
		{
			return _target;
		}

		GLuint
		GL45Texture::getInstanceID() const noexcept
		{
			return _texture;
		}

		bool
		GL45Texture::applyMipmapLimit(std::uint32_t min, std::uint32_t count) noexcept
		{
			glTextureParameteri(_texture, GL_TEXTURE_BASE_LEVEL, min);
			glTextureParameteri(_texture, GL_TEXTURE_MAX_LEVEL, min + count - 1);
			return true;
		}

		bool
		GL45Texture::applySamplerWrap(SamplerWrap wrap) noexcept
		{
			GLenum glwrap = GL33Types::asSamplerWrap(wrap);
			if (glwrap == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid sampler wrap");
				return false;
			}

			glTextureParameteri(_texture, GL_TEXTURE_WRAP_S, glwrap);
			glTextureParameteri(_texture, GL_TEXTURE_WRAP_T, glwrap);
			glTextureParameteri(_texture, GL_TEXTURE_WRAP_R, glwrap);

			return true;
		}

		bool
		GL45Texture::applySamplerFilter(SamplerFilter minFilter, SamplerFilter magFilter) noexcept
		{
			GLenum min = GL33Types::asSamplerMinFilter(minFilter);
			GLenum mag = GL33Types::asSamplerMagFilter(magFilter);
			if (min == GL_INVALID_ENUM || mag == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid sampler filter");
				return false;
			}

			glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, min);
			glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, mag);
			return true;
		}

		bool
		GL45Texture::applySamplerAnis(GraphicsSamplerAnis anis) noexcept
		{
			if (anis == GraphicsSamplerAnis::Anis1)
				glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
			else if (anis == GraphicsSamplerAnis::Anis2)
				glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
			else if (anis == GraphicsSamplerAnis::Anis4)
				glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
			else if (anis == GraphicsSamplerAnis::Anis8)
				glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
			else if (anis == GraphicsSamplerAnis::Anis16)
				glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
			else if (anis == GraphicsSamplerAnis::Anis32)
				glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 32);
			else if (anis == GraphicsSamplerAnis::Anis64)
				glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 64);
			else if (anis != GraphicsSamplerAnis::Anis0)
			{
				this->getDevice()->downcast<GL33Device>()->message("Can't support format with anisotropy.");
				return false;
			}

			return true;
		}

		const std::uint64_t
		GL45Texture::handle() const noexcept
		{
			return _texture;
		}

		const GraphicsTextureDesc&
		GL45Texture::getTextureDesc() const noexcept
		{
			return _textureDesc;
		}

		void
		GL45Texture::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL45Texture::getDevice() const noexcept
		{
			return _device.lock();
		}
	}
}