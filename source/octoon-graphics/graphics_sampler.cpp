#include <octoon/graphics/graphics_sampler.h>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(GraphicsSampler, GraphicsChild, "GraphicsSampler")

		GraphicsSamplerDesc::GraphicsSamplerDesc() noexcept
			: _wrap(GraphicsSamplerWrap::Repeat)
			, _filterMin(GraphicsSamplerFilter::Linear)
			, _filterMag(GraphicsSamplerFilter::Linear)
			, _anis(GraphicsSamplerAnis::Anis0)
		{
		}

		GraphicsSamplerDesc::GraphicsSamplerDesc(GraphicsSamplerWrap wrap, GraphicsSamplerFilter filter, GraphicsSamplerAnis anis) noexcept
			: _wrap(wrap)
			, _filterMin(filter)
			, _filterMag(filter)
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
		GraphicsSamplerDesc::setSamplerMinFilter(GraphicsSamplerFilter filter) noexcept
		{
			_filterMin = filter;
		}

		void
		GraphicsSamplerDesc::setSamplerMagFilter(GraphicsSamplerFilter filter) noexcept
		{
			_filterMag = filter;
		}

		void
		GraphicsSamplerDesc::setSamplerFilter(GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept
		{
			_filterMin = minFilter;
			_filterMag = magFilter;
		}

		void
		GraphicsSamplerDesc::setSamplerAnis(GraphicsSamplerAnis anis) noexcept
		{
			_anis = anis;
		}

		GraphicsSamplerWrap
		GraphicsSamplerDesc::getSamplerWrap() const noexcept
		{
			return _wrap;
		}

		GraphicsSamplerFilter
		GraphicsSamplerDesc::getSamplerMinFilter() const noexcept
		{
			return _filterMin;
		}

		GraphicsSamplerFilter
		GraphicsSamplerDesc::getSamplerMagFilter() const noexcept
		{
			return _filterMag;
		}

		GraphicsSamplerAnis
		GraphicsSamplerDesc::getSamplerAnis() const noexcept
		{
			return _anis;
		}
	}
}