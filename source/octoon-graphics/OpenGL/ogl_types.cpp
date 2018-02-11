#include "ogl_types.h"

#include <iostream>

namespace octoon
{
	namespace graphics
	{
		#ifdef _BUILD_OPENGL
		#	ifdef GLEW_MX
		GLEWContext _glewctx;
		#	endif
		#endif

		GLenum
		OGLTypes::asVertexType(GraphicsVertexType type) noexcept
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
				assert(false);
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asVertexFormat(GraphicsFormat format) noexcept
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
				return GL_INT64_ARB;
			case GraphicsFormat::R64UInt:
			case GraphicsFormat::R64G64UInt:
			case GraphicsFormat::R64G64B64UInt:
			case GraphicsFormat::R64G64B64A64UInt:
				return GL_UNSIGNED_INT64_ARB;
			case GraphicsFormat::R64SFloat:
			case GraphicsFormat::R64G64SFloat:
			case GraphicsFormat::R64G64B64SFloat:
			case GraphicsFormat::R64G64B64A64SFloat:
				return GL_DOUBLE;
			default:
				assert(false);
				return GL_INVALID_ENUM;
				break;
			}
		}

		GLenum
		OGLTypes::asIndexType(GraphicsIndexType type) noexcept
		{
			switch (type)
			{
			case GraphicsIndexType::UInt16: return GL_UNSIGNED_SHORT;
			case GraphicsIndexType::UInt32: return GL_UNSIGNED_INT;
			default:
				assert(false);
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asShaderStage(GraphicsShaderStageFlagBits stage) noexcept
		{
			switch (stage)
			{
			case GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit:         return GL_VERTEX_SHADER;
			case GraphicsShaderStageFlagBits::GraphicsShaderStageFragmentBit:       return GL_FRAGMENT_SHADER;
			case GraphicsShaderStageFlagBits::GraphicsShaderStageComputeBit:        return GL_COMPUTE_SHADER;
			case GraphicsShaderStageFlagBits::GraphicsShaderStageGeometryBit:       return GL_GEOMETRY_SHADER;
			case GraphicsShaderStageFlagBits::GraphicsShaderStageTessControlBit:    return GL_TESS_CONTROL_SHADER;
			case GraphicsShaderStageFlagBits::GraphicsShaderStageTessEvaluationBit: return GL_TESS_EVALUATION_SHADER;
			default:
				assert(false);
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asTextureTarget(GraphicsTextureDim target, bool multisampler) noexcept
		{
			if (target == GraphicsTextureDim::Texture2D)
			{
				if (multisampler)
					return GL_TEXTURE_2D_MULTISAMPLE;
				else
					return GL_TEXTURE_2D;
			}
			else if (target == GraphicsTextureDim::Texture3D)
			{
				if (!multisampler)
					return GL_TEXTURE_3D;
				else
					return GL_INVALID_ENUM;
			}
			else if (target == GraphicsTextureDim::Texture2DArray)
			{
				if (multisampler)
					return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
				else
					return GL_TEXTURE_2D_ARRAY;
			}
			else if (target == GraphicsTextureDim::Cube)
			{
				if (!multisampler)
					return GL_TEXTURE_CUBE_MAP;
				else
					return GL_INVALID_ENUM;
			}
			else if (target == GraphicsTextureDim::CubeArray)
			{
				if (!multisampler)
					return GL_TEXTURE_CUBE_MAP_ARRAY;
				else
					return GL_INVALID_ENUM;
			}

			return GL_INVALID_ENUM;
		}

		GLenum
		OGLTypes::asTextureFormat(GraphicsFormat format) noexcept
		{
			switch (format)
			{
			case GraphicsFormat::R8UNorm:
			case GraphicsFormat::R8SNorm:
			case GraphicsFormat::R8UScaled:
			case GraphicsFormat::R8SScaled:
			case GraphicsFormat::R8UInt:
			case GraphicsFormat::R8SInt:
			case GraphicsFormat::R8SRGB:
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
				return GL_RED;
			case GraphicsFormat::R4G4UNormPack8:
			case GraphicsFormat::R8G8UNorm:
			case GraphicsFormat::R8G8SNorm:
			case GraphicsFormat::R8G8UScaled:
			case GraphicsFormat::R8G8SScaled:
			case GraphicsFormat::R8G8UInt:
			case GraphicsFormat::R8G8SInt:
			case GraphicsFormat::R8G8SRGB:
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
				return GL_RG;
			case GraphicsFormat::R5G6B5UNormPack16:
			case GraphicsFormat::R8G8B8UNorm:
			case GraphicsFormat::R8G8B8SNorm:
			case GraphicsFormat::R8G8B8UScaled:
			case GraphicsFormat::R8G8B8SScaled:
			case GraphicsFormat::R8G8B8UInt:
			case GraphicsFormat::R8G8B8SInt:
			case GraphicsFormat::R8G8B8SRGB:
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
			case GraphicsFormat::B8G8R8SRGB:
			case GraphicsFormat::B10G11R11UFloatPack32:
				return GL_BGR;
			case GraphicsFormat::R4G4B4A4UNormPack16:
			case GraphicsFormat::R5G5B5A1UNormPack16:
			case GraphicsFormat::A1R5G5B5UNormPack16:
			case GraphicsFormat::R8G8B8A8UNorm:
			case GraphicsFormat::R8G8B8A8SNorm:
			case GraphicsFormat::R8G8B8A8UScaled:
			case GraphicsFormat::R8G8B8A8SScaled:
			case GraphicsFormat::R8G8B8A8UInt:
			case GraphicsFormat::R8G8B8A8SInt:
			case GraphicsFormat::R8G8B8A8SRGB:
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
			case GraphicsFormat::B8G8R8A8SRGB:
			case GraphicsFormat::A2B10G10R10UNormPack32:
			case GraphicsFormat::A2B10G10R10SNormPack32:
			case GraphicsFormat::A2B10G10R10UScaledPack32:
			case GraphicsFormat::A2B10G10R10SScaledPack32:
			case GraphicsFormat::A2B10G10R10UIntPack32:
			case GraphicsFormat::A2B10G10R10SIntPack32:
				return GL_BGRA;
			case GraphicsFormat::A8B8G8R8UNormPack32:
			case GraphicsFormat::A8B8G8R8SNormPack32:
			case GraphicsFormat::A8B8G8R8UScaledPack32:
			case GraphicsFormat::A8B8G8R8SScaledPack32:
			case GraphicsFormat::A8B8G8R8UIntPack32:
			case GraphicsFormat::A8B8G8R8SIntPack32:
			case GraphicsFormat::A8B8G8R8SRGBPack32:
				return GL_ABGR_EXT;
			case GraphicsFormat::D16UNorm:
			case GraphicsFormat::X8_D24UNormPack32:
			case GraphicsFormat::D32_SFLOAT:
				return GL_DEPTH_COMPONENT;
			case GraphicsFormat::S8UInt:
				return GL_STENCIL_INDEX;
			case GraphicsFormat::D16UNorm_S8UInt:
			case GraphicsFormat::D24UNorm_S8UInt:
			case GraphicsFormat::D32_SFLOAT_S8UInt:
				return GL_DEPTH_STENCIL;
			}

			return GL_INVALID_ENUM;
		}

		GLenum
		OGLTypes::asTextureType(GraphicsFormat format) noexcept
		{
			switch (format)
			{
			case GraphicsFormat::R8SNorm:                  return GL_BYTE;
			case GraphicsFormat::R8SScaled:                return GL_BYTE;
			case GraphicsFormat::R8SInt:                   return GL_BYTE;
			case GraphicsFormat::R8G8SNorm:                return GL_BYTE;
			case GraphicsFormat::R8G8SScaled:              return GL_BYTE;
			case GraphicsFormat::R8G8SInt:                 return GL_BYTE;
			case GraphicsFormat::R8G8B8SNorm:              return GL_BYTE;
			case GraphicsFormat::R8G8B8SScaled:            return GL_BYTE;
			case GraphicsFormat::R8G8B8SInt:               return GL_BYTE;
			case GraphicsFormat::B8G8R8SNorm:              return GL_BYTE;
			case GraphicsFormat::B8G8R8SScaled:            return GL_BYTE;
			case GraphicsFormat::B8G8R8SInt:               return GL_BYTE;
			case GraphicsFormat::R8G8B8A8SNorm:            return GL_BYTE;
			case GraphicsFormat::R8G8B8A8SScaled:          return GL_BYTE;
			case GraphicsFormat::R8G8B8A8SInt:             return GL_BYTE;
			case GraphicsFormat::B8G8R8A8SNorm:            return GL_BYTE;
			case GraphicsFormat::B8G8R8A8SScaled:          return GL_BYTE;
			case GraphicsFormat::B8G8R8A8SInt:             return GL_BYTE;
			case GraphicsFormat::A8B8G8R8SNormPack32:      return GL_BYTE;
			case GraphicsFormat::A8B8G8R8SScaledPack32:    return GL_BYTE;
			case GraphicsFormat::A8B8G8R8SIntPack32:       return GL_BYTE;
			case GraphicsFormat::R4G4UNormPack8:           return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8UNorm:                  return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8UScaled:                return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8UInt:                   return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8SRGB:                   return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8UNorm:                return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8UScaled:              return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8UInt:                 return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8SRGB:                 return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8UNorm:              return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8UScaled:            return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8UInt:               return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8SRGB:               return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8UNorm:              return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8UScaled:            return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8UInt:               return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8SRGB:               return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8A8UNorm:            return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8A8UScaled:          return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8A8UInt:             return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R8G8B8A8SRGB:             return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8A8UNorm:            return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8A8UScaled:          return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8A8UInt:             return GL_UNSIGNED_BYTE;
			case GraphicsFormat::B8G8R8A8SRGB:             return GL_UNSIGNED_BYTE;
			case GraphicsFormat::A8B8G8R8UNormPack32:      return GL_UNSIGNED_BYTE;
			case GraphicsFormat::A8B8G8R8UScaledPack32:    return GL_UNSIGNED_BYTE;
			case GraphicsFormat::A8B8G8R8UIntPack32:       return GL_UNSIGNED_BYTE;
			case GraphicsFormat::A8B8G8R8SRGBPack32:       return GL_UNSIGNED_BYTE;
			case GraphicsFormat::S8UInt:                   return GL_UNSIGNED_BYTE;
			case GraphicsFormat::R16SNorm:                 return GL_SHORT;
			case GraphicsFormat::R16SScaled:               return GL_SHORT;
			case GraphicsFormat::R16SInt:                  return GL_SHORT;
			case GraphicsFormat::R16G16SNorm:              return GL_SHORT;
			case GraphicsFormat::R16G16SScaled:            return GL_SHORT;
			case GraphicsFormat::R16G16SInt:               return GL_SHORT;
			case GraphicsFormat::R16G16B16SNorm:           return GL_SHORT;
			case GraphicsFormat::R16G16B16SScaled:         return GL_SHORT;
			case GraphicsFormat::R16G16B16SInt:            return GL_SHORT;
			case GraphicsFormat::R16G16B16A16SNorm:        return GL_SHORT;
			case GraphicsFormat::R16G16B16A16SScaled:      return GL_SHORT;
			case GraphicsFormat::R16G16B16A16SInt:         return GL_SHORT;
			case GraphicsFormat::R16UNorm:                 return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16UScaled:               return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16UInt:                  return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16UNorm:              return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16UScaled:            return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16UInt:               return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16UNorm:           return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16UScaled:         return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16UInt:            return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16A16UNorm:        return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16A16UScaled:      return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R16G16B16A16UInt:         return GL_UNSIGNED_SHORT;
			case GraphicsFormat::D16UNorm:                 return GL_UNSIGNED_SHORT;
			case GraphicsFormat::R4G4B4A4UNormPack16:      return GL_UNSIGNED_SHORT_4_4_4_4;
			case GraphicsFormat::B4G4R4A4UNormPack16:      return GL_UNSIGNED_SHORT_4_4_4_4;
			case GraphicsFormat::R5G6B5UNormPack16:        return GL_UNSIGNED_SHORT_5_6_5;
			case GraphicsFormat::B5G6R5UNormPack16:        return GL_UNSIGNED_SHORT_5_6_5;
			case GraphicsFormat::R5G5B5A1UNormPack16:      return GL_UNSIGNED_SHORT_5_5_5_1;
			case GraphicsFormat::B5G5R5A1UNormPack16:      return GL_UNSIGNED_SHORT_5_5_5_1;
			case GraphicsFormat::A1R5G5B5UNormPack16:      return GL_UNSIGNED_SHORT_1_5_5_5_REV;
			case GraphicsFormat::R32SInt:                  return GL_INT;
			case GraphicsFormat::R32G32SInt:               return GL_INT;
			case GraphicsFormat::R32G32B32SInt:            return GL_INT;
			case GraphicsFormat::R32G32B32A32SInt:         return GL_INT;
			case GraphicsFormat::A2R10G10B10SNormPack32:   return GL_INT_2_10_10_10_REV;
			case GraphicsFormat::A2R10G10B10SScaledPack32: return GL_INT_2_10_10_10_REV;
			case GraphicsFormat::A2R10G10B10SIntPack32:    return GL_INT_2_10_10_10_REV;
			case GraphicsFormat::A2B10G10R10SNormPack32:   return GL_INT_2_10_10_10_REV;
			case GraphicsFormat::A2B10G10R10SScaledPack32: return GL_INT_2_10_10_10_REV;
			case GraphicsFormat::A2B10G10R10SIntPack32:    return GL_INT_2_10_10_10_REV;
			case GraphicsFormat::R32UInt:                  return GL_UNSIGNED_INT;
			case GraphicsFormat::R32G32UInt:               return GL_UNSIGNED_INT;
			case GraphicsFormat::R32G32B32UInt:            return GL_UNSIGNED_INT;
			case GraphicsFormat::R32G32B32A32UInt:         return GL_UNSIGNED_INT;
			case GraphicsFormat::X8_D24UNormPack32:        return GL_UNSIGNED_INT;
			case GraphicsFormat::D16UNorm_S8UInt:          return GL_UNSIGNED_INT;
			case GraphicsFormat::A2R10G10B10UNormPack32:   return GL_UNSIGNED_INT_2_10_10_10_REV;
			case GraphicsFormat::A2R10G10B10UScaledPack32: return GL_UNSIGNED_INT_2_10_10_10_REV;
			case GraphicsFormat::A2R10G10B10UIntPack32:    return GL_UNSIGNED_INT_2_10_10_10_REV;
			case GraphicsFormat::A2B10G10R10UNormPack32:   return GL_UNSIGNED_INT_2_10_10_10_REV;
			case GraphicsFormat::A2B10G10R10UScaledPack32: return GL_UNSIGNED_INT_2_10_10_10_REV;
			case GraphicsFormat::A2B10G10R10UIntPack32:    return GL_UNSIGNED_INT_2_10_10_10_REV;
			case GraphicsFormat::E5B9G9R9UFloatPack32:     return GL_UNSIGNED_INT_5_9_9_9_REV;
			case GraphicsFormat::D24UNorm_S8UInt:          return GL_UNSIGNED_INT_24_8;
			case GraphicsFormat::R16SFloat:                return GL_HALF_FLOAT;
			case GraphicsFormat::R16G16SFloat:             return GL_HALF_FLOAT;
			case GraphicsFormat::R16G16B16SFloat:          return GL_HALF_FLOAT;
			case GraphicsFormat::R16G16B16A16SFloat:       return GL_HALF_FLOAT;
			case GraphicsFormat::R32SFloat:                return GL_FLOAT;
			case GraphicsFormat::R32G32SFloat:             return GL_FLOAT;
			case GraphicsFormat::R32G32B32SFloat:          return GL_FLOAT;
			case GraphicsFormat::R32G32B32A32SFloat:       return GL_FLOAT;
			case GraphicsFormat::B10G11R11UFloatPack32:    return GL_FLOAT;
			case GraphicsFormat::D32_SFLOAT:               return GL_FLOAT;
			case GraphicsFormat::D32_SFLOAT_S8UInt:        return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
			case GraphicsFormat::R64SFloat:                return GL_DOUBLE;
			case GraphicsFormat::R64G64SFloat:             return GL_DOUBLE;
			case GraphicsFormat::R64G64B64SFloat:          return GL_DOUBLE;
			case GraphicsFormat::R64G64B64A64SFloat:       return GL_DOUBLE;
			case GraphicsFormat::R64SInt:                  return GL_INT64_ARB;
			case GraphicsFormat::R64G64SInt:               return GL_INT64_ARB;
			case GraphicsFormat::R64G64B64SInt:            return GL_INT64_ARB;
			case GraphicsFormat::R64G64B64A64SInt:         return GL_INT64_ARB;
			case GraphicsFormat::R64UInt:                  return GL_UNSIGNED_INT64_ARB;
			case GraphicsFormat::R64G64UInt:               return GL_UNSIGNED_INT64_ARB;
			case GraphicsFormat::R64G64B64UInt:            return GL_UNSIGNED_INT64_ARB;
			case GraphicsFormat::R64G64B64A64UInt:         return GL_UNSIGNED_INT64_ARB;
			}

			return GL_INVALID_ENUM;
		}

		GLenum
		OGLTypes::asTextureInternalFormat(GraphicsFormat format) noexcept
		{
			GLenum internalFormat = GL_INVALID_ENUM;
			switch (format)
			{
			case GraphicsFormat::R4G4UNormPack8:	         internalFormat = GL_LUMINANCE4_ALPHA4; break;
			case GraphicsFormat::R4G4B4A4UNormPack16:	     internalFormat = GL_RGBA4; break;
			case GraphicsFormat::B4G4R4A4UNormPack16:	     internalFormat = GL_RGBA4; break;
			case GraphicsFormat::R5G6B5UNormPack16:	     internalFormat = GL_RGB565; break;
			case GraphicsFormat::B5G6R5UNormPack16:	     internalFormat = GL_RGB565; break;
			case GraphicsFormat::R5G5B5A1UNormPack16:	     internalFormat = GL_RGB5_A1; break;
			case GraphicsFormat::B5G5R5A1UNormPack16:	     internalFormat = GL_RGB5_A1; break;
			case GraphicsFormat::A1R5G5B5UNormPack16:	     internalFormat = GL_RGB5_A1; break;
			case GraphicsFormat::R8UNorm:	                 internalFormat = GL_R8; break;
			case GraphicsFormat::R8SNorm:	                 internalFormat = GL_R8_SNORM; break;
			case GraphicsFormat::R8UScaled:	             internalFormat = GL_R8UI; break;
			case GraphicsFormat::R8SScaled:	             internalFormat = GL_R8I; break;
			case GraphicsFormat::R8UInt:	                 internalFormat = GL_R8UI; break;
			case GraphicsFormat::R8SInt:	                 internalFormat = GL_R8I; break;
			case GraphicsFormat::R8SRGB:	                 internalFormat = GL_R8; break;
			case GraphicsFormat::R8G8UNorm:	             internalFormat = GL_RG8; break;
			case GraphicsFormat::R8G8SNorm:	             internalFormat = GL_RG8_SNORM; break;
			case GraphicsFormat::R8G8UScaled:	             internalFormat = GL_RG8UI; break;
			case GraphicsFormat::R8G8SScaled:	             internalFormat = GL_RG8I; break;
			case GraphicsFormat::R8G8UInt:	             internalFormat = GL_RG8UI; break;
			case GraphicsFormat::R8G8SInt:	             internalFormat = GL_RG8I; break;
			case GraphicsFormat::R8G8SRGB:	             internalFormat = GL_RG8; break;
			case GraphicsFormat::R8G8B8UNorm:	             internalFormat = GL_RGB8; break;
			case GraphicsFormat::R8G8B8SNorm:	             internalFormat = GL_RGB8_SNORM; break;
			case GraphicsFormat::R8G8B8UScaled:	         internalFormat = GL_RGB8UI; break;
			case GraphicsFormat::R8G8B8SScaled:	         internalFormat = GL_RGB8I; break;
			case GraphicsFormat::R8G8B8UInt:	             internalFormat = GL_RGB8UI; break;
			case GraphicsFormat::R8G8B8SInt:	             internalFormat = GL_RGB8I; break;
			case GraphicsFormat::R8G8B8SRGB:	             internalFormat = GL_SRGB8; break;
			case GraphicsFormat::B8G8R8UNorm:	             internalFormat = GL_RGB8; break;
			case GraphicsFormat::B8G8R8SNorm:	             internalFormat = GL_RGB8_SNORM; break;
			case GraphicsFormat::B8G8R8UScaled:	         internalFormat = GL_RGB8UI; break;
			case GraphicsFormat::B8G8R8SScaled:	         internalFormat = GL_RGB8I; break;
			case GraphicsFormat::B8G8R8UInt:	             internalFormat = GL_RGB8I; break;
			case GraphicsFormat::B8G8R8SInt:	             internalFormat = GL_RGB8UI; break;
			case GraphicsFormat::B8G8R8SRGB:	             internalFormat = GL_SRGB8; break;
			case GraphicsFormat::R8G8B8A8UNorm:	         internalFormat = GL_RGBA8; break;
			case GraphicsFormat::R8G8B8A8SNorm:	         internalFormat = GL_RGBA8_SNORM; break;
			case GraphicsFormat::R8G8B8A8UScaled:	         internalFormat = GL_RGBA8UI; break;
			case GraphicsFormat::R8G8B8A8SScaled:	         internalFormat = GL_RGBA8I; break;
			case GraphicsFormat::R8G8B8A8UInt:	         internalFormat = GL_RGBA8UI; break;
			case GraphicsFormat::R8G8B8A8SInt:	         internalFormat = GL_RGBA8I; break;
			case GraphicsFormat::R8G8B8A8SRGB:	         internalFormat = GL_SRGB8_ALPHA8; break;
			case GraphicsFormat::B8G8R8A8UNorm:	         internalFormat = GL_RGBA8; break;
			case GraphicsFormat::B8G8R8A8SNorm:	         internalFormat = GL_RGBA8_SNORM; break;
			case GraphicsFormat::B8G8R8A8UScaled:	         internalFormat = GL_RGBA8UI; break;
			case GraphicsFormat::B8G8R8A8SScaled:	         internalFormat = GL_RGBA8I; break;
			case GraphicsFormat::B8G8R8A8UInt:	         internalFormat = GL_RGBA8UI; break;
			case GraphicsFormat::B8G8R8A8SInt:	         internalFormat = GL_RGBA8I; break;
			case GraphicsFormat::B8G8R8A8SRGB:	         internalFormat = GL_SRGB8_ALPHA8; break;
			case GraphicsFormat::A8B8G8R8UNormPack32:	     internalFormat = GL_RGBA8; break;
			case GraphicsFormat::A8B8G8R8SNormPack32:	     internalFormat = GL_RGBA8_SNORM; break;
			case GraphicsFormat::A8B8G8R8UScaledPack32:	 internalFormat = GL_RGBA8UI; break;
			case GraphicsFormat::A8B8G8R8SScaledPack32:	 internalFormat = GL_RGBA8I; break;
			case GraphicsFormat::A8B8G8R8UIntPack32:	     internalFormat = GL_RGBA8UI; break;
			case GraphicsFormat::A8B8G8R8SIntPack32:	     internalFormat = GL_RGBA8I; break;
			case GraphicsFormat::A8B8G8R8SRGBPack32:	     internalFormat = GL_SRGB8_ALPHA8; break;
			case GraphicsFormat::A2R10G10B10UNormPack32:	 internalFormat = GL_RGB10_A2; break;
			case GraphicsFormat::A2R10G10B10SNormPack32:	 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2R10G10B10UScaledPack32: internalFormat = GL_RGB10_A2UI; break;
			case GraphicsFormat::A2R10G10B10SScaledPack32: internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2R10G10B10UIntPack32:	 internalFormat = GL_RGB10_A2UI; break;
			case GraphicsFormat::A2R10G10B10SIntPack32:	 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2B10G10R10UNormPack32:	 internalFormat = GL_RGB10_A2; break;
			case GraphicsFormat::A2B10G10R10SNormPack32:	 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2B10G10R10UScaledPack32: internalFormat = GL_RGB10_A2UI; break;
			case GraphicsFormat::A2B10G10R10SScaledPack32: internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::A2B10G10R10UIntPack32:	 internalFormat = GL_RGB10_A2UI; break;
			case GraphicsFormat::A2B10G10R10SIntPack32:	 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R16UNorm:	             internalFormat = GL_R16; break;
			case GraphicsFormat::R16SNorm:	             internalFormat = GL_R16_SNORM; break;
			case GraphicsFormat::R16UScaled:	             internalFormat = GL_R16UI; break;
			case GraphicsFormat::R16SScaled:	             internalFormat = GL_R16I; break;
			case GraphicsFormat::R16UInt:	                 internalFormat = GL_R16UI; break;
			case GraphicsFormat::R16SInt:	                 internalFormat = GL_R16I; break;
			case GraphicsFormat::R16SFloat:	             internalFormat = GL_R16F; break;
			case GraphicsFormat::R16G16UNorm:	             internalFormat = GL_RG16; break;
			case GraphicsFormat::R16G16SNorm:	             internalFormat = GL_RG16_SNORM; break;
			case GraphicsFormat::R16G16UScaled:	         internalFormat = GL_RG16UI; break;
			case GraphicsFormat::R16G16SScaled:	         internalFormat = GL_RG16I; break;
			case GraphicsFormat::R16G16UInt:	             internalFormat = GL_RG16UI; break;
			case GraphicsFormat::R16G16SInt:	             internalFormat = GL_RG16I; break;
			case GraphicsFormat::R16G16SFloat:	         internalFormat = GL_RG16F; break;
			case GraphicsFormat::R16G16B16UNorm:	         internalFormat = GL_RGB16; break;
			case GraphicsFormat::R16G16B16SNorm:	         internalFormat = GL_RGB16_SNORM; break;
			case GraphicsFormat::R16G16B16UScaled:	     internalFormat = GL_RGB16UI; break;
			case GraphicsFormat::R16G16B16SScaled:	     internalFormat = GL_RGB16I; break;
			case GraphicsFormat::R16G16B16UInt:	         internalFormat = GL_RGB16UI; break;
			case GraphicsFormat::R16G16B16SInt:	         internalFormat = GL_RGB16I; break;
			case GraphicsFormat::R16G16B16SFloat:	         internalFormat = GL_RGB16F; break;
			case GraphicsFormat::R16G16B16A16UNorm:	     internalFormat = GL_RGBA16; break;
			case GraphicsFormat::R16G16B16A16SNorm:	     internalFormat = GL_RGBA16_SNORM; break;
			case GraphicsFormat::R16G16B16A16UScaled:	     internalFormat = GL_RGBA16UI; break;
			case GraphicsFormat::R16G16B16A16SScaled:	     internalFormat = GL_RGBA16I; break;
			case GraphicsFormat::R16G16B16A16UInt:	     internalFormat = GL_RGBA16UI; break;
			case GraphicsFormat::R16G16B16A16SInt:	     internalFormat = GL_RGBA16I; break;
			case GraphicsFormat::R16G16B16A16SFloat:	     internalFormat = GL_RGBA16F; break;
			case GraphicsFormat::R32UInt:	                 internalFormat = GL_R32UI; break;
			case GraphicsFormat::R32SInt:	                 internalFormat = GL_R32I; break;
			case GraphicsFormat::R32SFloat:	             internalFormat = GL_R32F; break;
			case GraphicsFormat::R32G32UInt:	             internalFormat = GL_RG32UI; break;
			case GraphicsFormat::R32G32SInt:	             internalFormat = GL_RG32I; break;
			case GraphicsFormat::R32G32SFloat:	         internalFormat = GL_RG32F; break;
			case GraphicsFormat::R32G32B32UInt:	         internalFormat = GL_RGB32UI; break;
			case GraphicsFormat::R32G32B32SInt:	         internalFormat = GL_RGB32I; break;
			case GraphicsFormat::R32G32B32SFloat:	         internalFormat = GL_RGB32F; break;
			case GraphicsFormat::R32G32B32A32UInt:	     internalFormat = GL_RGBA32UI; break;
			case GraphicsFormat::R32G32B32A32SInt:	     internalFormat = GL_RGBA32I; break;
			case GraphicsFormat::R32G32B32A32SFloat:	     internalFormat = GL_RGBA32F; break;
			case GraphicsFormat::R64UInt:	                 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64SInt:	                 internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64SFloat:	             internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64UInt:	             internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64SInt:	             internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64SFloat:	         internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64UInt:	         internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64SInt:	         internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64SFloat:	         internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64A64UInt:	     internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64A64SInt:	     internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::R64G64B64A64SFloat:	     internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::B10G11R11UFloatPack32:	 internalFormat = GL_R11F_G11F_B10F; break;
			case GraphicsFormat::E5B9G9R9UFloatPack32:	 internalFormat = GL_RGB9_E5; break;
			case GraphicsFormat::D16UNorm:	             internalFormat = GL_DEPTH_COMPONENT16; break;
			case GraphicsFormat::X8_D24UNormPack32:	     internalFormat = GL_DEPTH_COMPONENT24; break;
			case GraphicsFormat::D32_SFLOAT:	             internalFormat = GL_DEPTH_COMPONENT32; break;
			case GraphicsFormat::S8UInt:	                 internalFormat = GL_STENCIL_INDEX8; break;
			case GraphicsFormat::D16UNorm_S8UInt:	         internalFormat = GL_INVALID_ENUM; break;
			case GraphicsFormat::D24UNorm_S8UInt:	         internalFormat = GL_DEPTH24_STENCIL8; break;
			case GraphicsFormat::D32_SFLOAT_S8UInt:	     internalFormat = GL_DEPTH32F_STENCIL8; break;
			case GraphicsFormat::BC1RGBUNormBlock:	     internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break;
			case GraphicsFormat::BC1RGBSRGBBlock:	         internalFormat = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT; break;
			case GraphicsFormat::BC1RGBAUNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
			case GraphicsFormat::BC1RGBASRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT; break;
			case GraphicsFormat::BC2UNormBlock:	         internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
			case GraphicsFormat::BC2SRGBBlock:	         internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT; break;
			case GraphicsFormat::BC3UNormBlock:	         internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
			case GraphicsFormat::BC3SRGBBlock:	         internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT; break;
			case GraphicsFormat::BC4UNormBlock:	         internalFormat = GL_COMPRESSED_RED_RGTC1; break;
			case GraphicsFormat::BC4SNormBlock:	         internalFormat = GL_COMPRESSED_SIGNED_RED_RGTC1; break;
			case GraphicsFormat::BC5UNormBlock:	         internalFormat = GL_COMPRESSED_RG_RGTC2; break;
			case GraphicsFormat::BC5SNormBlock:	         internalFormat = GL_COMPRESSED_SIGNED_RG_RGTC2; break;
			case GraphicsFormat::BC6HUFloatBlock:	         internalFormat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT; break;
			case GraphicsFormat::BC6HSFloatBlock:	         internalFormat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT; break;
			case GraphicsFormat::BC7UNormBlock:	         internalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM; break;
			case GraphicsFormat::BC7SRGBBlock:	         internalFormat = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM; break;
			case GraphicsFormat::ETC2R8G8B8UNormBlock:	 internalFormat = GL_COMPRESSED_RGB8_ETC2; break;
			case GraphicsFormat::ETC2R8G8B8SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ETC2; break;
			case GraphicsFormat::ETC2R8G8B8A1UNormBlock:	 internalFormat = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2; break;
			case GraphicsFormat::ETC2R8G8B8A1SRGBBlock:	 internalFormat = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2; break;
			case GraphicsFormat::ETC2R8G8B8A8UNormBlock:	 internalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC; break;
			case GraphicsFormat::ETC2R8G8B8A8SRGBBlock:	 internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC; break;
			case GraphicsFormat::EACR11UNormBlock:	     internalFormat = GL_COMPRESSED_R11_EAC; break;
			case GraphicsFormat::EACR11SNormBlock:	     internalFormat = GL_COMPRESSED_SIGNED_R11_EAC; break;
			case GraphicsFormat::EACR11G11UNormBlock:	     internalFormat = GL_COMPRESSED_RG11_EAC; break;
			case GraphicsFormat::EACR11G11SNormBlock:	     internalFormat = GL_COMPRESSED_SIGNED_RG11_EAC; break;
			case GraphicsFormat::ASTC4x4UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR; break;
			case GraphicsFormat::ASTC4x4SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR; break;
			case GraphicsFormat::ASTC5x4UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR; break;
			case GraphicsFormat::ASTC5x4SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR; break;
			case GraphicsFormat::ASTC5x5UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_5x5_KHR; break;
			case GraphicsFormat::ASTC5x5SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR; break;
			case GraphicsFormat::ASTC6x5UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_6x5_KHR; break;
			case GraphicsFormat::ASTC6x5SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR; break;
			case GraphicsFormat::ASTC6x6UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR; break;
			case GraphicsFormat::ASTC6x6SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR; break;
			case GraphicsFormat::ASTC8x5UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_8x5_KHR; break;
			case GraphicsFormat::ASTC8x5SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR; break;
			case GraphicsFormat::ASTC8x6UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_8x6_KHR; break;
			case GraphicsFormat::ASTC8x6SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR; break;
			case GraphicsFormat::ASTC8x8UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR; break;
			case GraphicsFormat::ASTC8x8SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR; break;
			case GraphicsFormat::ASTC10x5UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_10x5_KHR; break;
			case GraphicsFormat::ASTC10x5SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR; break;
			case GraphicsFormat::ASTC10x6UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_10x6_KHR; break;
			case GraphicsFormat::ASTC10x6SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR; break;
			case GraphicsFormat::ASTC10x8UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_10x8_KHR; break;
			case GraphicsFormat::ASTC10x8SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR; break;
			case GraphicsFormat::ASTC10x10UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_10x10_KHR; break;
			case GraphicsFormat::ASTC10x10SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR; break;
			case GraphicsFormat::ASTC12x10UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_12x10_KHR; break;
			case GraphicsFormat::ASTC12x10SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR; break;
			case GraphicsFormat::ASTC12x12UNormBlock:	     internalFormat = GL_COMPRESSED_RGBA_ASTC_12x12_KHR; break;
			case GraphicsFormat::ASTC12x12SRGBBlock:	     internalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR; break;
			default:
				assert(false);
				return GL_INVALID_ENUM;
			}

			return internalFormat;
		}

		GLenum
		OGLTypes::asCompareFunction(GraphicsCompareFunc func) noexcept
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
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asBlendFactor(GraphicsBlendFactor func) noexcept
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
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asBlendOperation(GraphicsBlendOp blendop) noexcept
		{
			switch (blendop)
			{
			case GraphicsBlendOp::Add:        return GL_FUNC_ADD;
			case GraphicsBlendOp::Subtract:   return GL_FUNC_SUBTRACT;
			case GraphicsBlendOp::RevSubtract:return GL_FUNC_REVERSE_SUBTRACT;
			default:
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asCullMode(GraphicsCullMode mode) noexcept
		{
			switch (mode)
			{
			case GraphicsCullMode::None:      return GL_NONE;
			case GraphicsCullMode::Front:     return GL_FRONT;
			case GraphicsCullMode::Back:      return GL_BACK;
			case GraphicsCullMode::FrontBack: return GL_FRONT_AND_BACK;
			default:
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asFrontFace(GraphicsFrontFace face) noexcept
		{
			switch (face)
			{
			case GraphicsFrontFace::CW:
				return GL_CW;
			case GraphicsFrontFace::CCW:
				return GL_CCW;
			default:
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asFillMode(GraphicsPolygonMode mode) noexcept
		{
			switch (mode)
			{
			case GraphicsPolygonMode::Point:     return GL_POINT;
			case GraphicsPolygonMode::Wireframe: return GL_LINE;
			case GraphicsPolygonMode::Solid:     return GL_FILL;
			default:
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asStencilOperation(GraphicsStencilOp stencilop) noexcept
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
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asSamplerWrap(GraphicsSamplerWrap wrap) noexcept
		{
			switch (wrap)
			{
			case GraphicsSamplerWrap::Repeat:      return GL_REPEAT;
			case GraphicsSamplerWrap::Mirror:      return GL_MIRRORED_REPEAT;
			case GraphicsSamplerWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
			default:
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asSamplerMinFilter(GraphicsSamplerFilter filter) noexcept
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
				return GL_INVALID_ENUM;
			}
		}

		GLenum
		OGLTypes::asSamplerMagFilter(GraphicsSamplerFilter filter) noexcept
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
				return GL_INVALID_ENUM;
			}
		}

		GLsizei
		OGLTypes::getFormatNum(GLenum format, GLenum type) noexcept
		{
			GLsizei typeSize = 0;
			if (type == GL_UNSIGNED_BYTE || type == GL_BYTE)
				typeSize = 1;
			else if (type == GL_UNSIGNED_SHORT || type == GL_SHORT || type == GL_HALF_FLOAT)
				typeSize = 2;
			else if (type == GL_UNSIGNED_INT || type == GL_INT || type == GL_FLOAT)
				typeSize = 4;
			else if (type == GL_DOUBLE)
				typeSize = 8;
			else
			{
				assert(false);
				return 1;
			}

			if (format == GL_RED)
				return 1 * typeSize;
			else if (format == GL_RG)
				return 2 * typeSize;
			else if (format == GL_RGB || format == GL_BGR || format == GL_SRGB)
				return 3 * typeSize;
			else if (format == GL_RGBA || format == GL_BGRA || format == GL_ABGR_EXT || format == GL_SRGB_ALPHA)
				return 4 * typeSize;
			else
			{
				assert(false);
				return 0;
			}
		}

		GLsizei
		OGLTypes::getCompressedTextureSize(GLsizei width, GLsizei height, GLsizei depth, GLenum internalFormat) noexcept
		{
			switch (internalFormat)
			{
			case GL_COMPRESSED_RGB_FXT1_3DFX:
			case GL_COMPRESSED_RGBA_FXT1_3DFX:
				width = (width + 7) & ~7;
				height = (height + 3) & ~3;
				return std::max(16, width * height / 2);
			case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			case GL_RGB_S3TC:
			case GL_RGB4_S3TC:
				width = (width + 3) & ~3;
				height = (height + 3) & ~3;
				return std::max(8, width * height / 2);
			case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			case GL_RGBA_S3TC:
			case GL_RGBA4_S3TC:
				width = (width + 3) & ~3;
				height = (height + 3) & ~3;
				return std::max(16, width * height);
			default:
				return 0;
			}
		}

		GLboolean
		OGLTypes::isNormFormat(GraphicsFormat format) noexcept
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
				return false;
			}
		}

		GLboolean
		OGLTypes::isStencilFormat(GraphicsFormat format) noexcept
		{
			if (format == GraphicsFormat::S8UInt)
				return GL_TRUE;
			return GL_FALSE;
		}

		GLboolean
		OGLTypes::isDepthFormat(GraphicsFormat format) noexcept
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
		OGLTypes::isDepthStencilFormat(GraphicsFormat format) noexcept
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
		OGLTypes::isCompressedTexture(GraphicsFormat format) noexcept
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
		OGLCheck::checkError() noexcept
		{
			bool success = true;

			GLenum result = glGetError();
			if (GL_NO_ERROR != result)
			{
				success = false;

				switch (result)
				{
				case GL_INVALID_ENUM:
					std::cout << "glGetError() fail : GL_INVALID_ENUM" << std::endl;
					break;
				case GL_INVALID_VALUE:
					std::cout << "glGetError() fail : GL_INVALID_VALUE" << std::endl;
					break;
				case GL_INVALID_OPERATION:
					std::cout << "glGetError() fail : GL_INVALID_OPERATION" << std::endl;
					break;
				case GL_STACK_OVERFLOW:
					std::cout << "glGetError() fail : GL_STACK_OVERFLOW" << std::endl;
					break;
				case GL_STACK_UNDERFLOW:
					std::cout << "glGetError() fail : GL_STACK_UNDERFLOW" << std::endl;
					break;
				case GL_OUT_OF_MEMORY:
					std::cout << "glGetError() fail : GL_OUT_OF_MEMORY" << std::endl;
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					std::cout << "glGetError() fail : GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
					break;
				default:
					std::cout << "glGetError() fail : Unknown" << std::endl;
				};
			}

			result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (GL_FRAMEBUFFER_COMPLETE != result)
			{
				success = false;

				switch (result)
				{
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					std::cout << "FBO:Incompleteattachment" << std::endl;
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					std::cout << "FBO:Incomplete missingattachment" << std::endl;
					break;
				case GL_FRAMEBUFFER_UNSUPPORTED:
					std::cout << "FBO:Unsupported" << std::endl;
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
					std::cout << "FBO:Incomplete drawbuffer" << std::endl;
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
					std::cout << "FBO:Incomplete readbuffer" << std::endl;
					break;
				default:
					std::cout << "FBO:Unknown" << std::endl;
				}
			}

		#if defined(OCTOON_BUILD_PLATFORM_LINUX)
			return true;
		#else
			return success;
		#endif
		}
	}
}