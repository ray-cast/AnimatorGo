#ifndef OCTOON_GRAPHICS_SAMPLER_H_
#define OCTOON_GRAPHICS_SAMPLER_H_

#include <octoon/hal/graphics_resource.h>

namespace octoon
{
	class OCTOON_EXPORT GraphicsSamplerDesc final
	{
	public:
		GraphicsSamplerDesc() noexcept;
		GraphicsSamplerDesc(SamplerWrap wrap, SamplerFilter filter, GraphicsSamplerAnis anis = GraphicsSamplerAnis::Anis0) noexcept;
		~GraphicsSamplerDesc() noexcept;

		void setSamplerWrap(SamplerWrap wrap) noexcept;
		void setSamplerMinFilter(SamplerFilter filter) noexcept;
		void setSamplerMagFilter(SamplerFilter filter) noexcept;
		void setSamplerFilter(SamplerFilter minFilter, SamplerFilter magFilter) noexcept;
		void setSamplerAnis(GraphicsSamplerAnis anis) noexcept;

		SamplerWrap getSamplerWrap() const noexcept;
		SamplerFilter getSamplerMinFilter() const noexcept;
		SamplerFilter getSamplerMagFilter() const noexcept;
		GraphicsSamplerAnis getSamplerAnis() const noexcept;

	private:
		GraphicsSamplerAnis _anis;
		SamplerWrap _wrap;
		SamplerFilter _filterMin;
		SamplerFilter _filterMag;
	};

	class OCTOON_EXPORT GraphicsSampler : public GraphicsResource
	{
		OctoonDeclareSubInterface(GraphicsSampler, GraphicsResource)
	public:
		GraphicsSampler() noexcept = default;
		virtual ~GraphicsSampler() = default;

		virtual const GraphicsSamplerDesc& getSamplerDesc() const noexcept = 0;

	private:
		GraphicsSampler(const GraphicsSampler&) noexcept = delete;
		GraphicsSampler& operator=(const GraphicsSampler&) noexcept = delete;
	};
}

#endif