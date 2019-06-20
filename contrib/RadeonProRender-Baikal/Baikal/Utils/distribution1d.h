/**********************************************************************
Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/
#pragma once

#include <cstdint>
#include <vector>

namespace Baikal
{
    ///< The class represents 1D piecewise constant distribution of random variable.
    ///< The PDF is proprtional to passed function defined at N points in [0,1] interval
    ///< Partially taken from Pharr & Humphreys, but a bug with lower bound fixed.
    ///<
    struct Distribution1D
    {
    public:
        // values are function values at equal spacing at numsegments points within [0,1] range
        Distribution1D();
        Distribution1D(float const* values, std::uint32_t num_segments);

        void Set(float const* values, std::uint32_t num_segments);

        // Sample one value using this distribution
        // u is uniformely distributed random var
        float Sample1D(float u, float& pdf) const;

        // PDF
        float pdf(float u) const;

        // Function values
        std::vector<float> m_func_values;
        // Cumulative distribution function
        std::vector<float> m_cdf;
        // Number of segments
        std::uint32_t m_num_segments;
        // Integral of the function over the whole range (normalizer)
        float m_func_sum;
    };
}