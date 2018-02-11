#ifndef OCTOON_GRAPHICS_SAMPLER_H_
#define OCTOON_GRAPHICS_SAMPLER_H_

#include <octoon/graphics/graphics_child.h>

namespace octoon
{
	namespace graphics
	{
		class OCTOON_EXPORT GraphicsSamplerDesc final
		{
		public:
			GraphicsSamplerDesc() noexcept;
			GraphicsSamplerDesc(GraphicsSamplerWrap wrap, GraphicsSamplerFilter filter, GraphicsSamplerAnis anis = GraphicsSamplerAnis::Anis0) noexcept;
			~GraphicsSamplerDesc() noexcept;

			void setSamplerWrap(GraphicsSamplerWrap wrap) noexcept;
			void setSamplerAnis(GraphicsSamplerAnis anis) noexcept;
			void setSamplerFilter(GraphicsSamplerFilter filter) noexcept;

			GraphicsSamplerWrap getSamplerWrap() const noexcept;
			GraphicsSamplerAnis getSamplerAnis() const noexcept;
			GraphicsSamplerFilter getSamplerFilter() const noexcept;

		private:
			GraphicsSamplerWrap _wrap;
			GraphicsSamplerAnis _anis;
			GraphicsSamplerFilter _filter;
		};

		class OCTOON_EXPORT GraphicsSampler : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsSampler, GraphicsChild)
		public:
			GraphicsSampler() noexcept = default;
			virtual ~GraphicsSampler() = default;

			virtual const GraphicsSamplerDesc& getGraphicsSamplerDesc() const noexcept = 0;

		private:
			GraphicsSampler(const GraphicsSampler&) noexcept = delete;
			GraphicsSampler& operator=(const GraphicsSampler&) noexcept = delete;
		};
	}
}

#endif