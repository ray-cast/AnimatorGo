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

class ArithmeticTest : public BasicTest
{
public:
    virtual void SetUp() override
    {
        BasicTest::SetUp();
        CreateScene(SceneType::kSphereAndPlane);
        AddEnvironmentLight("../Resources/Textures/studio015.hdr");
    }

    void TestBinaryOperationConstantMaps(rpr_material_node_type operation, float3 first_arg, float3 second_arg)
    {
        rpr_material_node arithmetic;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &arithmetic), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputU(arithmetic, "op", operation), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF(arithmetic, "color0", first_arg.x, first_arg.y, first_arg.z, first_arg.w), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputF(arithmetic, "color1", second_arg.x, second_arg.y, second_arg.z, second_arg.w), RPR_SUCCESS);
        AddMaterialNode("arithmetic", arithmetic);

        ASSERT_EQ(rprMaterialNodeSetInputN_ext(GetMaterial("sphere_mtl"), RPR_UBER_MATERIAL_DIFFUSE_COLOR, GetMaterial("arithmetic")), RPR_SUCCESS);

        Render();
        SaveAndCompare();
    }

    void TestUnaryOperationTextureMap(rpr_material_node_type operation)
    {
        rpr_material_node inputTexture;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &inputTexture), RPR_SUCCESS);
        AddMaterialNode("tex", inputTexture);
        ASSERT_EQ(rprMaterialNodeSetInputImageData(inputTexture, "data", FindImage("../Resources/Textures/test_albedo1.jpg")), RPR_SUCCESS);

        rpr_material_node arithmetic;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &arithmetic), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputU(arithmetic, "op", operation), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputN(arithmetic, "color0", inputTexture), RPR_SUCCESS);
        AddMaterialNode("arithmetic", arithmetic);

        ASSERT_EQ(rprMaterialNodeSetInputN_ext(GetMaterial("sphere_mtl"), RPR_UBER_MATERIAL_DIFFUSE_COLOR, GetMaterial("arithmetic")), RPR_SUCCESS);

        Render();
        SaveAndCompare();
    }

    void TestBinaryOperationTextureMaps(rpr_material_node_type operation)
    {
        rpr_material_node texture_arg1;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &texture_arg1), RPR_SUCCESS);
        AddMaterialNode("tex1", texture_arg1);
        ASSERT_EQ(rprMaterialNodeSetInputImageData(texture_arg1, "data", FindImage("../Resources/Textures/test_albedo1.jpg")), RPR_SUCCESS);

        rpr_material_node texture_arg2;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &texture_arg2), RPR_SUCCESS);
        AddMaterialNode("tex2", texture_arg2);
        ASSERT_EQ(rprMaterialNodeSetInputImageData(texture_arg2, "data", FindImage("../Resources/Textures/test_albedo2.jpg")), RPR_SUCCESS);

        rpr_material_node arithmetic;
        ASSERT_EQ(rprMaterialSystemCreateNode(m_matsys, RPR_MATERIAL_NODE_ARITHMETIC, &arithmetic), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputU(arithmetic, "op", operation), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputN(arithmetic, "color0", texture_arg1), RPR_SUCCESS);
        ASSERT_EQ(rprMaterialNodeSetInputN(arithmetic, "color1", texture_arg2), RPR_SUCCESS);
        AddMaterialNode("arithmetic", arithmetic);

        ASSERT_EQ(rprMaterialNodeSetInputN_ext(GetMaterial("sphere_mtl"), RPR_UBER_MATERIAL_DIFFUSE_COLOR, arithmetic), RPR_SUCCESS);

        Render();
        SaveAndCompare();
    }

};

TEST_F(ArithmeticTest, Arithmetic_AddConst)
{
    TestBinaryOperationConstantMaps(RPR_MATERIAL_NODE_OP_ADD, float3(1.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0f));
}

TEST_F(ArithmeticTest, Arithmetic_SubConst)
{
    TestBinaryOperationConstantMaps(RPR_MATERIAL_NODE_OP_SUB, float3(1.0f, 1.0f, 0.0f), float3(0.0f, 1.0f, 0.0f));
}

TEST_F(ArithmeticTest, Arithmetic_MulConst)
{
    TestBinaryOperationConstantMaps(RPR_MATERIAL_NODE_OP_MUL, float3(1.0f, 1.0f, 1.0f), float3(0.0f, 1.0f, 0.0f));
}

TEST_F(ArithmeticTest, Arithmetic_DivConst)
{
    TestBinaryOperationConstantMaps(RPR_MATERIAL_NODE_OP_DIV, float3(1.0f, 1.0f, 1.0f), float3(4.0f, 4.0f, 1.0f));
}

TEST_F(ArithmeticTest, Arithmetic_AddTextures)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_ADD);
}

TEST_F(ArithmeticTest, Arithmetic_SubTextures)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_SUB);
}

TEST_F(ArithmeticTest, Arithmetic_MulTextures)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_MUL);
}

TEST_F(ArithmeticTest, Arithmetic_DivTextures)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_DIV);
}

TEST_F(ArithmeticTest, Arithmetic_Sin)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_SIN);
}

TEST_F(ArithmeticTest, Arithmetic_Cos)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_COS);
}

TEST_F(ArithmeticTest, Arithmetic_Tan)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_TAN);
}

TEST_F(ArithmeticTest, Arithmetic_ASin)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_ASIN);
}

TEST_F(ArithmeticTest, Arithmetic_ACos)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_ACOS);
}

TEST_F(ArithmeticTest, Arithmetic_ATan)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_ATAN);
}

TEST_F(ArithmeticTest, Arithmetic_SelectX)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_SELECT_X);
}

TEST_F(ArithmeticTest, Arithmetic_SelectY)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_SELECT_Y);
}

TEST_F(ArithmeticTest, Arithmetic_SelectZ)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_SELECT_Z);
}

TEST_F(ArithmeticTest, Arithmetic_SelectW)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_SELECT_W);
}

TEST_F(ArithmeticTest, Arithmetic_Combine)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_COMBINE);
}

TEST_F(ArithmeticTest, Arithmetic_Dot3)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_DOT3);
}

TEST_F(ArithmeticTest, Arithmetic_Dot4)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_DOT4);
}

TEST_F(ArithmeticTest, Arithmetic_Cross3)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_CROSS3);
}

TEST_F(ArithmeticTest, Arithmetic_Length3)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_LENGTH3);
}

TEST_F(ArithmeticTest, Arithmetic_Normalize3)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_NORMALIZE3);
}

TEST_F(ArithmeticTest, Arithmetic_Pow)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_POW);
}

TEST_F(ArithmeticTest, Arithmetic_AverageXYZ)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_AVERAGE_XYZ);
}

TEST_F(ArithmeticTest, Arithmetic_Average)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_AVERAGE);
}

TEST_F(ArithmeticTest, Arithmetic_Min)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_MIN);
}

TEST_F(ArithmeticTest, Arithmetic_Max)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_MAX);
}

TEST_F(ArithmeticTest, Arithmetic_Floor)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_FLOOR);
}

TEST_F(ArithmeticTest, Arithmetic_Mod)
{
    TestBinaryOperationTextureMaps(RPR_MATERIAL_NODE_OP_MOD);
}

TEST_F(ArithmeticTest, Arithmetic_Abs)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_ABS);
}

TEST_F(ArithmeticTest, Arithmetic_ShuffleYZWX)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_SHUFFLE_YZWX);
}

TEST_F(ArithmeticTest, Arithmetic_ShuffleZWXY)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_SHUFFLE_ZWXY);
}

TEST_F(ArithmeticTest, Arithmetic_ShuffleWXYZ)
{
    TestUnaryOperationTextureMap(RPR_MATERIAL_NODE_OP_SHUFFLE_WXYZ);
}
