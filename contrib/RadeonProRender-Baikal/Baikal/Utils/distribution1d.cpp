#include "distribution1d.h"
#include "math/mathutils.h"

#include <algorithm>
#include <cassert>

namespace Baikal
{
    Distribution1D::Distribution1D()
        : m_num_segments(0u)
    {
    }

    Distribution1D::Distribution1D(float const* values, std::uint32_t num_segments)
    {
        Set(values, num_segments);
    }

    void Distribution1D::Set(float const* values, std::uint32_t num_segments)
    {
        assert(num_segments > 0);
        m_num_segments = num_segments;
        m_func_values.resize(num_segments);
        m_cdf.resize(num_segments + 1);

        // Copy function values
        std::copy(values, values + num_segments, m_func_values.begin());

        // Calculate CDF
        m_cdf[0] = 0.f;
        for (auto i = 1u; i < num_segments + 1; ++i)
        {
            m_cdf[i] = m_cdf[i - 1] + m_func_values[i - 1] / num_segments;
        }

        // Calculate normalizer
        m_func_sum = m_cdf[num_segments];

        // Normalize CDF
        for (auto i = 0u; i < num_segments + 1; ++i)
        {
            m_cdf[i] /= m_func_sum;
        }
    }

    float Distribution1D::Sample1D(float u, float& pdf) const
    {
        assert(m_num_segments > 0);

        // Find the segment here u lies
        // this one is in [0, N-1] and we need to handle corner case:
        // for 0 it returns 0 index
        auto iter = std::lower_bound(m_cdf.cbegin(), m_cdf.cend(), u); 

        // Find segment index : clamp it as last may be returned for 1
        auto segment_idx = std::max((std::uint32_t)std::distance(m_cdf.cbegin(), iter), 1u);

        assert(u >= m_cdf[segment_idx - 1] && u <= m_cdf[segment_idx]);

        // Find lerp coefficient
        float du = (u - m_cdf[segment_idx - 1]) / (m_cdf[segment_idx] - m_cdf[segment_idx - 1]);

        // Calc pdf
        pdf = m_func_values[segment_idx - 1] / m_func_sum;

        // Return corresponding value
        return (segment_idx - 1 + du) / m_num_segments;
    }

    float Distribution1D::pdf(float u) const
    {
        // Find the segment here u lies
        auto iter = std::lower_bound(m_cdf.cbegin(), m_cdf.cend(), u);

        // Find segment index : clamp it as last may be returned for 1
        auto segment_idx = std::max((std::uint32_t)std::distance(m_cdf.cbegin(), iter), 1u);

        // Calc pdf
        return m_func_values[segment_idx - 1] / m_func_sum;
    }
}