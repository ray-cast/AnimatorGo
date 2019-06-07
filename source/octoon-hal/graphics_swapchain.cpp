#include <octoon/hal/graphics_swapchain.h>

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubInterface(GraphicsSwapchain, GraphicsChild, "GraphicsSwapchain")

		GraphicsSwapchainDesc::GraphicsSwapchainDesc() noexcept
			: _format(GraphicsFormat::Undefined)
			, _depthStencilFormat(GraphicsFormat::Undefined)
			, _interval(GraphicsSwapInterval::Free)
			, _imageNums(2)
			, window_(0)
			, width_(0)
			, height_(0)
		{
		}

		GraphicsSwapchainDesc::~GraphicsSwapchainDesc() noexcept
		{
		}

		void
		GraphicsSwapchainDesc::setWindHandle(WindHandle hwnd) noexcept
		{
			window_ = hwnd;
		}

		WindHandle
		GraphicsSwapchainDesc::getWindHandle() const noexcept
		{
			return window_;
		}

		void
		GraphicsSwapchainDesc::setWidth(std::uint32_t width) noexcept
		{
			width_ = width;
		}

		std::uint32_t
		GraphicsSwapchainDesc::getWidth() const noexcept
		{
			return width_;
		}

		void
		GraphicsSwapchainDesc::setHeight(std::uint32_t height) noexcept
		{
			height_ = height;
		}

		std::uint32_t
		GraphicsSwapchainDesc::getHeight() const noexcept
		{
			return height_;
		}

		void
		GraphicsSwapchainDesc::setSwapInterval(GraphicsSwapInterval interval) noexcept
		{
			_interval = interval;
		}

		GraphicsSwapInterval
		GraphicsSwapchainDesc::getSwapInterval() const noexcept
		{
			return _interval;
		}

		void
		GraphicsSwapchainDesc::setColorFormat(GraphicsFormat format) noexcept
		{
			_format = format;
		}

		GraphicsFormat
		GraphicsSwapchainDesc::getColorFormat() const noexcept
		{
			return _format;
		}

		void
		GraphicsSwapchainDesc::setDepthStencilFormat(GraphicsFormat format) noexcept
		{
			assert(format == GraphicsFormat::D16UNorm ||
				format == GraphicsFormat::X8_D24UNormPack32 ||
				format == GraphicsFormat::D32_SFLOAT ||
				format == GraphicsFormat::D16UNorm_S8UInt ||
				format == GraphicsFormat::D24UNorm_S8UInt ||
				format == GraphicsFormat::D32_SFLOAT_S8UInt);
			_depthStencilFormat = format;
		}

		GraphicsFormat
		GraphicsSwapchainDesc::getDepthStencilFormat() const noexcept
		{
			return _depthStencilFormat;
		}

		void
		GraphicsSwapchainDesc::setImageNums(std::uint32_t imageNums) noexcept
		{
			_imageNums = imageNums;
		}

		std::uint32_t
		GraphicsSwapchainDesc::getImageNums() const noexcept
		{
			return _imageNums;
		}
	}
}