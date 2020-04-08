#include "clw_output.h"

namespace octoon::video
{
    ClwOutput::ClwOutput(CLWContext context, std::uint32_t w, std::uint32_t h)
        : Output(w, h)
        , context_(context)
        , data_(context.CreateBuffer<math::float4>(w * h, CL_MEM_READ_WRITE))
    {
    }

    void
    ClwOutput::getData(math::float4* data) const
    {
        context_.ReadBuffer(0, data_, data, data_.GetElementCount()).Wait();
    }

    void
    ClwOutput::getData(math::float4* data, std::size_t offset, std::size_t elems_count) const
    {
        context_.ReadBuffer(0, data_, data, offset, elems_count).Wait();
    }

    void
    ClwOutput::clear(math::float4 const& val)
    {
        context_.FillBuffer(0, data_, val, data_.GetElementCount()).Wait();
    }

    const CLWBuffer<math::float4>&
    ClwOutput::data() const  noexcept
    {
        return data_;
    }
}