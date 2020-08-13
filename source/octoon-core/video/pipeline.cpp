#include "pipeline.h"
#include <stdexcept>

namespace octoon::video
{
	Pipeline::Pipeline() noexcept
	{
        std::fill(outputs_.begin(), outputs_.end(), nullptr);
	}

	Pipeline::~Pipeline() noexcept
	{
	}

    void 
    Pipeline::setOutput(OutputType type, Output* output)
    {
        auto idx = static_cast<std::size_t>(type);
        if (idx >= static_cast<std::size_t>(OutputType::kMax))
            throw std::out_of_range("Output type is out of supported range");

        outputs_[idx] = output;
    }

    Output*
    Pipeline::getOutput(OutputType type) const
    {
        auto idx = static_cast<std::size_t>(type);
        if (idx >= static_cast<std::size_t>(OutputType::kMax))
            throw std::out_of_range("Output type is out of supported range");
        return outputs_[idx];
    }

}