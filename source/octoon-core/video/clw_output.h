#ifndef OCTOON_VIDEO_CLW_OUTPUT_H_
#define OCTOON_VIDEO_CLW_OUTPUT_H_

#include <CLW.h>

#include "output.h"

namespace octoon::video
{
    class ClwOutput : public Output
    {
    public:
        ClwOutput(CLWContext context, std::uint32_t w, std::uint32_t h);

        void getData(math::float4* data) const override;
        void getData(math::float4* data, std::size_t offset, std::size_t elems_count) const override;

        void clear(math::float4 const& val) override;

        const CLWBuffer<math::float4>& data() const noexcept;

    private:
        CLWContext context_;
        CLWBuffer<math::float4> data_;
    };
}

#endif