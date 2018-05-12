#include "gl20_types.h"
#include <cstring>
#include <cstdarg>

namespace octoon
{
	namespace graphics
	{
		GLboolean GL20Types::_gl20init = GL_FALSE;
		GLboolean GL20Types::_gl20Features[GL20_RangeSize];

		bool
		GL20Types::setup() noexcept
		{
			if (_gl20init)
				return true;

			std::memset(_gl20Features, GL_FALSE, sizeof(_gl20Features));

			const GLubyte* extension = glGetString(GL_EXTENSIONS);
			if (!extension)
				return false;

			std::size_t extensionLength = std::strlen((const char*)extension);
			if (extensionLength == 0)
				return false;

			const char* offset = (char*)extension;
			for (;;)
			{
				const char* pos = std::strstr(offset, " ");
				if (!pos)
				{
					std::size_t length = std::strlen(offset);
					if (length == 0)
						break;

					pos = offset + length;
				}

				std::intptr_t length = pos - offset;
				if (length < 0)
					return false;

				const char* src = pos - length;

				if (strncmp(src, "GL_OES_required_internalformat", length) == 0)		   _gl20Features[GL20Features::GL20_OES_required_internalformat] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_read_format_bgra", length) == 0)             _gl20Features[GL20Features::GL20_EXT_read_format_bgra] = GL_TRUE;
				else if (strncmp(src, "GL_ARM_rgba8", length) == 0)                        _gl20Features[GL20Features::GL20_ARM_rgba8] = GL_TRUE;
				else if (strncmp(src, "GL_OES_rgb8_rgba8", length) == 0)                   _gl20Features[GL20Features::GL20_OES_rgb8_rgba8] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_texture_format_BGRA8888", length) == 0)      _gl20Features[GL20Features::GL20_EXT_texture_format_BGRA8888] = GL_TRUE;
				else if (strncmp(src, "GL_APPLE_texture_format_BGRA8888", length) == 0)    _gl20Features[GL20Features::GL20_APPLE_texture_format_BGRA8888] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_sRGB", length) == 0)                         _gl20Features[GL20Features::GL20_EXT_sRGB] = GL_TRUE;
				else if (strncmp(src, "GL_NV_sRGB_formats", length) == 0)                  _gl20Features[GL20Features::GL20_NV_sRGB_formats] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_texture_rg", length) == 0)                   _gl20Features[GL20Features::GL20_EXT_texture_rg] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_texture_norm16", length) == 0)               _gl20Features[GL20Features::GL20_EXT_texture_norm16] = GL_TRUE;
				else if (strncmp(src, "GL_OES_texture_half_float", length) == 0)           _gl20Features[GL20Features::GL20_OES_texture_half_float] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_color_buffer_half_float", length) == 0)      _gl20Features[GL20Features::GL20_EXT_color_buffer_half_float] = GL_TRUE;
				else if (strncmp(src, "GL_OES_texture_float", length) == 0)                _gl20Features[GL20Features::GL20_OES_texture_float] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_color_buffer_float", length) == 0)           _gl20Features[GL20Features::GL20_EXT_color_buffer_float] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_texture_type_2_10_10_10_REV", length) == 0)  _gl20Features[GL20Features::GL20_EXT_texture_type_2_10_10_10_REV] = GL_TRUE;
				else if (strncmp(src, "GL_APPLE_texture_packed_float", length) == 0)       _gl20Features[GL20Features::GL20_APPLE_texture_packed_float] = GL_TRUE;
				else if (strncmp(src, "GL_OES_depth_texture", length) == 0)                _gl20Features[GL20Features::GL20_OES_depth_texture] = GL_TRUE;
				else if (strncmp(src, "GL_OES_depth24", length) == 0)                      _gl20Features[GL20Features::GL20_OES_depth24] = GL_TRUE;
				else if (strncmp(src, "GL_OES_depth32", length) == 0)                      _gl20Features[GL20Features::GL20_OES_depth32] = GL_TRUE;
				else if (strncmp(src, "GL_OES_texture_stencil8", length) == 0)             _gl20Features[GL20Features::GL20_OES_texture_stencil8] = GL_TRUE;
				else if (strncmp(src, "GL_OES_packed_depth_stencil", length) == 0)         _gl20Features[GL20Features::GL20_OES_packed_depth_stencil] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_texture_compression_dxt1", length) == 0)     _gl20Features[GL20Features::GL20_EXT_texture_compression_dxt1] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_texture_compression_s3tc", length) == 0)     _gl20Features[GL20Features::GL20_EXT_texture_compression_s3tc] = GL_TRUE;
				else if (strncmp(src, "GL_KHR_texture_compression_astc_ldr", length) == 0) _gl20Features[GL20Features::GL20_KHR_texture_compression_astc_ldr] = GL_TRUE;
				else if (strncmp(src, "GL_OES_vertex_type_10_10_10_2", length) == 0)       _gl20Features[GL20Features::GL20_OES_vertex_type_10_10_10_2] = GL_TRUE;
				else if (strncmp(src, "GL_OES_vertex_half_float", length) == 0)            _gl20Features[GL20Features::GL20_OES_vertex_half_float] = GL_TRUE;
				else if (strncmp(src, "GL_EXT_texture_filter_anisotropic", length) == 0)   _gl20Features[GL20Features::GL20_EXT_texture_filter_anisotropic] = GL_TRUE;
				else if (strncmp(src, "GL_KHR_debug", length) == 0)                        _gl20Features[GL20Features::GL20_KHR_debug] = GL_TRUE;

				offset = pos + 1;
			}

			_gl20init = true;
			return true;
		}

		GLenum
		GL20Types::asVertexType(GraphicsVertexType type) noexcept
		{
			switch (type)
			{
			case GraphicsVertexType::PointList:     return GL_POINTS;
			case GraphicsVertexType::LineList:      return GL_LINES;
			case GraphicsVertexType::LineStrip:     return GL_LINE_STRIP;
			case GraphicsVertexType::TriangleList:  return GL_TRIANGLES;
			case GraphicsVertexType::TriangleStrip: return GL_TRIANGLE_STRIP;
			case GraphicsVertexType::TriangleFan:   return GL_TRIANGLE_FAN;
			default:
				GL_PLATFORM_LOG("Invalid vertex type");
				return false;
			}
		}

		GLenum
		GL20Types::asVertexFormat(GraphicsFormat format) noexcept
		{
			switch (format)
			{
			case GraphicsFormat::R8SNorm:
			case GraphicsFormat::R8SScaled:
			case GraphicsFormat::R8SInt:
			case GraphicsFormat::R8SRGB:
			case GraphicsFormat::R8G8SNorm:
			case GraphicsFormat::R8G8SScaled:
			case GraphicsFormat::R8G8SInt:
			case GraphicsFormat::R8G8SRGB:
			case GraphicsFormat::R8G8B8SNorm:
			case GraphicsFormat::R8G8B8SScaled:
			case GraphicsFormat::R8G8B8SInt:
			case GraphicsFormat::R8G8B8SRGB:
			case GraphicsFormat::B8G8R8SNorm:
			case GraphicsFormat::B8G8R8SScaled:
			case GraphicsFormat::B8G8R8SInt:
			case GraphicsFormat::B8G8R8SRGB:
			case GraphicsFormat::R8G8B8A8SNorm:
			case GraphicsFormat::R8G8B8A8SScaled:
			case GraphicsFormat::R8G8B8A8SInt:
			case GraphicsFormat::R8G8B8A8SRGB:
			case GraphicsFormat::B8G8R8A8SNorm:
			case GraphicsFormat::B8G8R8A8SScaled:
			case GraphicsFormat::B8G8R8A8SInt:
			case GraphicsFormat::B8G8R8A8SRGB:
			case GraphicsFormat::A8B8G8R8SNormPack32:
			case GraphicsFormat::A8B8G8R8SScaledPack32:
			case GraphicsFormat::A8B8G8R8SIntPack32:
			case GraphicsFormat::A8B8G8R8SRGBPack32:
				return GL_BYTE;
			case GraphicsFormat::R8UNorm:
			case GraphicsFormat::R8UScaled:
			case GraphicsFormat::R8UInt:
			case GraphicsFormat::R8G8UNorm:
			case GraphicsFormat::R8G8UScaled:
			case GraphicsFormat::R8G8UInt:
			case GraphicsFormat::R8G8B8UNorm:
			case GraphicsFormat::R8G8B8UScaled:
			case GraphicsFormat::R8G8B8UInt:
			case GraphicsFormat::B8G8R8UNorm:
			case GraphicsFormat::B8G8R8UScaled:
			case GraphicsFormat::B8G8R8UInt:
			case GraphicsFormat::R8G8B8A8UNorm:
			case GraphicsFormat::R8G8B8A8UScaled:
			case GraphicsFormat::R8G8B8A8UInt:
			case GraphicsFormat::B8G8R8A8UNorm:
			case GraphicsFormat::B8G8R8A8UScaled:
			case GraphicsFormat::B8G8R8A8UInt:
			case GraphicsFormat::A8B8G8R8UNormPack32:
			case GraphicsFormat::A8B8G8R8UScaledPack32:
			case GraphicsFormat::A8B8G8R8UIntPack32:
				return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R16SNorm:
			case GraphicsFormat::R16SScaled:
			case GraphicsFormat::R16SInt:
			case GraphicsFormat::R16SFloat:
			case GraphicsFormat::R16G16SNorm:
			case GraphicsFormat::R16G16SScaled:
			case GraphicsFormat::R16G16SInt:
			case GraphicsFormat::R16G16SFloat:
			case GraphicsFormat::R16G16B16SNorm:
			case GraphicsFormat::R16G16B16SScaled:
			case GraphicsFormat::R16G16B16SInt:
			case GraphicsFormat::R16G16B16SFloat:
			case GraphicsFormat::R16G16B16A16SNorm:
			case GraphicsFormat::R16G16B16A16SScaled:
			case GraphicsFormat::R16G16B16A16SInt:
			case GraphicsFormat::R16G16B16A16SFloat:
				return GL_SHORT;
			case GraphicsFormat::R16UNorm:
			case GraphicsFormat::R16UScaled:
			case GraphicsFormat::R16UInt:
			case GraphicsFormat::R16G16UNorm:
			case GraphicsFormat::R16G16UScaled:
			case GraphicsFormat::R16G16UInt:
			case GraphicsFormat::R16G16B16UNorm:
			case GraphicsFormat::R16G16B16UScaled:
			case GraphicsFormat::R16G16B16UInt:
			case GraphicsFormat::R16G16B16A16UNorm:
			case GraphicsFormat::R16G16B16A16UScaled:
			case GraphicsFormat::R16G16B16A16UInt:
				return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R32SInt:
			case GraphicsFormat::R32G32SInt:
			case GraphicsFormat::R32G32B32SInt:
			case GraphicsFormat::R32G32B32A32SInt:
				return GL_INT;
			case GraphicsFormat::R32UInt:
			case GraphicsFormat::R32G32UInt:
			case GraphicsFormat::R32G32B32UInt:
			case GraphicsFormat::R32G32B32A32UInt:
				return GL_UNSIGNED_INT;
			case GraphicsFormat::R32SFloat:
			case GraphicsFormat::R32G32SFloat:
			case GraphicsFormat::R32G32B32SFloat:
			case GraphicsFormat::R32G32B32A32SFloat:
				return GL_FLOAT;
			case GraphicsFormat::R64SInt:
			case GraphicsFormat::R64G64SInt:
			case GraphicsFormat::R64G64B64SInt:
			case GraphicsFormat::R64G64B64A64SInt:
			case GraphicsFormat::R64UInt:
			case GraphicsFormat::R64G64UInt:
			case GraphicsFormat::R64G64B64UInt:
			case GraphicsFormat::R64G64B64A64UInt:
			case GraphicsFormat::R64SFloat:
			case GraphicsFormat::R64G64SFloat:
			case GraphicsFormat::R64G64B64SFloat:
			case GraphicsFormat::R64G64B64A64SFloat:
				GL_PLATFORM_LOG("Can't support format");
				return GL_INVALID_ENUM;
			default:
				return GL_INVALID_ENUM;
				break;
			}
		}

		GLenum
		GL20Types::asIndexType(GraphicsIndexType type) noexcept
		{
			switch (type)
			{
			case GraphicsIndexType::UInt16: return GL_UNSIGNED_SHORT;
			case GraphicsIndexType::UInt32: return GL_UNSIGNED_INT;
			default:
				GL_PLATFORM_LOG("Invalid index type");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asShaderStage(GraphicsShaderStageFlags stage) noexcept
		{
			switch (stage)
			{
			case GraphicsShaderStageFlagBits::VertexBit:         return GL_VERTEX_SHADER;
			case GraphicsShaderStageFlagBits::FragmentBit:       return GL_FRAGMENT_SHADER;
			case GraphicsShaderStageFlagBits::ComputeBit:
			case GraphicsShaderStageFlagBits::GeometryBit:
			case GraphicsShaderStageFlagBits::TessControlBit:
			case GraphicsShaderStageFlagBits::TessEvaluationBit:
				GL_PLATFORM_LOG("Can't support shader stage");
			default:
				GL_PLATFORM_LOG("Invalid shader type");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asTextureTarget(GraphicsTextureDim target) noexcept
		{
			switch (target)
			{
			case GraphicsTextureDim::Texture2D:					return GL_TEXTURE_2D;
			case GraphicsTextureDim::Cube:						return GL_TEXTURE_CUBE_MAP;
			case GraphicsTextureDim::Texture2DMultisample:
			case GraphicsTextureDim::Texture2DArrayMultisample:
			case GraphicsTextureDim::Texture2DArray:
			case GraphicsTextureDim::Texture3D:
			case GraphicsTextureDim::CubeArray:
			default:
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asTextureFormat(GraphicsFormat format) noexcept
		{
			switch (format)
			{
			case GraphicsFormat::R8UNorm:
			case GraphicsFormat::R8SNorm:
			case GraphicsFormat::R8UScaled:
			case GraphicsFormat::R8SScaled:
			case GraphicsFormat::R8UInt:
			case GraphicsFormat::R8SInt:
			case GraphicsFormat::R16UNorm:
			case GraphicsFormat::R16SNorm:
			case GraphicsFormat::R16UScaled:
			case GraphicsFormat::R16SScaled:
			case GraphicsFormat::R16UInt:
			case GraphicsFormat::R16SInt:
			case GraphicsFormat::R16SFloat:
			case GraphicsFormat::R32UInt:
			case GraphicsFormat::R32SInt:
			case GraphicsFormat::R32SFloat:
			case GraphicsFormat::R64UInt:
			case GraphicsFormat::R64SInt:
			case GraphicsFormat::R64SFloat:
				return GL_INVALID_ENUM;
			case GraphicsFormat::R4G4UNormPack8:
			case GraphicsFormat::R8G8UNorm:
			case GraphicsFormat::R8G8SNorm:
			case GraphicsFormat::R8G8UScaled:
			case GraphicsFormat::R8G8SScaled:
			case GraphicsFormat::R8G8UInt:
			case GraphicsFormat::R8G8SInt:
			case GraphicsFormat::R16G16UNorm:
			case GraphicsFormat::R16G16SNorm:
			case GraphicsFormat::R16G16UScaled:
			case GraphicsFormat::R16G16SScaled:
			case GraphicsFormat::R16G16UInt:
			case GraphicsFormat::R16G16SInt:
			case GraphicsFormat::R16G16SFloat:
			case GraphicsFormat::R32G32UInt:
			case GraphicsFormat::R32G32SInt:
			case GraphicsFormat::R32G32SFloat:
			case GraphicsFormat::R64G64UInt:
			case GraphicsFormat::R64G64SInt:
			case GraphicsFormat::R64G64SFloat:
				return GL_INVALID_ENUM;
			case GraphicsFormat::R5G6B5UNormPack16:
			case GraphicsFormat::R8G8B8UNorm:
			case GraphicsFormat::R8G8B8SNorm:
			case GraphicsFormat::R8G8B8UScaled:
			case GraphicsFormat::R8G8B8SScaled:
			case GraphicsFormat::R8G8B8UInt:
			case GraphicsFormat::R8G8B8SInt:
			case GraphicsFormat::R16G16B16UNorm:
			case GraphicsFormat::R16G16B16SNorm:
			case GraphicsFormat::R16G16B16UScaled:
			case GraphicsFormat::R16G16B16SScaled:
			case GraphicsFormat::R16G16B16UInt:
			case GraphicsFormat::R16G16B16SInt:
			case GraphicsFormat::R16G16B16SFloat:
			case GraphicsFormat::R32G32B32UInt:
			case GraphicsFormat::R32G32B32SInt:
			case GraphicsFormat::R32G32B32SFloat:
			case GraphicsFormat::R64G64B64UInt:
			case GraphicsFormat::R64G64B64SInt:
			case GraphicsFormat::R64G64B64SFloat:
				return GL_RGB;
			case GraphicsFormat::B5G6R5UNormPack16:
			case GraphicsFormat::B8G8R8UNorm:
			case GraphicsFormat::B8G8R8SNorm:
			case GraphicsFormat::B8G8R8UScaled:
			case GraphicsFormat::B8G8R8SScaled:
			case GraphicsFormat::B8G8R8UInt:
			case GraphicsFormat::B8G8R8SInt:
			case GraphicsFormat::B10G11R11UFloatPack32:
				return GL_INVALID_ENUM;
			case GraphicsFormat::R4G4B4A4UNormPack16:
			case GraphicsFormat::R5G5B5A1UNormPack16:
			case GraphicsFormat::A1R5G5B5UNormPack16:
			case GraphicsFormat::R8G8B8A8UNorm:
			case GraphicsFormat::R8G8B8A8SNorm:
			case GraphicsFormat::R8G8B8A8UScaled:
			case GraphicsFormat::R8G8B8A8SScaled:
			case GraphicsFormat::R8G8B8A8UInt:
			case GraphicsFormat::R8G8B8A8SInt:
			case GraphicsFormat::A2R10G10B10UNormPack32:
			case GraphicsFormat::A2R10G10B10SNormPack32:
			case GraphicsFormat::A2R10G10B10UScaledPack32:
			case GraphicsFormat::A2R10G10B10SScaledPack32:
			case GraphicsFormat::A2R10G10B10UIntPack32:
			case GraphicsFormat::A2R10G10B10SIntPack32:
			case GraphicsFormat::R16G16B16A16UNorm:
			case GraphicsFormat::R16G16B16A16SNorm:
			case GraphicsFormat::R16G16B16A16UScaled:
			case GraphicsFormat::R16G16B16A16SScaled:
			case GraphicsFormat::R16G16B16A16UInt:
			case GraphicsFormat::R16G16B16A16SInt:
			case GraphicsFormat::R16G16B16A16SFloat:
			case GraphicsFormat::R32G32B32A32UInt:
			case GraphicsFormat::R32G32B32A32SInt:
			case GraphicsFormat::R32G32B32A32SFloat:
			case GraphicsFormat::R64G64B64A64UInt:
			case GraphicsFormat::R64G64B64A64SInt:
			case GraphicsFormat::R64G64B64A64SFloat:
				return GL_RGBA;
			case GraphicsFormat::B4G4R4A4UNormPack16:
			case GraphicsFormat::B5G5R5A1UNormPack16:
			case GraphicsFormat::B8G8R8A8UNorm:
			case GraphicsFormat::B8G8R8A8SNorm:
			case GraphicsFormat::B8G8R8A8UScaled:
			case GraphicsFormat::B8G8R8A8SScaled:
			case GraphicsFormat::B8G8R8A8UInt:
			case GraphicsFormat::B8G8R8A8SInt:
			case GraphicsFormat::A2B10G10R10UNormPack32:
			case GraphicsFormat::A2B10G10R10SNormPack32:
			case GraphicsFormat::A2B10G10R10UScaledPack32:
			case GraphicsFormat::A2B10G10R10SScaledPack32:
			case GraphicsFormat::A2B10G10R10UIntPack32:
			case GraphicsFormat::A2B10G10R10SIntPack32:
				return GL_INVALID_ENUM;
			case GraphicsFormat::A8B8G8R8UNormPack32:
			case GraphicsFormat::A8B8G8R8SNormPack32:
			case GraphicsFormat::A8B8G8R8UScaledPack32:
			case GraphicsFormat::A8B8G8R8SScaledPack32:
			case GraphicsFormat::A8B8G8R8UIntPack32:
			case GraphicsFormat::A8B8G8R8SIntPack32:
				return GL_INVALID_ENUM;
			case GraphicsFormat::R8SRGB:
			case GraphicsFormat::R8G8SRGB:
			case GraphicsFormat::R8G8B8SRGB:
			case GraphicsFormat::B8G8R8SRGB:
				return GL_INVALID_ENUM;
			case GraphicsFormat::R8G8B8A8SRGB:
			case GraphicsFormat::B8G8R8A8SRGB:
			case GraphicsFormat::A8B8G8R8SRGBPack32:
				return GL_INVALID_ENUM;
			case GraphicsFormat::D16UNorm:
			case GraphicsFormat::X8_D24UNormPack32:
			case GraphicsFormat::D32_SFLOAT:
				return GL_DEPTH_COMPONENT;
			case GraphicsFormat::S8UInt:
				return GL_STENCIL_INDEX8;
			case GraphicsFormat::D24UNorm_S8UInt:
			case GraphicsFormat::D16UNorm_S8UInt:
			case GraphicsFormat::D32_SFLOAT_S8UInt:
				return GL_INVALID_ENUM;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid texture format");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asTextureType(GraphicsFormat format) noexcept
		{
			switch (format)
			{
			case GraphicsFormat::R4G4UNormPack8:           return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R4G4B4A4UNormPack16:      return GL_UNSIGNED_SHORT_4_4_4_4;
			case GraphicsFormat::B4G4R4A4UNormPack16:      return GL_UNSIGNED_SHORT_4_4_4_4;
			case GraphicsFormat::R5G6B5UNormPack16:        return GL_UNSIGNED_SHORT_5_6_5;
			case GraphicsFormat::B5G6R5UNormPack16:        return GL_UNSIGNED_SHORT_5_6_5;
			case GraphicsFormat::R5G5B5A1UNormPack16:      return GL_UNSIGNED_SHORT_5_5_5_1;
			case GraphicsFormat::B5G5R5A1UNormPack16:      return GL_UNSIGNED_SHORT_5_5_5_1;
			case GraphicsFormat::A1R5G5B5UNormPack16:      return GL_UNSIGNED_SHORT_5_5_5_1;
			case GraphicsFormat::R8UNorm:                  return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8SNorm:                  return GL_BYTE;
			case GraphicsFormat::R8UScaled:                return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8SScaled:                return GL_BYTE;
			case GraphicsFormat::R8UInt:                   return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8SInt:                   return GL_BYTE;
			case GraphicsFormat::R8SRGB:                   return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8UNorm:                return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8SNorm:                return GL_BYTE;
			case GraphicsFormat::R8G8UScaled:              return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8SScaled:              return GL_BYTE;
			case GraphicsFormat::R8G8UInt:                 return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8SInt:                 return GL_BYTE;
			case GraphicsFormat::R8G8SRGB:                 return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8UNorm:              return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8SNorm:              return GL_BYTE;
			case GraphicsFormat::R8G8B8UScaled:            return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8SScaled:            return GL_BYTE;
			case GraphicsFormat::R8G8B8UInt:               return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8SInt:               return GL_BYTE;
			case GraphicsFormat::R8G8B8SRGB:               return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8UNorm:              return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8SNorm:              return GL_BYTE;
			case GraphicsFormat::B8G8R8UScaled:            return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8SScaled:            return GL_BYTE;
			case GraphicsFormat::B8G8R8UInt:               return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8SInt:               return GL_BYTE;
			case GraphicsFormat::B8G8R8SRGB:               return GL_BYTE;
			case GraphicsFormat::R8G8B8A8UNorm:            return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8A8SNorm:            return GL_BYTE;
			case GraphicsFormat::R8G8B8A8UScaled:          return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8A8SScaled:          return GL_BYTE;
			case GraphicsFormat::R8G8B8A8UInt:             return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8A8SInt:             return GL_BYTE;
			case GraphicsFormat::R8G8B8A8SRGB:             return GL_BYTE;
			case GraphicsFormat::B8G8R8A8UNorm:            return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8A8SNorm:            return GL_BYTE;
			case GraphicsFormat::B8G8R8A8UScaled:          return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8A8SScaled:          return GL_BYTE;
			case GraphicsFormat::B8G8R8A8UInt:             return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8A8SInt:             return GL_BYTE;
			case GraphicsFormat::B8G8R8A8SRGB:             return GL_BYTE;
			case GraphicsFormat::A8B8G8R8UNormPack32:      return GL_UNSIGNED_BYTE;
			case GraphicsFormat::A8B8G8R8SNormPack32:      return GL_BYTE;
			case GraphicsFormat::A8B8G8R8UScaledPack32:    return GL_UNSIGNED_BYTE;
			case GraphicsFormat::A8B8G8R8SScaledPack32:    return GL_BYTE;
			case GraphicsFormat::A8B8G8R8UIntPack32:       return GL_UNSIGNED_BYTE;
			case GraphicsFormat::A8B8G8R8SIntPack32:       return GL_BYTE;
			case GraphicsFormat::A8B8G8R8SRGBPack32:       return GL_UNSIGNED_BYTE;
			case GraphicsFormat::A2R10G10B10UNormPack32:   return GL_INVALID_ENUM;
			case GraphicsFormat::A2R10G10B10UScaledPack32: return GL_INVALID_ENUM;
			case GraphicsFormat::A2R10G10B10UIntPack32:    return GL_INVALID_ENUM;
			case GraphicsFormat::A2B10G10R10UNormPack32:   return GL_INVALID_ENUM;
			case GraphicsFormat::A2B10G10R10UScaledPack32: return GL_INVALID_ENUM;
			case GraphicsFormat::A2B10G10R10UIntPack32:    return GL_INVALID_ENUM;
			case GraphicsFormat::R16UNorm:                 return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16SNorm:                 return GL_SHORT;
			case GraphicsFormat::R16UScaled:               return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16SScaled:               return GL_SHORT;
			case GraphicsFormat::R16UInt:                  return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16SInt:                  return GL_SHORT;
			case GraphicsFormat::R16SFloat:                return GL_INVALID_ENUM;
			case GraphicsFormat::R16G16UNorm:              return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16SNorm:              return GL_SHORT;
			case GraphicsFormat::R16G16UScaled:            return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16SScaled:            return GL_SHORT;
			case GraphicsFormat::R16G16UInt:               return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16SInt:               return GL_SHORT;
			case GraphicsFormat::R16G16SFloat:             return GL_INVALID_ENUM;
			case GraphicsFormat::R16G16B16UNorm:           return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16SNorm:           return GL_SHORT;
			case GraphicsFormat::R16G16B16UScaled:         return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16SScaled:         return GL_SHORT;
			case GraphicsFormat::R16G16B16UInt:            return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16SInt:            return GL_SHORT;
			case GraphicsFormat::R16G16B16SFloat:          return GL_INVALID_ENUM;
			case GraphicsFormat::R16G16B16A16UNorm:        return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16A16SNorm:        return GL_SHORT;
			case GraphicsFormat::R16G16B16A16UScaled:      return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16A16SScaled:      return GL_SHORT;
			case GraphicsFormat::R16G16B16A16UInt:         return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16A16SInt:         return GL_SHORT;
			case GraphicsFormat::R16G16B16A16SFloat:       return GL_INVALID_ENUM;
			case GraphicsFormat::R32UInt:                  return GL_UNSIGNED_INT;
			case GraphicsFormat::R32SInt:                  return GL_INT;
			case GraphicsFormat::R32SFloat:                return GL_FLOAT;
			case GraphicsFormat::R32G32UInt:               return GL_UNSIGNED_INT;
			case GraphicsFormat::R32G32SInt:               return GL_INT;
			case GraphicsFormat::R32G32SFloat:             return GL_FLOAT;
			case GraphicsFormat::R32G32B32UInt:            return GL_UNSIGNED_INT;
			case GraphicsFormat::R32G32B32SInt:            return GL_INT;
			case GraphicsFormat::R32G32B32SFloat:          return GL_FLOAT;
			case GraphicsFormat::R32G32B32A32UInt:         return GL_UNSIGNED_INT;
			case GraphicsFormat::R32G32B32A32SInt:         return GL_INT;
			case GraphicsFormat::R32G32B32A32SFloat:       return GL_FLOAT;
			case GraphicsFormat::D16UNorm:                 return GL_UNSIGNED_SHORT;
			case GraphicsFormat::D32_SFLOAT:               return GL_FLOAT;
			case GraphicsFormat::S8UInt:                   return GL_UNSIGNED_BYTE;
			case GraphicsFormat::X8_D24UNormPack32:        return GL_UNSIGNED_INT;
			case GraphicsFormat::D16UNorm_S8UInt:          return GL_INVALID_ENUM;
			case GraphicsFormat::D24UNorm_S8UInt:          return GL_INVALID_ENUM;
			case GraphicsFormat::D32_SFLOAT_S8UInt:        return GL_INVALID_ENUM;
			case GraphicsFormat::A2R10G10B10SNormPack32:   return GL_INVALID_ENUM;
			case GraphicsFormat::A2R10G10B10SScaledPack32: return GL_INVALID_ENUM;
			case GraphicsFormat::A2R10G10B10SIntPack32:    return GL_INVALID_ENUM;
			case GraphicsFormat::A2B10G10R10SNormPack32:   return GL_INVALID_ENUM;
			case GraphicsFormat::A2B10G10R10SScaledPack32: return GL_INVALID_ENUM;
			case GraphicsFormat::A2B10G10R10SIntPack32:    return GL_INVALID_ENUM;
			case GraphicsFormat::B10G11R11UFloatPack32:    return GL_INVALID_ENUM;
			case GraphicsFormat::E5B9G9R9UFloatPack32:     return GL_INVALID_ENUM;
			case GraphicsFormat::R64UInt:
			case GraphicsFormat::R64SInt:
			case GraphicsFormat::R64SFloat:
			case GraphicsFormat::R64G64UInt:
			case GraphicsFormat::R64G64SInt:
			case GraphicsFormat::R64G64SFloat:
			case GraphicsFormat::R64G64B64UInt:
			case GraphicsFormat::R64G64B64SInt:
			case GraphicsFormat::R64G64B64SFloat:
			case GraphicsFormat::R64G64B64A64UInt:
			case GraphicsFormat::R64G64B64A64SInt:
			case GraphicsFormat::R64G64B64A64SFloat:
				GL_PLATFORM_ASSERT(false, "Can't support 64bit format");
				return GL_INVALID_ENUM;
			default:
				GL_PLATFORM_ASSERT(false, "Invlida texture data type.");
			}

			return GL_INVALID_ENUM;
		}

		GLenum
		GL20Types::asTextureInternalFormat(GraphicsFormat format) noexcept
		{
			GLenum internalFormat = GL_INVALID_ENUM;
			switch (format)
			{
			case GraphicsFormat::R4G4UNormPack8:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R4G4B4A4UNormPack16:	    internalFormat = GL_RGBA4; break;
			case GraphicsFormat::B4G4R4A4UNormPack16:	    internalFormat = GL_RGBA4; break;
			case GraphicsFormat::R5G6B5UNormPack16:	        internalFormat = GL_RGB565; break;
			case GraphicsFormat::B5G6R5UNormPack16:	        internalFormat = GL_RGB565; break;
			case GraphicsFormat::R5G5B5A1UNormPack16:	    internalFormat = GL_RGB5_A1; break;
			case GraphicsFormat::B5G5R5A1UNormPack16:	    internalFormat = GL_RGB5_A1; break;
			case GraphicsFormat::A1R5G5B5UNormPack16:	    internalFormat = GL_RGB5_A1; break;
			case GraphicsFormat::R8UNorm:                   internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8SNorm:                   internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8UScaled:                 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8SScaled:                 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8UInt:                    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8SInt:                    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8SRGB:                    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8UNorm:                 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8SNorm:                 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8UScaled:               internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8SScaled:               internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8UInt:                  internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8SInt:                  internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8SRGB:                  internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8UNorm:               internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8SNorm:               internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8UScaled:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8SScaled:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8UInt:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8SInt:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8SRGB:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8UNorm:               internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8SNorm:               internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8UScaled:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8SScaled:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8UInt:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8SInt:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8SRGB:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8A8UNorm:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8A8SNorm:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8A8UScaled:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8A8SScaled:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8A8UInt:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8A8SInt:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R8G8B8A8SRGB:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8A8UNorm:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8A8SNorm:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8A8UScaled:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8A8SScaled:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8A8UInt:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8A8SInt:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B8G8R8A8SRGB:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A8B8G8R8UNormPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A8B8G8R8SNormPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A8B8G8R8UScaledPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A8B8G8R8SScaledPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A8B8G8R8UIntPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A8B8G8R8SIntPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A8B8G8R8SRGBPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2R10G10B10UNormPack32:	internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2R10G10B10SNormPack32:	internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2R10G10B10UScaledPack32:  internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2R10G10B10SScaledPack32:  internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2R10G10B10UIntPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2R10G10B10SIntPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2B10G10R10UNormPack32:	internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2B10G10R10SNormPack32:	internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2B10G10R10UScaledPack32:  internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2B10G10R10SScaledPack32:  internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2B10G10R10UIntPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2B10G10R10SIntPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16UNorm:                  internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16SNorm:                  internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16UScaled:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16SScaled:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16UInt:                   internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16SInt:                   internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16SFloat:                 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16UNorm:               internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16SNorm:               internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16UScaled:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16SScaled:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16UInt:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16SInt:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16SFloat:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16UNorm:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16SNorm:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16UScaled:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16SScaled:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16UInt:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16SInt:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16SFloat:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16A16UNorm:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16A16SNorm:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16A16UScaled:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16A16SScaled:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16A16UInt:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16A16SInt:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16G16B16A16SFloat:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32UInt:                   internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32SInt:                   internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32SFloat:                 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32G32UInt:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32G32SInt:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32G32SFloat:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32G32B32UInt:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32G32B32SInt:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32G32B32SFloat:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32G32B32A32UInt:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32G32B32A32SInt:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R32G32B32A32SFloat:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64UInt:                   internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64SInt:                   internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64SFloat:                 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64UInt:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64SInt:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64SFloat:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64UInt:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64SInt:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64SFloat:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64A64UInt:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64A64SInt:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64A64SFloat:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B10G11R11UFloatPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::E5B9G9R9UFloatPack32:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::D16UNorm:                  internalFormat = GL_DEPTH_COMPONENT16; break;
			case GraphicsFormat::X8_D24UNormPack32:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::D32_SFLOAT:                internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::S8UInt:                    internalFormat = GL_STENCIL_INDEX8; break;
			case GraphicsFormat::D16UNorm_S8UInt:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::D24UNorm_S8UInt:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::D32_SFLOAT_S8UInt:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC1RGBUNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC1RGBSRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC1RGBAUNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC1RGBASRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC2UNormBlock:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC2SRGBBlock:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC3UNormBlock:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC3SRGBBlock:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC4UNormBlock:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC4SNormBlock:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC5UNormBlock:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC5SNormBlock:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC6HUFloatBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC6HSFloatBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC7UNormBlock:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::BC7SRGBBlock:	            internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ETC2R8G8B8UNormBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ETC2R8G8B8SRGBBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ETC2R8G8B8A1UNormBlock:	internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ETC2R8G8B8A1SRGBBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ETC2R8G8B8A8UNormBlock:	internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ETC2R8G8B8A8SRGBBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::EACR11UNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::EACR11SNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::EACR11G11UNormBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::EACR11G11SNormBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC4x4UNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC4x4SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC5x4UNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC5x4SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC5x5UNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC5x5SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC6x5UNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC6x5SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC6x6UNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC6x6SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC8x5UNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC8x5SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC8x6UNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC8x6SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC8x8UNormBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC8x8SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC10x5UNormBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC10x5SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC10x6UNormBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC10x6SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC10x8UNormBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC10x8SRGBBlock:	        internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC10x10UNormBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC10x10SRGBBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC12x10UNormBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC12x10SRGBBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC12x12UNormBlock:	    internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::ASTC12x12SRGBBlock:	    internalFormat = GL_INVALID_ENUM; break;
			default:
				assert(false);
				return GL_INVALID_ENUM;
			}

			GL_PLATFORM_ASSERT(internalFormat != GL_INVALID_ENUM, "Invalid texture internal format.");
			return internalFormat;
		}

		GLenum
		GL20Types::asCompareFunction(GraphicsCompareFunc func) noexcept
		{
			switch (func)
			{
			case GraphicsCompareFunc::None:     return GL_NONE;
			case GraphicsCompareFunc::Lequal:   return GL_LEQUAL;
			case GraphicsCompareFunc::Equal:    return GL_EQUAL;
			case GraphicsCompareFunc::Greater:  return GL_GREATER;
			case GraphicsCompareFunc::Less:     return GL_LESS;
			case GraphicsCompareFunc::Gequal:   return GL_GEQUAL;
			case GraphicsCompareFunc::NotEqual: return GL_NOTEQUAL;
			case GraphicsCompareFunc::Always:   return GL_ALWAYS;
			case GraphicsCompareFunc::Never:    return GL_NEVER;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid compare function");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asBlendFactor(GraphicsBlendFactor func) noexcept
		{
			switch (func)
			{
			case GraphicsBlendFactor::Zero:                  return GL_ZERO;
			case GraphicsBlendFactor::One:                   return GL_ONE;
			case GraphicsBlendFactor::DstCol:                return GL_DST_COLOR;
			case GraphicsBlendFactor::SrcColor:              return GL_SRC_COLOR;
			case GraphicsBlendFactor::SrcAlpha:              return GL_SRC_ALPHA;
			case GraphicsBlendFactor::DstAlpha:              return GL_DST_ALPHA;
			case GraphicsBlendFactor::OneMinusSrcCol:        return GL_ONE_MINUS_SRC_COLOR;
			case GraphicsBlendFactor::OneMinusDstCol:        return GL_ONE_MINUS_DST_COLOR;
			case GraphicsBlendFactor::OneMinusSrcAlpha:      return GL_ONE_MINUS_SRC_ALPHA;
			case GraphicsBlendFactor::OneMinusDstAlpha:      return GL_ONE_MINUS_DST_ALPHA;
			case GraphicsBlendFactor::ConstantColor:         return GL_CONSTANT_COLOR;
			case GraphicsBlendFactor::ConstantAlpha:         return GL_CONSTANT_ALPHA;
			case GraphicsBlendFactor::OneMinusConstantColor: return GL_CONSTANT_ALPHA;
			case GraphicsBlendFactor::OneMinusConstantAlpha: return GL_CONSTANT_ALPHA;
			case GraphicsBlendFactor::SrcAlphaSaturate:      return GL_SRC_ALPHA_SATURATE;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid blend factor");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asBlendOperation(GraphicsBlendOp blendop) noexcept
		{
			switch (blendop)
			{
			case GraphicsBlendOp::Add:        return GL_FUNC_ADD;
			case GraphicsBlendOp::Subtract:   return GL_FUNC_SUBTRACT;
			case GraphicsBlendOp::RevSubtract:return GL_FUNC_REVERSE_SUBTRACT;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid blend operation");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asCullMode(GraphicsCullMode mode) noexcept
		{
			switch (mode)
			{
			case GraphicsCullMode::None:      return GL_NONE;
			case GraphicsCullMode::Front:     return GL_FRONT;
			case GraphicsCullMode::Back:      return GL_BACK;
			case GraphicsCullMode::FrontBack: return GL_FRONT_AND_BACK;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid cull mode");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asFrontFace(GraphicsFrontFace face) noexcept
		{
			switch (face)
			{
			case GraphicsFrontFace::CW:
				return GL_CW;
			case GraphicsFrontFace::CCW:
				return GL_CCW;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid front face");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asFillMode(GraphicsPolygonMode mode) noexcept
		{
			switch (mode)
			{
			case GraphicsPolygonMode::Point:
			case GraphicsPolygonMode::Wireframe:
			case GraphicsPolygonMode::Solid:
				GL_PLATFORM_ASSERT(false, "Can't support glPolygonMode");
				return GL_INVALID_ENUM;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid fill mode");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asStencilOperation(GraphicsStencilOp stencilop) noexcept
		{
			switch (stencilop)
			{
			case GraphicsStencilOp::Keep:     return GL_KEEP;
			case GraphicsStencilOp::Replace:  return GL_REPLACE;
			case GraphicsStencilOp::Incr:     return GL_INCR;
			case GraphicsStencilOp::Decr:     return GL_DECR;
			case GraphicsStencilOp::Zero:     return GL_ZERO;
			case GraphicsStencilOp::IncrWrap: return GL_INCR_WRAP;
			case GraphicsStencilOp::DecrWrap: return GL_DECR_WRAP;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid stencil operation");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asSamplerWrap(GraphicsSamplerWrap wrap) noexcept
		{
			switch (wrap)
			{
			case GraphicsSamplerWrap::Repeat:      return GL_REPEAT;
			case GraphicsSamplerWrap::Mirror:      return GL_MIRRORED_REPEAT;
			case GraphicsSamplerWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid sampler wrap");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asSamplerMinFilter(GraphicsSamplerFilter filter) noexcept
		{
			switch (filter)
			{
			case GraphicsSamplerFilter::Nearest:              return GL_NEAREST;
			case GraphicsSamplerFilter::Linear:               return GL_LINEAR;
			case GraphicsSamplerFilter::NearestMipmapLinear:  return GL_NEAREST_MIPMAP_LINEAR;
			case GraphicsSamplerFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
			case GraphicsSamplerFilter::LinearMipmapNearest:  return GL_LINEAR_MIPMAP_NEAREST;
			case GraphicsSamplerFilter::LinearMipmapLinear:   return GL_LINEAR_MIPMAP_LINEAR;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid sampler filter");
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		GL20Types::asSamplerMagFilter(GraphicsSamplerFilter filter) noexcept
		{
			switch (filter)
			{
			case GraphicsSamplerFilter::Nearest:              return GL_NEAREST;
			case GraphicsSamplerFilter::Linear:               return GL_LINEAR;
			case GraphicsSamplerFilter::NearestMipmapLinear:  return GL_NEAREST;
			case GraphicsSamplerFilter::NearestMipmapNearest: return GL_NEAREST;
			case GraphicsSamplerFilter::LinearMipmapNearest:  return GL_LINEAR;
			case GraphicsSamplerFilter::LinearMipmapLinear:   return GL_LINEAR;
			default:
				GL_PLATFORM_ASSERT(false, "Invalid sampler filter");
				return GL_INVALID_ENUM;
			}
		}

		GLsizei
		GL20Types::getFormatNum(GLenum format, GLenum type) noexcept
		{
			GLsizei typeSize = 0;
			if (type == GL_UNSIGNED_BYTE || type == GL_BYTE)
				typeSize = 1;
			else if (type == GL_UNSIGNED_SHORT || type == GL_SHORT)
				typeSize = 2;
			else if (type == GL_UNSIGNED_INT || type == GL_INT || type == GL_FLOAT)
				typeSize = 4;
			else
			{
				assert(false);
				return 1;
			}

			if (format == GL_RGB)
				return 3 * typeSize;
			else if (format == GL_RGBA)
				return 4 * typeSize;
			else
			{
				assert(false);
				return 0;
			}
		}

		GLsizei
		GL20Types::getCompressedTextureSize(GLsizei width, GLsizei height, GLsizei depth, GLenum internalFormat) noexcept
		{
			GL_PLATFORM_ASSERT(false, "bad texformat in compressed_texture_size");
			return 0;
		}

		GLboolean
		GL20Types::isNormFormat(GraphicsFormat format) noexcept
		{
			switch (format)
			{
			case GraphicsFormat::R8UNorm:
			case GraphicsFormat::R8SNorm:
			case GraphicsFormat::R8G8UNorm:
			case GraphicsFormat::R8G8SNorm:
			case GraphicsFormat::R8G8B8UNorm:
			case GraphicsFormat::R8G8B8SNorm:
			case GraphicsFormat::B8G8R8UNorm:
			case GraphicsFormat::B8G8R8SNorm:
			case GraphicsFormat::R8G8B8A8UNorm:
			case GraphicsFormat::R8G8B8A8SNorm:
			case GraphicsFormat::B8G8R8A8UNorm:
			case GraphicsFormat::B8G8R8A8SNorm:
			case GraphicsFormat::A8B8G8R8UNormPack32:
			case GraphicsFormat::A8B8G8R8SNormPack32:
			case GraphicsFormat::A2R10G10B10UNormPack32:
			case GraphicsFormat::A2R10G10B10SNormPack32:
			case GraphicsFormat::A2B10G10R10UNormPack32:
			case GraphicsFormat::A2B10G10R10SNormPack32:
			case GraphicsFormat::R16UNorm:
			case GraphicsFormat::R16SNorm:
			case GraphicsFormat::R16G16UNorm:
			case GraphicsFormat::R16G16SNorm:
			case GraphicsFormat::R16G16B16UNorm:
			case GraphicsFormat::R16G16B16SNorm:
			case GraphicsFormat::R16G16B16A16UNorm:
			case GraphicsFormat::R16G16B16A16SNorm:
				return GL_TRUE;
			default:
				return GL_FALSE;
			}
		}

		GLboolean
		GL20Types::isSupportFeature(GL20Features features) noexcept
		{
			assert(features >= GL20Features::GL20_BeginRange && features <= GL20Features::GL20_EndRange);
			return _gl20Features[features];
		}

		GLboolean
		GL20Types::isStencilFormat(GraphicsFormat format) noexcept
		{
			if (format == GraphicsFormat::S8UInt)
				return GL_TRUE;
			return GL_FALSE;
		}

		GLboolean
		GL20Types::isDepthFormat(GraphicsFormat format) noexcept
		{
			if (format == GraphicsFormat::D16UNorm ||
				format == GraphicsFormat::X8_D24UNormPack32 ||
				format == GraphicsFormat::D32_SFLOAT)
			{
				return GL_TRUE;
			}

			return GL_FALSE;
		}

		GLboolean
		GL20Types::isDepthStencilFormat(GraphicsFormat format) noexcept
		{
			if (format == GraphicsFormat::D16UNorm_S8UInt ||
				format == GraphicsFormat::D24UNorm_S8UInt ||
				format == GraphicsFormat::D32_SFLOAT_S8UInt)
			{
				return GL_TRUE;
			}

			return GL_FALSE;
		}

		GLboolean
		GL20Types::isCompressedTexture(GraphicsFormat format) noexcept
		{
			switch (format)
			{
			case GraphicsFormat::BC1RGBUNormBlock:
			case GraphicsFormat::BC1RGBSRGBBlock:
			case GraphicsFormat::BC1RGBAUNormBlock:
			case GraphicsFormat::BC1RGBASRGBBlock:
			case GraphicsFormat::BC2UNormBlock:
			case GraphicsFormat::BC2SRGBBlock:
			case GraphicsFormat::BC3UNormBlock:
			case GraphicsFormat::BC3SRGBBlock:
			case GraphicsFormat::BC4UNormBlock:
			case GraphicsFormat::BC4SNormBlock:
			case GraphicsFormat::BC5UNormBlock:
			case GraphicsFormat::BC5SNormBlock:
			case GraphicsFormat::BC6HUFloatBlock:
			case GraphicsFormat::BC6HSFloatBlock:
			case GraphicsFormat::BC7UNormBlock:
			case GraphicsFormat::BC7SRGBBlock:
			case GraphicsFormat::ETC2R8G8B8UNormBlock:
			case GraphicsFormat::ETC2R8G8B8SRGBBlock:
			case GraphicsFormat::ETC2R8G8B8A1UNormBlock:
			case GraphicsFormat::ETC2R8G8B8A1SRGBBlock:
			case GraphicsFormat::ETC2R8G8B8A8UNormBlock:
			case GraphicsFormat::ETC2R8G8B8A8SRGBBlock:
			case GraphicsFormat::EACR11UNormBlock:
			case GraphicsFormat::EACR11SNormBlock:
			case GraphicsFormat::EACR11G11UNormBlock:
			case GraphicsFormat::EACR11G11SNormBlock:
			case GraphicsFormat::ASTC4x4UNormBlock:
			case GraphicsFormat::ASTC4x4SRGBBlock:
			case GraphicsFormat::ASTC5x4UNormBlock:
			case GraphicsFormat::ASTC5x4SRGBBlock:
			case GraphicsFormat::ASTC5x5UNormBlock:
			case GraphicsFormat::ASTC5x5SRGBBlock:
			case GraphicsFormat::ASTC6x5UNormBlock:
			case GraphicsFormat::ASTC6x5SRGBBlock:
			case GraphicsFormat::ASTC6x6UNormBlock:
			case GraphicsFormat::ASTC6x6SRGBBlock:
			case GraphicsFormat::ASTC8x5UNormBlock:
			case GraphicsFormat::ASTC8x5SRGBBlock:
			case GraphicsFormat::ASTC8x6UNormBlock:
			case GraphicsFormat::ASTC8x6SRGBBlock:
			case GraphicsFormat::ASTC8x8UNormBlock:
			case GraphicsFormat::ASTC8x8SRGBBlock:
			case GraphicsFormat::ASTC10x5UNormBlock:
			case GraphicsFormat::ASTC10x5SRGBBlock:
			case GraphicsFormat::ASTC10x6UNormBlock:
			case GraphicsFormat::ASTC10x6SRGBBlock:
			case GraphicsFormat::ASTC10x8UNormBlock:
			case GraphicsFormat::ASTC10x8SRGBBlock:
			case GraphicsFormat::ASTC10x10UNormBlock:
			case GraphicsFormat::ASTC10x10SRGBBlock:
			case GraphicsFormat::ASTC12x10UNormBlock:
			case GraphicsFormat::ASTC12x10SRGBBlock:
			case GraphicsFormat::ASTC12x12UNormBlock:
			case GraphicsFormat::ASTC12x12SRGBBlock:
				return GL_TRUE;
			default:
				return GL_FALSE;
			}
		}

		bool
		GL20Check::checkError() noexcept
		{
			bool success = true;
			GLenum result = ::glGetError();
			if (GL_NO_ERROR != result)
			{
				success = false;

				switch (result)
				{
				case GL_INVALID_ENUM:
					GL_PLATFORM_LOG("glGetError() fail : GL_INVALID_ENUM");
					break;
				case GL_INVALID_VALUE:
					GL_PLATFORM_LOG("glGetError() fail : GL_INVALID_VALUE");
					break;
				case GL_INVALID_OPERATION:
					GL_PLATFORM_LOG("glGetError() fail : GL_INVALID_OPERATION");
					break;
				case GL_OUT_OF_MEMORY:
					GL_PLATFORM_LOG("glGetError() fail : GL_OUT_OF_MEMORY");
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					GL_PLATFORM_LOG("glGetError() fail : GL_INVALID_FRAMEBUFFER_OPERATION");
					break;
				default:
					GL_PLATFORM_LOG("glGetError() fail : Unknown");
				};
			}

			result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (GL_FRAMEBUFFER_COMPLETE != result)
			{
				success = false;

				switch (result)
				{
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					GL_PLATFORM_LOG("FBO:Incomplete attachment");
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					GL_PLATFORM_LOG("FBO:Incomplete missing attachment");
					break;
				case GL_FRAMEBUFFER_UNSUPPORTED:
					GL_PLATFORM_LOG("FBO:Unsupported");
					break;
				default:
					GL_PLATFORM_LOG("FBO:Unknown");
				}
			}

			return success;
		}

		void
		GL20Check::debugOutput(const char* message, ...) noexcept
		{
			va_list va;
			va_start(va, message);
			vprintf(message, va);
			printf("\n");
			va_end(va);
		}
	}
}