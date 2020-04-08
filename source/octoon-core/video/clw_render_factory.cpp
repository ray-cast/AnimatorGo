#include "clw_render_factory.h"
#include "clw_output.h"

namespace octoon::video
{
    ClwRenderFactory::ClwRenderFactory(CLWContext context, std::string_view cache_path) noexcept
        : context_(context)
        , intersector_(RadeonRays::CreateFromOpenClContext(context, context.GetDevice(0).GetID(), context.GetCommandQueue(0)), RadeonRays::IntersectionApi::Delete)
    {
    }

    ClwRenderFactory::~ClwRenderFactory() noexcept
    {
    }

    std::unique_ptr<Output>
    ClwRenderFactory::CreateOutput(std::uint32_t w, std::uint32_t h) const
    {
        return std::unique_ptr<Output>(new ClwOutput(context_, w, h));
    }
}