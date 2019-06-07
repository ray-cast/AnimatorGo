#ifndef OCTOON_GRAPHICS_SAMPLER_H_
#define OCTOON_GRAPHICS_SAMPLER_H_

#include <octoon/hal/graphics_child.h>

namespace octoon
{
	namespace hal
	{
		class OCTOON_EXPORT GraphicsSamplerDesc final
		{
		public:
			GraphicsSamplerDesc() noexcept;
			GraphicsSamplerDesc(GraphicsSamplerWrap wrap, GraphicsSamplerFilter filter, GraphicsSamplerAnis anis = GraphicsSamplerAnis::Anis0) noexcept;
			~GraphicsSamplerDesc() noexcept;

			void setSamplerWrap(GraphicsSamplerWrap wrap) noexcept;
			void setSamplerMinFilter(GraphicsSamplerFilter filter) noexcept;
			void setSamplerMagFilter(GraphicsSamplerFilter filter) noexcept;
			void setSamplerFilter(GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept;
			void setSamplerAnis(GraphicsSamplerAnis anis) noexcept;

			GraphicsSamplerWrap getSamplerWrap() const noexcept;
			GraphicsSamplerFilter getSamplerMinFilter() const noexcept;
			GraphicsSamplerFilter getSamplerMagFilter() const noexcept;
			GraphicsSamplerAnis getSamplerAnis() const noexcept;

		private:
			GraphicsSamplerAnis _anis;
			GraphicsSamplerWrap _wrap;
			GraphicsSamplerFilter _filterMin;
			GraphicsSamplerFilter _filterMag;
		};

		class OCTOON_EXPORT GraphicsSampler : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsSampler, GraphicsChild)
		public:
			GraphicsSampler() noexcept = default;
			virtual ~GraphicsSampler() = default;

			virtual const GraphicsSamplerDesc& getSamplerDesc() const noexcept = 0;

		private:
			GraphicsSampler(const GraphicsSampler&) noexcept = delete;
			GraphicsSampler& operator=(const GraphicsSampler&) noexcept = delete;
		};
	}
}

#endif