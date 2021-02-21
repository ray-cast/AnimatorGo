#ifndef OCTOON_VIDEO_FORWARD_OUTPUT_H_
#define OCTOON_VIDEO_FORWARD_OUTPUT_H_

#include <octoon/hal/graphics_context.h>
#include "output.h"

namespace octoon
{
	class OCTOON_EXPORT ForwardOutput final : public Output
	{
	public:
		ForwardOutput(hal::GraphicsContextPtr context, std::uint32_t w, std::uint32_t h);

		void getData(math::float4* data) const override;
		void getData(math::float4* data, std::size_t offset, std::size_t elems_count) const override;

		void clear(math::float4 const& val) override;

		const hal::GraphicsTexturePtr& ForwardOutput::data() const noexcept;

	private:
		ForwardOutput(const ForwardOutput&) = delete;
		ForwardOutput& operator=(const ForwardOutput&) = delete;

	private:
		hal::GraphicsContextPtr context_;
		hal::GraphicsTexturePtr data_;
	};
}

#endif