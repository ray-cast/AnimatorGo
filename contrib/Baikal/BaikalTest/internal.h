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
#include "gtest/gtest.h"

#include "Utils/distribution1d.h"
#include "math/mathutils.h"

class InternalTest : public ::testing::Test
{

};

TEST_F(InternalTest, Distribuiton1D)
{
    float vals[] = { 2, 4, 6, 8 };
    Baikal::Distribution1D dist(vals, 4);

    int cnts[]{ 0, 0, 0, 0 };

    for (auto i = 0u; i < 1000; ++i)
    {
        float pdf = 0.f;
        float v = dist.Sample1D(RadeonRays::rand_float(), pdf);

        int idx = std::min((int)(v * 4), 3);
        ++cnts[idx];
    }

    cnts[0] += cnts[1];
}
