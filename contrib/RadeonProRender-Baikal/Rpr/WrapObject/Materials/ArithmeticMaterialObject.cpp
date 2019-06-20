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
#include "ArithmeticMaterialObject.h"
#include "TextureMaterialObject.h"
#include "WrapObject/Exception.h"
#include "SceneGraph/material.h"
#include "SceneGraph/inputmaps.h"

using namespace Baikal;
using namespace RadeonRays;

ArithmeticMaterialObject::ArithmeticMaterialObject(MaterialObject::Type mat_type)
    : MaterialObject(mat_type)
{
}

Baikal::InputMap::Ptr ArithmeticMaterialObject::GetInputMap()
{
    if (!m_input_map)
    {
        throw Exception(RPR_ERROR_INTERNAL_ERROR, "InputMap from empty arithmetic node was requested. Set operation first.");
    }
    return m_input_map;
}

void ArithmeticMaterialObject::SetInputMaterial(const std::string& input_name, MaterialObject* input)
{
    if (!input->IsArithmetic())
    {
        throw Exception(RPR_ERROR_INTERNAL_ERROR, "Only Arithmetic node can be used as input");
    }
    if (!m_input_map)
    {
        throw Exception(RPR_ERROR_INTERNAL_ERROR, "\"op\" input must be set first");
    } 
    ArithmeticMaterialObject *arithmetic = static_cast<ArithmeticMaterialObject*>(input);
    SetInputMap(input_name, arithmetic->GetInputMap());
}

void ArithmeticMaterialObject::SetInputTexture(const std::string& input_name, TextureMaterialObject* input)
{
    auto input_map = Baikal::InputMap_Sampler::Create(input->GetTexture());
    SetInputMap(input_name, input_map);
}


void ArithmeticMaterialObject::SetInputF(const std::string& input_name, const RadeonRays::float4& val)
{
    auto input_map = Baikal::InputMap_ConstantFloat3::Create(val);
    SetInputMap(input_name, input_map);
}

void ArithmeticMaterialObject::Update(MaterialObject* mat)
{

}

Baikal::Material::Ptr ArithmeticMaterialObject::GetMaterial()
{
    return nullptr;
}

void ArithmeticMaterialObject::SetInputU(const std::string& input_name, rpr_uint val)
{
    if (input_name == "op")
    {
        if (m_input_map)
        {
            throw Exception(RPR_ERROR_INTERNAL_ERROR, "Arithmetic field \"op\" can not be set twice");
        }

        switch (val)
        {
        case RPR_MATERIAL_NODE_OP_ADD:
            m_input_map = Baikal::InputMap_Add::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f),
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_SUB:
            m_input_map = Baikal::InputMap_Sub::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f),
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_MUL:
            m_input_map = Baikal::InputMap_Mul::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f),
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_DIV:
            m_input_map = Baikal::InputMap_Div::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f),
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_SIN:
            m_input_map = Baikal::InputMap_Sin::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_COS:
            m_input_map = Baikal::InputMap_Cos::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_TAN:
            m_input_map = Baikal::InputMap_Tan::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_SELECT_X:
            m_input_map = Baikal::InputMap_Select::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                Baikal::InputMap_Select::Selection::kX);
            break;
        case RPR_MATERIAL_NODE_OP_SELECT_Y:
            m_input_map = Baikal::InputMap_Select::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                Baikal::InputMap_Select::Selection::kY);
            break;
        case RPR_MATERIAL_NODE_OP_SELECT_Z:
            m_input_map = Baikal::InputMap_Select::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                Baikal::InputMap_Select::Selection::kZ);
            break;
        case RPR_MATERIAL_NODE_OP_SELECT_W:
            m_input_map = Baikal::InputMap_Select::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                Baikal::InputMap_Select::Selection::kW);
            break;
        case RPR_MATERIAL_NODE_OP_COMBINE:
            m_input_map = Baikal::InputMap_Shuffle2::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                {{ 0, 4, 1, 5 }}); // {c0.x, c1.x, c0.y, c1.y}
            break;
        case RPR_MATERIAL_NODE_OP_DOT3:
            m_input_map = Baikal::InputMap_Dot3::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3())
                );
            break;
        case RPR_MATERIAL_NODE_OP_CROSS3:
            m_input_map = Baikal::InputMap_Cross3::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3())
            );
            break;
        case RPR_MATERIAL_NODE_OP_LENGTH3:
            m_input_map = Baikal::InputMap_Length3::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()));
            break;
        case RPR_MATERIAL_NODE_OP_NORMALIZE3:
            m_input_map = Baikal::InputMap_Normalize3::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()));
            break;
        case RPR_MATERIAL_NODE_OP_POW:
            m_input_map = Baikal::InputMap_Pow::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f),
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_ACOS:
            m_input_map = Baikal::InputMap_Acos::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_ASIN:
            m_input_map = Baikal::InputMap_Asin::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_ATAN:
            m_input_map = Baikal::InputMap_Atan::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_AVERAGE_XYZ:
        case RPR_MATERIAL_NODE_OP_AVERAGE:
            m_input_map = Baikal::InputMap_Lerp::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                Baikal::InputMap_ConstantFloat::Create(0.5f));
        case RPR_MATERIAL_NODE_OP_MIN:
            m_input_map = Baikal::InputMap_Min::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f),
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_MAX:
            m_input_map = Baikal::InputMap_Max::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f),
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_FLOOR:
            m_input_map = Baikal::InputMap_Floor::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_MOD:
            m_input_map = Baikal::InputMap_Mod::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f),
                Baikal::InputMap_ConstantFloat::Create(1.f)
                );
            break;
        case RPR_MATERIAL_NODE_OP_ABS:
            m_input_map = Baikal::InputMap_Abs::Create(
                Baikal::InputMap_ConstantFloat::Create(0.f));
            break;
        case RPR_MATERIAL_NODE_OP_SHUFFLE_YZWX:
            m_input_map = Baikal::InputMap_Shuffle::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                {{1, 2, 3, 0}});
            break;
        case RPR_MATERIAL_NODE_OP_SHUFFLE_ZWXY:
            m_input_map = Baikal::InputMap_Shuffle::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                {{2, 3, 0, 1}});
            break;
        case RPR_MATERIAL_NODE_OP_SHUFFLE_WXYZ:
            m_input_map = Baikal::InputMap_Shuffle::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                {{3, 0, 1, 2}});
            break;
        case RPR_MATERIAL_NODE_OP_MAT_MUL:
            m_input_map = Baikal::InputMap_MatMul::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                RadeonRays::matrix());
            break;
        case RPR_MATERIAL_NODE_OP_DOT4:
            m_input_map = Baikal::InputMap_Dot4::Create(
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3()),
                Baikal::InputMap_ConstantFloat3::Create(RadeonRays::float3())
            );
            break;
        default:
            throw Exception(RPR_ERROR_INTERNAL_ERROR, "Unsupported operation");
        }
    }
}

void ArithmeticMaterialObject::SetInputMap(const std::string& input_name, Baikal::InputMap::Ptr input_map)
{
    // get number from string "colorN"
    int position = input_name[5] - '0';
    if (position < 0 || position > 3)
    {
        throw Exception(RPR_ERROR_INTERNAL_ERROR, "Invalid input");
    }

    switch (m_input_map->m_type)
    {
        // Two inputs
        case InputMap::InputMapType::kAdd:
        case InputMap::InputMapType::kSub:
        case InputMap::InputMapType::kMul:
        case InputMap::InputMapType::kDiv:
        case InputMap::InputMapType::kMin:
        case InputMap::InputMapType::kMax:
        case InputMap::InputMapType::kDot3:
        case InputMap::InputMapType::kDot4:
        case InputMap::InputMapType::kCross3:
        case InputMap::InputMapType::kCross4:
        case InputMap::InputMapType::kPow:
        case InputMap::InputMapType::kMod:
        {
            //It's safe since all this types differs only in id value
            InputMap_Add *i = static_cast<InputMap_Add*>(m_input_map.get());
            if (position == 0)
            {
                i->SetA(input_map);
            }
            else if (position == 1)
            {
                i->SetB(input_map);
            }
            else
            {
                throw Exception(RPR_ERROR_INTERNAL_ERROR, "Invalid input");
            }
            break;
        }
        //Single input
        case InputMap::InputMapType::kSin:
        case InputMap::InputMapType::kCos:
        case InputMap::InputMapType::kTan:
        case InputMap::InputMapType::kAsin:
        case InputMap::InputMapType::kAcos:
        case InputMap::InputMapType::kAtan:
        case InputMap::InputMapType::kLength3:
        case InputMap::InputMapType::kNormalize3:
        case InputMap::InputMapType::kFloor:
        case InputMap::InputMapType::kAbs:
        {
            //It's safe since all this types differs only in id value
            InputMap_Sin *i = static_cast<InputMap_Sin*>(m_input_map.get());
            if (position == 0)
            {
                i->SetArg(input_map);
            }
            else
            {
                throw Exception(RPR_ERROR_INTERNAL_ERROR, "Invalid input");
            }
            break;
        }

        // Specials
        case InputMap::InputMapType::kLerp:
        {
            InputMap_Lerp *i = static_cast<InputMap_Lerp*>(m_input_map.get());
            if (position == 0)
            {
                i->SetA(input_map);
            }
            else if (position == 1)
            {
                i->SetB(input_map);
            }
            else
            {
                throw Exception(RPR_ERROR_INTERNAL_ERROR, "Invalid input");
            }

            break;
        }
        case InputMap::InputMapType::kSelect:
        {
            InputMap_Select *i = static_cast<InputMap_Select*>(m_input_map.get());
            if (position == 0)
            {
                i->SetArg(input_map);
            }
            else
            {
                throw Exception(RPR_ERROR_INTERNAL_ERROR, "Invalid input");
            }

            break;
        }
        case InputMap::InputMapType::kShuffle:
        {
            InputMap_Shuffle *i = static_cast<InputMap_Shuffle*>(m_input_map.get());
            if (position == 0)
            {
                i->SetArg(input_map);
            }
            else
            {
                throw Exception(RPR_ERROR_INTERNAL_ERROR, "Invalid input");
            }

            break;
        }
        case InputMap::InputMapType::kShuffle2:
        {
            InputMap_Shuffle2 *i = static_cast<InputMap_Shuffle2*>(m_input_map.get());

            if (position == 0)
            {
                i->SetA(input_map);
            }
            else if (position == 1)
            {
                i->SetB(input_map);
            }
            else
            {
                throw Exception(RPR_ERROR_INTERNAL_ERROR, "Invalid input");
            }

            break;
        }
        case InputMap::InputMapType::kMatMul:
        {
            InputMap_MatMul *i = static_cast<InputMap_MatMul*>(m_input_map.get());

            if (position == 3) // value is color3
            {
                i->SetArg(input_map);
            }

            break;
        }
        default:
            throw Exception(RPR_ERROR_INTERNAL_ERROR, "Unsupported input map type");
    }
}
