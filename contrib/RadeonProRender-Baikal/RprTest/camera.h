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

class CameraTest : public BasicTest
{
public:
    virtual void SetUp() override
    {
        BasicTest::SetUp();
        CreateScene(SceneType::kSphereIbl);
    }
    
};

TEST_F(CameraTest, Camera_FocalLength)
{
    std::vector<float> values = { 0.014f, 0.035f, 0.05f, 0.085f, 0.135f };

    for (auto v : values)
    {
        ASSERT_EQ(rprCameraSetFocalLength(m_camera, v * 1000.0f), RPR_SUCCESS);
        Render();
        SaveAndCompare("%.3f", v);
    }
}

TEST_F(CameraTest, Camera_SensorSize)
{
    std::vector<std::pair<float, float>> values =
    {
        { 0.0167f, 0.0251f }, // APS-C
        { 0.024f,  0.036f  }, // Full frame
        { 0.036f,  0.036f  }  // Mid frame
    };

    for (auto v : values)
    {
        ASSERT_EQ(rprCameraSetSensorSize(m_camera, v.first * 1000.0f, v.second * 1000.0f), RPR_SUCCESS);
        Render();
        SaveAndCompare("%.3f_%.3f", v.first, v.second);
    }
}

TEST_F(CameraTest, Camera_Aperture)
{
    std::vector<float> values = { 0.1f, 0.01f, 0.05f, 0.025f };

    ASSERT_EQ(rprCameraSetFocusDistance(m_camera, 6.0f), RPR_SUCCESS);
    
    for (auto v : values)
    {
        ASSERT_EQ(rprCameraSetFStop(m_camera, v * 1000.0f), RPR_SUCCESS);
        Render();
        SaveAndCompare("%.3f", v);
    }
}

TEST_F(CameraTest, Camera_FocusDistance)
{
    std::vector<float> values = { 1.f, 3.f, 6.f, 9.f };

    ASSERT_EQ(rprCameraSetFStop(m_camera, 0.15f * 1000.0f), RPR_SUCCESS);

    for (auto v : values)
    {
        ASSERT_EQ(rprCameraSetFocusDistance(m_camera, v), RPR_SUCCESS);
        Render();
        SaveAndCompare("%.3f", v);
    }
}

TEST_F(CameraTest, Camera_Ortho)
{
    ASSERT_EQ(rprCameraSetMode(m_camera, RPR_CAMERA_MODE_ORTHOGRAPHIC), RPR_SUCCESS);
    ASSERT_EQ(rprCameraSetOrthoWidth(m_camera, 5.0f), RPR_SUCCESS);
    ASSERT_EQ(rprCameraSetOrthoHeight(m_camera, 5.0f), RPR_SUCCESS);

    Render();
    SaveAndCompare();
}
