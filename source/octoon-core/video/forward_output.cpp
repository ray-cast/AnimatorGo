#include <octoon/video/forward_output.h>

namespace octoon
{
	ForwardOutput::ForwardOutput(hal::GraphicsContextPtr context, std::uint32_t w, std::uint32_t h)
		: Output(w, h)
		, context_(context)
	{
	}

	void
	ForwardOutput::getData(math::float4* data) const
	{
	}

	void
	ForwardOutput::getData(math::float4* data, std::size_t offset, std::size_t elems_count) const
	{
	}

	void
	ForwardOutput::clear(math::float4 const& val)
	{
	}

	const hal::GraphicsTexturePtr&
	ForwardOutput::data() const noexcept
	{
		return data_;
	}
}