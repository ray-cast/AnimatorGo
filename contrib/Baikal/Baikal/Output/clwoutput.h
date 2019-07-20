#pragma once

#include "output.h"
#include "CLW.h"

namespace Baikal
{
    class ClwOutput : public Output
    {
    public:
        ClwOutput(CLWContext context, std::uint32_t w, std::uint32_t h)
        : Output(w, h)
        , m_context(context)
        , m_data(context.CreateBuffer<RadeonRays::float3>(w*h, CL_MEM_READ_WRITE))
        {
        }

        void GetData(RadeonRays::float3* data) const override
        {
            m_context.ReadBuffer(0, m_data, data, m_data.GetElementCount()).Wait();
        }

        void GetData(RadeonRays::float3* data, /* offset in elems */ size_t offset, /* read elems */size_t elems_count) const override
        {
            m_context.ReadBuffer(
                0,
                m_data,
                data,
                offset,
                elems_count).Wait();
        }

        void Clear(RadeonRays::float3 const& val) override
        {
            m_context.FillBuffer(0, m_data, val, m_data.GetElementCount()).Wait();
        }

        CLWBuffer<RadeonRays::float3> data() const { return m_data; }

    private:
        CLWContext m_context;
        CLWBuffer<RadeonRays::float3> m_data;
    };
}
