#ifndef OCTOON_VIDEO_CLW_OUTPUT_H_
#define OCTOON_VIDEO_CLW_OUTPUT_H_

#include <CLW.h>

#include "output.h"

namespace octoon
{
	class ClwOutput : public Output
	{
	public:
		ClwOutput(CLWContext context, std::uint32_t w, std::uint32_t h);

		virtual void getData(math::float4* data) const override;
		virtual void getData(math::float4* data, std::size_t offset, std::size_t elems_count) const override;

		virtual void clear(math::float4 const& val) override;

		virtual const CLWBuffer<math::float4>& data() const noexcept;

	protected:
		CLWContext context_;
		CLWBuffer<math::float4> data_;
	};
}

#endif