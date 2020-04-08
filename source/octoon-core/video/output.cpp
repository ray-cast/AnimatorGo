#include "output.h"

namespace octoon::video
{
    Output::Output(std::uint32_t w, std::uint32_t h) noexcept
        : width_(w)
        , height_(h)
    {
    }

    Output::~Output() noexcept
    {
    }

    std::uint32_t
    Output::width() const
    {
        return width_;
    }

    std::uint32_t
    Output::height() const
    {
        return height_;
    }
}