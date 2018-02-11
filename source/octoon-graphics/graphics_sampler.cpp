#include <octoon/graphics/graphics_sampler.h>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(GraphicsSampler, GraphicsChild, "GraphicsSampler")

		GraphicsSamplerDesc::GraphicsSamplerDesc() noexcept
			: _wrap(GraphicsSamplerWrap::Repeat)
			, _filter(GraphicsSamplerFilter::Nearest)
			, _anis(GraphicsSamplerAnis::Anis0)
		{
		}

		GraphicsSamplerDesc::GraphicsSamplerDesc(GraphicsSamplerWrap wrap, GraphicsSamplerFilter filter, GraphicsSamplerAnis anis) noexcept
			: _wrap(wrap)
			, _filter(filter)
			, _anis(anis)
		{
		}

		GraphicsSamplerDesc::~GraphicsSamplerDesc() noexcept
		{
		}

		void
		GraphicsSamplerDesc::setSamplerWrap(GraphicsSamplerWrap wrap) noexcept
		{
			_wrap = wrap;
		}

		void
		GraphicsSamplerDesc::setSamplerAnis(GraphicsSamplerAnis anis) noexcept
		{
			_anis = anis;
		}

		void
		GraphicsSamplerDesc::setSamplerFilter(GraphicsSamplerFilter filter) noexcept
		{
			_filter = filter;
		}

		GraphicsSamplerWrap
		GraphicsSamplerDesc::getSamplerWrap() const noexcept
		{
			return _wrap;
		}

		GraphicsSamplerAnis
		GraphicsSamplerDesc::getSamplerAnis() const noexcept
		{
			return _anis;
		}

		GraphicsSamplerFilter
		GraphicsSamplerDesc::getSamplerFilter() const noexcept
		{
			return _filter;
		}
	}
}