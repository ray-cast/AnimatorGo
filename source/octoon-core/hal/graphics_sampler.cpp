#include <octoon/hal/graphics_sampler.h>

namespace octoon
{
	OctoonImplementSubInterface(GraphicsSampler, GraphicsResource, "GraphicsSampler")

	GraphicsSamplerDesc::GraphicsSamplerDesc() noexcept
		: _wrap(SamplerWrap::Repeat)
		, _filterMin(SamplerFilter::Linear)
		, _filterMag(SamplerFilter::Linear)
		, _anis(GraphicsSamplerAnis::Anis0)
	{
	}

	GraphicsSamplerDesc::GraphicsSamplerDesc(SamplerWrap wrap, SamplerFilter filter, GraphicsSamplerAnis anis) noexcept
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
	GraphicsSamplerDesc::setSamplerWrap(SamplerWrap wrap) noexcept
	{
		_wrap = wrap;
	}

	void
	GraphicsSamplerDesc::setSamplerMinFilter(SamplerFilter filter) noexcept
	{
		_filterMin = filter;
	}

	void
	GraphicsSamplerDesc::setSamplerMagFilter(SamplerFilter filter) noexcept
	{
		_filterMag = filter;
	}

	void
	GraphicsSamplerDesc::setSamplerFilter(SamplerFilter minFilter, SamplerFilter magFilter) noexcept
	{
		_filterMin = minFilter;
		_filterMag = magFilter;
	}

	void
	GraphicsSamplerDesc::setSamplerAnis(GraphicsSamplerAnis anis) noexcept
	{
		_anis = anis;
	}

	SamplerWrap
	GraphicsSamplerDesc::getSamplerWrap() const noexcept
	{
		return _wrap;
	}

	SamplerFilter
	GraphicsSamplerDesc::getSamplerMinFilter() const noexcept
	{
		return _filterMin;
	}

	SamplerFilter
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