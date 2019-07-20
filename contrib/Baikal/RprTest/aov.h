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

class AovTest : public BasicTest
{
public:
    void CreateFramebuffer() override
    {
        // Create Framebuffer
        rpr_framebuffer_desc desc = { kOutputWidth, kOutputHeight };
        rpr_framebuffer_format fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };
        ASSERT_EQ(rprContextCreateFrameBuffer(m_context, fmt, &desc, &m_framebuffer), RPR_SUCCESS);
    }

    void TestAovImplemented(rpr_aov aov)
    {
        CreateScene(SceneType::kSphereAndPlane);
        AddEnvironmentLight("../Resources/Textures/studio015.hdr");

        ASSERT_EQ(rprContextSetAOV(m_context, aov, m_framebuffer), RPR_SUCCESS);
        Render();
        SaveAndCompare();
    }

    void TestAovNotImplemented(rpr_aov aov) const
    {
        ASSERT_EQ(rprContextSetAOV(m_context, aov, m_framebuffer), RPR_ERROR_UNIMPLEMENTED);
    }
};

// Test aovs that implemented
TEST_F(AovTest, Aov_Color)
{
    TestAovImplemented(RPR_AOV_COLOR);
}

TEST_F(AovTest, Aov_WorldCoordinate)
{
    TestAovImplemented(RPR_AOV_WORLD_COORDINATE);
}

TEST_F(AovTest, Aov_Uv)
{
    TestAovImplemented(RPR_AOV_UV);
}

TEST_F(AovTest, Aov_GeometricNormal)
{
    TestAovImplemented(RPR_AOV_GEOMETRIC_NORMAL);
}

TEST_F(AovTest, Aov_ShadingNormal)
{
    TestAovImplemented(RPR_AOV_SHADING_NORMAL);
}

TEST_F(AovTest, Aov_Opacity)
{
    // Create dummy framebuffer
    rpr_framebuffer_desc desc = { kOutputWidth, kOutputHeight };
    rpr_framebuffer_format fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };
    rpr_framebuffer dummy_fb = nullptr;
    ASSERT_EQ(rprContextCreateFrameBuffer(m_context, fmt, &desc, &dummy_fb), RPR_SUCCESS);
    ASSERT_EQ(rprContextSetAOV(m_context, RPR_AOV_COLOR, dummy_fb), RPR_SUCCESS);

    CreateScene(SceneType::kOpacityPlanes);
    AddEnvironmentLight("../Resources/Textures/studio015.hdr");
    
    ASSERT_EQ(rprContextSetAOV(m_context, RPR_AOV_OPACITY, m_framebuffer), RPR_SUCCESS);
    Render();
    SaveAndCompare();
}

TEST_F(AovTest, Aov_ObjectID)
{
    TestAovImplemented(RPR_AOV_OBJECT_ID);
}

TEST_F(AovTest, Aov_ObjectGroupID)
{
    CreateScene(SceneType::kSphereAndPlane);
    AddEnvironmentLight("../Resources/Textures/studio015.hdr");

    ASSERT_EQ(rprContextSetAOV(m_context, RPR_AOV_OBJECT_GROUP_ID, m_framebuffer), RPR_SUCCESS);
    const rpr_shape sphere = GetShape("sphere");
    ASSERT_EQ(rprShapeSetObjectGroupID(sphere, 0), RPR_SUCCESS);
    const rpr_shape plane = GetShape("plane");
    ASSERT_EQ(rprShapeSetObjectGroupID(plane, 1), RPR_SUCCESS);

    Render();
    SaveAndCompare("1");

    // Move sphere to group 1
    ASSERT_EQ(rprShapeSetObjectGroupID(sphere, 1), RPR_SUCCESS);
    
    Render();
    SaveAndCompare("2");

}

TEST_F(AovTest, Aov_Background)
{
    TestAovImplemented(RPR_AOV_BACKGROUND);
}

// Make sure that aovs below are not implemented
TEST_F(AovTest, Aov_MaterialIndex)
{
    TestAovNotImplemented(RPR_AOV_MATERIAL_IDX);
}

TEST_F(AovTest, Aov_Depth)
{
    TestAovNotImplemented(RPR_AOV_DEPTH);
}

TEST_F(AovTest, Aov_ShadowCatcher)
{
    TestAovNotImplemented(RPR_AOV_SHADOW_CATCHER);
}

TEST_F(AovTest, Aov_Emission)
{
    TestAovNotImplemented(RPR_AOV_EMISSION);
}

TEST_F(AovTest, Aov_Velocity)
{
    TestAovNotImplemented(RPR_AOV_VELOCITY);
}

TEST_F(AovTest, Aov_DirectIllumination)
{
    TestAovNotImplemented(RPR_AOV_DIRECT_ILLUMINATION);
}

TEST_F(AovTest, Aov_IndirectIllumination)
{
    TestAovNotImplemented(RPR_AOV_INDIRECT_ILLUMINATION);
}
