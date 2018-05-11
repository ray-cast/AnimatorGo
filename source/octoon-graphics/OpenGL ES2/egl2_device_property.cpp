#include "egl2_device_property.h"

namespace octoon 
{
	namespace graphics
	{
		EGL2DeviceProperty::EGL2DeviceProperty() noexcept
		{
		}

		EGL2DeviceProperty::~EGL2DeviceProperty() noexcept
		{
			this->close();
		}

		bool
		EGL2DeviceProperty::setup(const GraphicsDeviceDesc& deviceDesc) noexcept
		{
			EGLConfig config;
			if (!initPixelFormat(deviceDesc, config))
				return false;

			if (!initContext(deviceDesc, config))
				return false;

			return true;
		}

		void
		EGL2DeviceProperty::close() noexcept
		{
		}

		bool
		EGL2DeviceProperty::initPixelFormat(const GraphicsDeviceDesc& deviceDesc, EGLConfig& config) noexcept
		{
			EGLint pixelFormat[80];
			EGLint index = 0;

			pixelFormat[index++] = EGL_SURFACE_TYPE;
			pixelFormat[index++] = EGL_WINDOW_BIT;

			pixelFormat[index++] = EGL_RENDERABLE_TYPE;
			pixelFormat[index++] = EGL_OPENGL_ES2_BIT;

			pixelFormat[index++] = EGL_BUFFER_SIZE;
			pixelFormat[index++] = 32;

			pixelFormat[index++] = EGL_RED_SIZE;
			pixelFormat[index++] = 8;

			pixelFormat[index++] = EGL_GREEN_SIZE;
			pixelFormat[index++] = 8;

			pixelFormat[index++] = EGL_BLUE_SIZE;
			pixelFormat[index++] = 8;

			pixelFormat[index++] = EGL_ALPHA_SIZE;
			pixelFormat[index++] = 8;

			pixelFormat[index++] = EGL_DEPTH_SIZE;
			pixelFormat[index++] = 16;

			pixelFormat[index++] = EGL_STENCIL_SIZE;
			pixelFormat[index++] = 0;

			pixelFormat[index++] = EGL_NONE;

			auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
			if (display == EGL_NO_DISPLAY)
			{
				GL_PLATFORM_LOG("eglGetDisplay() fail.");
				return false;
			}

			if (::eglInitialize(display, nullptr, nullptr) == EGL_FALSE)
			{
				GL_PLATFORM_LOG("eglInitialize() fail.");
				return false;
			}

			if (::eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
			{
				GL_PLATFORM_LOG("eglBindAPI() fail.");
				return false;
			}

			EGLint num = 0;
			if (::eglChooseConfig(display, pixelFormat, &config, 1, &num) == EGL_FALSE)
			{
				GL_PLATFORM_LOG("eglChooseConfig() fail.");
				return false;
			}

			return true;
		}

		bool
		EGL2DeviceProperty::initContext(const GraphicsDeviceDesc& deviceDesc, EGLConfig config) noexcept
		{
			EGLint attribIndex = 0;

			EGLint attribs[80];

		#if !defined(_BUILD_PLATFORM_ANDROID)
			attribs[attribIndex++] = EGL_CONTEXT_MAJOR_VERSION;
			attribs[attribIndex++] = 2;

			attribs[attribIndex++] = EGL_CONTEXT_MINOR_VERSION;
			attribs[attribIndex++] = 0;
		#else
			attribs[attribIndex++] = EGL_CONTEXT_CLIENT_VERSION;
			attribs[attribIndex++] = 2;
		#endif

			attribs[attribIndex++] = EGL_NONE;
			attribs[attribIndex++] = EGL_NONE;

			EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

			EGLNativeWindowType hwnd = nullptr;// (EGLNativeWindowType)deviceDesc.getWindHandle();
			EGLSurface surface = ::eglCreateWindowSurface(display, config, hwnd, NULL);
			if (::eglGetError() != EGL_SUCCESS)
			{
				GL_PLATFORM_LOG("eglCreateContext() fail.");
				return false;
			}

			EGLContext context = ::eglCreateContext(display, config, EGL_NO_CONTEXT, attribs);
			if (eglGetError() != EGL_SUCCESS)
			{
				::eglDestroySurface(display, surface);
				GL_PLATFORM_LOG("eglCreateContext() fail.");
				return false;
			}

			::eglMakeCurrent(display, surface, surface, context);

			this->initDeviceProperties();

			::eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

			::eglDestroyContext(display, context);
			::eglDestroySurface(display, surface);

			return true;
		}

		bool
		EGL2DeviceProperty::initDeviceProperties() noexcept
		{
			static_assert(sizeof(GLint) == sizeof(std::uint32_t), "not match");

			EGL2Types::setup();

			this->initTextureSupports();
			this->initVertexSupports();
			this->initTextureDimSupports();
			this->initShaderSupports();

			glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension1D);
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimension2D);
			glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, (GLint*)&_deviceProperties.maxImageDimensionCube);

			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxVertexInputAttributes);
			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxVertexInputBindings);
			glGetIntegerv(GL_MAX_VARYING_VECTORS, (GLint*)&_deviceProperties.maxVertexOutputComponents);

			glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&_deviceProperties.maxFragmentInputComponents);

			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subPixelPrecisionBits);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.subTexelPrecisionBits);
			glGetIntegerv(GL_SUBPIXEL_BITS, (GLint*)&_deviceProperties.mipmapPrecisionBits);

			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, (GLint*)&_deviceProperties.maxViewportDimensionsW);
	
			glGetIntegerv(GL_LINE_WIDTH, (GLint*)&_deviceProperties.strictLines);

			return true;
		}

		bool
		EGL2DeviceProperty::initTextureSupports() noexcept
		{
			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_required_internalformat))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R4G4UNormPack8);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R4G4B4A4UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B4G4R4A4UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B5G5R5A1UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B5G6R5UNormPack16);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R5G6B5UNormPack16);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_read_format_bgra))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A1R5G5B5UNormPack16);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_ARM_rgba8) || EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_rgb8_rgba8))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8UNorm);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_format_BGRA8888) || EGL2Types::isSupportFeature(EGL2Features::EGL2_APPLE_texture_format_BGRA8888))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8UNorm);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_sRGB))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8B8A8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B8G8R8A8SRGB);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A8B8G8R8SRGBPack32);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_NV_sRGB_formats))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBSRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBASRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC3SRGBBlock);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_rg))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R8G8UNorm);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_norm16))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16UNorm);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SNorm);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_texture_half_float) || EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_color_buffer_half_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R16G16B16A16SFloat);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_texture_float) || EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_color_buffer_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32SFloat);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::R32G32B32A32SFloat);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_type_2_10_10_10_REV))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2R10G10B10UNormPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::A2B10G10R10UNormPack32);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_APPLE_texture_packed_float))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::B10G11R11UFloatPack32);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::E5B9G9R9UFloatPack32);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_depth_texture))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D16UNorm);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_depth24))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::X8_D24UNormPack32);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_depth32))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::D32_SFLOAT);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_texture_stencil8))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::S8UInt);
			}

			/*if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_packed_depth_stencil))
			{
			_deviceProperties.supportTextures.push_back(GraphicsFormat::D24UNorm_S8UInt);
			}*/

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_compression_dxt1))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBUNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC1RGBAUNormBlock);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_compression_s3tc))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC2UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::BC3UNormBlock);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_KHR_texture_compression_astc_ldr))
			{
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC4x4UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC4x4SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x4UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x4SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC5x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x6UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC6x6SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x6UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x6SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x8UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC8x8SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x5UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x5SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x6UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x6SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x8UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x8SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x10UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC10x10SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x10UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x10SRGBBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x12UNormBlock);
				_deviceProperties.supportTextures.push_back(GraphicsFormat::ASTC12x12SRGBBlock);
			}

			return true;
		}

		bool
		EGL2DeviceProperty::initTextureDimSupports() noexcept
		{
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Texture2D);
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Texture2DArray);
			_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::Cube);

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_EXT_texture_cube_map_array))
				_deviceProperties.supportTextureDims.push_back(GraphicsTextureDim::CubeArray);

			return true;
		}

		bool
		EGL2DeviceProperty::initVertexSupports() noexcept
		{
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8A8SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8A8UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8A8SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R8G8B8A8UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16A16SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16A16UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16A16SNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16G16B16A16UNorm);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32A32SInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32A32UInt);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32SFloat);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32SFloat);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32SFloat);
			_deviceProperties.supportAttribute.push_back(GraphicsFormat::R32G32B32A32SFloat);

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_vertex_type_10_10_10_2))
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10SIntPack32);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10UIntPack32);
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::A2R10G10B10UNormPack32);
			}

			if (EGL2Types::isSupportFeature(EGL2Features::EGL2_OES_vertex_half_float))
			{
				_deviceProperties.supportAttribute.push_back(GraphicsFormat::R16SFloat);
			}

			return true;
		}

		bool
		EGL2DeviceProperty::initShaderSupports() noexcept
		{
			_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::VertexBit);
			_deviceProperties.supportShaders.push_back(GraphicsShaderStageFlagBits::FragmentBit);
			return true;
		}

		const GraphicsDeviceProperties&
		EGL2DeviceProperty::getDeviceProperties() const noexcept
		{
			return _deviceProperties;
		}
	}
}
