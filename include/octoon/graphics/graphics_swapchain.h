#ifndef OCTOON_GRAPHICS_SWAPCHAIN_H_
#define OCTOON_GRAPHICS_SWAPCHINA_H_

#include <octoon/graphics/graphics_child.h>

namespace octoon
{
	namespace graphics
	{
		class OCTOON_EXPORT GraphicsSwapchainDesc final
		{
		public:
			GraphicsSwapchainDesc() noexcept;
			~GraphicsSwapchainDesc() noexcept;

			void setWindHandle(WindHandle hwnd) noexcept;
			WindHandle getWindHandle() const noexcept;

			void setWidth(std::uint32_t width) noexcept;
			std::uint32_t getWidth() const noexcept;

			void setHeight(std::uint32_t height) noexcept;
			std::uint32_t getHeight() const noexcept;

			void setSwapInterval(GraphicsSwapInterval interval) noexcept;
			GraphicsSwapInterval getSwapInterval() const noexcept;

			void setColorFormat(GraphicsFormat format) noexcept;
			GraphicsFormat getColorFormat() const noexcept;

			void setDepthStencilFormat(GraphicsFormat format) noexcept;
			GraphicsFormat getDepthStencilFormat() const noexcept;

			void setImageNums(std::uint32_t imageNums) noexcept;
			std::uint32_t getImageNums() const noexcept;

		private:
			WindHandle window_;
			std::uint32_t width_;
			std::uint32_t height_;

			std::uint32_t _imageNums;

			GraphicsSwapInterval _interval;

			GraphicsFormat _format;
			GraphicsFormat _depthStencilFormat;
		};

		class OCTOON_EXPORT GraphicsSwapchain : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsSwapchain, GraphicsChild)
		public:
			GraphicsSwapchain() noexcept = default;
			virtual ~GraphicsSwapchain() = default;

			virtual void setSwapInterval(GraphicsSwapInterval interval) noexcept = 0;
			virtual GraphicsSwapInterval getSwapInterval() const noexcept = 0;

			virtual void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept = 0;
			virtual void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept = 0;

			virtual const GraphicsSwapchainDesc& getGraphicsSwapchainDesc() const noexcept = 0;

		private:
			GraphicsSwapchain(const GraphicsSwapchain&) = delete;
			GraphicsSwapchain& operator=(const GraphicsSwapchain&) = delete;
		};
	}
}

#endif