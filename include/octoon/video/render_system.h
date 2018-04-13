#ifndef OCTOON_RENDER_SYSTEM_H_
#define OCTOON_RENDER_SYSTEM_H_

#include <octoon/runtime/singleton.h>
#include <octoon/video/render_types.h>

namespace octoon
{
	namespace video
	{
		class RenderSystem final : public runtime::Singleton<RenderSystem>
		{
		public:
			RenderSystem() noexcept;
			~RenderSystem() noexcept;

			void setWindowSize(std::uint32_t w, std::uint32_t h) noexcept;
			void getWindowSize(std::uint32_t& w, std::uint32_t& h) const noexcept;

			void render() noexcept;

		private:
			std::uint32_t width_, height_;
		};
	}
}

#endif