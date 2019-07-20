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

#include "basic.h"
#include "SceneGraph/light.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/material.h"
#include "image_io.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace RadeonRays;

class UberV2Test : public BasicTest
{

protected:

    void LoadTestScene() override
    {
        ASSERT_NO_THROW(m_output = m_factory->CreateOutput(1024, 1024));
        m_output->Clear(RadeonRays::float3(0.0f));
        ASSERT_NO_THROW(m_renderer->SetOutput(Baikal::Renderer::OutputType::kColor, m_output.get()));

        m_scene = Baikal::SceneIo::LoadScene("uberv2_test_spheres.test", "");
    }
};

TEST_F(UberV2Test, UberV2_Basic)
{
    m_camera->LookAt(
        RadeonRays::float3(0.f, 0.f, 10.f),
        RadeonRays::float3(0.f, 0.f, 9.f),
        RadeonRays::float3(0.f, 1.f, 0.f));

    ASSERT_NO_THROW(m_controller->CompileScene(m_scene));

    auto& scene = m_controller->GetCachedScene(m_scene);

    for (auto i = 0u; i < kNumIterations; ++i)
    {
        ASSERT_NO_THROW(m_renderer->Render(scene));
    }

    {
        std::ostringstream oss;
        oss << test_name() << ".png";
        SaveOutput(oss.str());
        ASSERT_TRUE(CompareToReference(oss.str()));
    }
}
