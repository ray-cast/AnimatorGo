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


#include "uber_node.h"

using namespace Baikal;

////////////////////////////////////////
// UberNode_OneArg implementation
////////////////////////////////////////

std::uint32_t UberNode::m_next_id = 0;

#define GET_ONE_ARG(type, sufix)\
    {\
        auto input_map = std::dynamic_pointer_cast<type>(m_input_map);\
        if (!input_map){\
            throw std::runtime_error("Dynamic_cast failure");\
        }\
        return input_map->Get##sufix();\
    }\

#define SET_ONE_ARG(param, type, sufix)\
    {\
        auto input_map_ = std::dynamic_pointer_cast<type>(m_input_map);\
        if (!input_map_){\
            throw std::runtime_error("Dynamic_cast failure");\
        }\
        input_map_->Set##sufix(param);\
        break;\
    }\

#define GET_ARG(X) GET_ONE_ARG(X, Arg)
#define GET_ARG_A(X) GET_ONE_ARG(X, A)
#define GET_ARG_B(X) GET_ONE_ARG(X, B)
#define GET_ARG_C(X) GET_ONE_ARG(X, C)
#define SET_ARG(param, X) SET_ONE_ARG(param, X, Arg)
#define SET_ARG_A(param, X) SET_ONE_ARG(param, X, A)
#define SET_ARG_B(param, X) SET_ONE_ARG(param, X, B)
#define SET_ARG_C(param, X) SET_ONE_ARG(param, X, C)

#define GET_ARG_HANDLER(input_type, sufix)\
        switch (input_type)\
        {\
            case InputMap::InputMapType::kAdd:\
                GET_ARG_##sufix(InputMap_Add);\
            case InputMap::InputMapType::kSub:\
                GET_ARG_##sufix(InputMap_Sub);\
            case InputMap::InputMapType::kMul:\
                GET_ARG_##sufix(InputMap_Mul);\
            case InputMap::InputMapType::kDiv:\
                GET_ARG_##sufix(InputMap_Div);\
            case InputMap::InputMapType::kMin:\
                GET_ARG_##sufix(InputMap_Min);\
            case InputMap::InputMapType::kMax:\
                GET_ARG_##sufix(InputMap_Max);\
            case InputMap::InputMapType::kDot3:\
                GET_ARG_##sufix(InputMap_Dot3);\
            case InputMap::InputMapType::kCross3:\
                GET_ARG_##sufix(InputMap_Cross3);\
            case InputMap::InputMapType::kDot4:\
                GET_ARG_##sufix(InputMap_Dot4);\
            case InputMap::InputMapType::kCross4:\
                GET_ARG_##sufix(InputMap_Cross4);\
            case InputMap::InputMapType::kPow:\
                GET_ARG_##sufix(InputMap_Pow);\
            case InputMap::InputMapType::kMod:\
                GET_ARG_##sufix(InputMap_Mod);\
            case InputMap::InputMapType::kLerp:\
                GET_ARG_##sufix(InputMap_TwoArg<InputMap::InputMapType::kLerp>)\
            case InputMap::InputMapType::kShuffle2:\
                GET_ARG_##sufix(InputMap_TwoArg<InputMap::InputMapType::kShuffle2>);\
            default:\
                return nullptr;\
        }\

#define SET_ARG_HANDLER(param, input_type, sufix)\
        switch (input_type)\
        {\
            case InputMap::InputMapType::kAdd:\
                SET_ARG_##sufix(param, InputMap_Add);\
            case InputMap::InputMapType::kSub:\
                SET_ARG_##sufix(param, InputMap_Sub);\
            case InputMap::InputMapType::kMul:\
                SET_ARG_##sufix(param, InputMap_Mul);\
            case InputMap::InputMapType::kDiv:\
                SET_ARG_##sufix(param, InputMap_Div);\
            case InputMap::InputMapType::kMin:\
                SET_ARG_##sufix(param, InputMap_Min);\
            case InputMap::InputMapType::kMax:\
                SET_ARG_##sufix(param, InputMap_Max);\
            case InputMap::InputMapType::kDot3:\
                SET_ARG_##sufix(param, InputMap_Dot3);\
            case InputMap::InputMapType::kCross3:\
                SET_ARG_##sufix(param,InputMap_Cross3);\
            case InputMap::InputMapType::kDot4:\
                SET_ARG_##sufix(param, InputMap_Dot4);\
            case InputMap::InputMapType::kCross4:\
                SET_ARG_##sufix(param, InputMap_Cross4);\
            case InputMap::InputMapType::kPow:\
                SET_ARG_##sufix(param, InputMap_Pow);\
            case InputMap::InputMapType::kMod:\
                SET_ARG_##sufix(param, InputMap_Mod);\
            case InputMap::InputMapType::kLerp:\
                SET_ARG_##sufix(param, InputMap_TwoArg<InputMap::InputMapType::kLerp>)\
            case InputMap::InputMapType::kShuffle2:\
                SET_ARG_##sufix(param, InputMap_TwoArg<InputMap::InputMapType::kShuffle2>);\
            default:\
                throw std::runtime_error("SET_ARG_HANDLER macros:"\
                                         "unxupported InputMapType for this operation");\
        }\

#define GET_ARG_HANDLER_A(input_type) GET_ARG_HANDLER(input_type, A)
#define GET_ARG_HANDLER_B(input_type) GET_ARG_HANDLER(input_type, B)
#define GET_ARG_HANDLER_C(input_type) GET_ARG_HANDLER(input_type, C)
#define SET_ARG_HANDLER_A(param, input_type) SET_ARG_HANDLER(param, input_type, A)
#define SET_ARG_HANDLER_B(param, input_type) SET_ARG_HANDLER(param, input_type, B)
#define SET_ARG_HANDLER_C(param, input_type) SET_ARG_HANDLER(param, input_type, C)

InputMap::Ptr UberNode_Arg::GetArg(std::uint32_t arg_number)
{
    if (arg_number != 0)
    {
        throw std::logic_error(
            "UberNode_Arg::GetArg(...): attempt to set arg_number distinguish");
    }

    if (m_children[arg_number] < 0)
    {
        return nullptr;
    }

    switch (m_input_map->m_type)
    {
        case InputMap::InputMapType::kSin:
            GET_ARG(InputMap_OneArg<InputMap::InputMapType::kSin>);
        case InputMap::InputMapType::kCos:
            GET_ARG(InputMap_OneArg<InputMap::InputMapType::kCos>);
        case InputMap::InputMapType::kTan:
            GET_ARG(InputMap_OneArg<InputMap::InputMapType::kTan>);
        case InputMap::InputMapType::kAsin:
            GET_ARG(InputMap_OneArg<InputMap::InputMapType::kAsin>);
        case InputMap::InputMapType::kAcos:
            GET_ARG(InputMap_OneArg<InputMap::InputMapType::kAcos>);
        case InputMap::InputMapType::kAtan:
            GET_ARG(InputMap_OneArg<InputMap::InputMapType::kAtan>);
        case InputMap::InputMapType::kLength3:
            GET_ARG(InputMap_OneArg<InputMap::InputMapType::kLength3>);
        case InputMap::InputMapType::kNormalize3:
            GET_ARG(InputMap_OneArg<InputMap::InputMapType::kNormalize3>);
        case InputMap::InputMapType::kFloor:
            GET_ARG(InputMap_OneArg<InputMap::InputMapType::kFloor>);
        case InputMap::InputMapType::kAbs:
            GET_ARG(InputMap_OneArg<InputMap::InputMapType::kAbs>);
        default:
            return nullptr;
    }
}

void UberNode_Arg::SetArg(InputMap::Ptr input_map, std::uint32_t arg_number)
{
    if (arg_number != 0)
    {
        throw std::logic_error("UberNode_Arg::SetArg(...):"
                               "attempt to set arg_number distinguish");
    }

    if (!input_map)
    {
        return;
    }

    switch (input_map->m_type)
    {
        case InputMap::InputMapType::kSin:
            SET_ARG(input_map, InputMap_Sin);
        case InputMap::InputMapType::kCos:
            SET_ARG(input_map, InputMap_Cos);
        case InputMap::InputMapType::kTan:
            SET_ARG(input_map, InputMap_Tan);
        case InputMap::InputMapType::kAsin:
            SET_ARG(input_map, InputMap_Asin);
        case InputMap::InputMapType::kAcos:
            SET_ARG(input_map, InputMap_Acos);
        case InputMap::InputMapType::kAtan:
            SET_ARG(input_map, InputMap_Atan);
        case InputMap::InputMapType::kLength3:
            SET_ARG(input_map, InputMap_Length3);
        case InputMap::InputMapType::kNormalize3:
            SET_ARG(input_map, InputMap_Normalize3);
        case InputMap::InputMapType::kFloor:
            SET_ARG(input_map, InputMap_Floor);
        case InputMap::InputMapType::kAbs:
            SET_ARG(input_map, InputMap_Abs);
        case InputMap::InputMapType::kSelect:
            SET_ARG(input_map, InputMap_OneArg<InputMap::InputMapType::kSelect>);
        case InputMap::InputMapType::kShuffle:
            SET_ARG(input_map, InputMap_OneArg<InputMap::InputMapType::kShuffle>);
        case InputMap::InputMapType::kMatMul:
            SET_ARG(input_map, InputMap_OneArg<InputMap::InputMapType::kMatMul>);
        default:
            throw std::runtime_error("UberNode_Arg::SetArg(...):"
                                     "unxupported InputMapType for this operation");
    }
}

bool UberNode::IsValid() const
{
    size_t arg_number = 0;
    switch (GetType())
    {
        case NodeType::kNoneArgs:
            return true;
        case NodeType::kOneArg:
            arg_number = 1;
            break;
        case NodeType::kTwoArgs:
            arg_number = 2;
            break;
        case NodeType::kThreeArgs:
            arg_number = 3;
            break;
    }

    for (size_t i = 0; i < arg_number; i++)
    {
        if (m_children[i] < 0)
        {
            return false;
        }
    }

    return true;
}

void UberNode::SetChild(std::uint32_t arg_number, Ptr child)
{
    if ((size_t)arg_number < m_children.size())
    {
        m_children[arg_number] = child;
    }
}

////////////////////////////////////////
// UberNode_Select implementation
////////////////////////////////////////

InputMap_Select::Selection UberNode_Select::GetSelection()
{
    auto input_map = std::dynamic_pointer_cast<InputMap_Select>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Select::GetSelection(...):"
                                 "invalid dynamic_cast");
    }

    return input_map->GetSelection();
}

// set selection parametr (Not InputMap)
void UberNode_Select::SetSelection(Baikal::InputMap_Select::Selection selection)
{
    auto input_map = std::dynamic_pointer_cast<InputMap_Select>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Select::SetSelection(...):"
                                 "invalid dynamic_cast");
    }

    input_map->SetSelection(selection);
}

////////////////////////////////////////
// UberNode_Shuffle implementation
////////////////////////////////////////

std::array<uint32_t, 4> UberNode_Shuffle::GetMask() const
{
    auto input_map = std::dynamic_pointer_cast<InputMap_Shuffle>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Shuffle::GetMask(...):"
                                 "invalid dynamic_cast");
    }

    return input_map->GetMask();
}

void UberNode_Shuffle::SetMask(const std::array<uint32_t, 4>& mask)
{
    auto input_map = std::dynamic_pointer_cast<InputMap_Shuffle>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Shuffle::GetMask(...):"
                                 "invalid dynamic_cast");
    }

    input_map->SetMask(mask);
}

////////////////////////////////////////
// UberNode_Matmul implementation
////////////////////////////////////////

RadeonRays::matrix UberNode_Matmul::GetMatrix() const
{
    auto input_map = std::dynamic_pointer_cast<InputMap_MatMul>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Matmul::GetMatrix(...):"
                                 "invalid dynamic_cast");
    }

    return input_map->GetMatrix();
}

void UberNode_Matmul::SetMatrix(const RadeonRays::matrix &mat4)
{
    auto input_map = std::dynamic_pointer_cast<InputMap_MatMul>(m_input_map);
    if (!input_map)
    {
        throw std::runtime_error(
            "UberNode_Matmul::SetMatrix(...): invalid dynamic_cast");
    }
    input_map->SetMatrix(mat4);
}

////////////////////////////////////////
// UberNode_TwoArgs implementation
////////////////////////////////////////

InputMap::Ptr UberNode_TwoArgs::GetArg(std::uint32_t arg_number)
{
    if (arg_number > MAX_ARGS - 1)
    {
        throw std::logic_error("UberNode_TwoArgs::GetArg(...):"
                               "'arg_number' can not be bigger than 1");
    }

    if (arg_number == 0)
    {
        GET_ARG_HANDLER_A(m_input_map->m_type)
    }
    else
    {
        GET_ARG_HANDLER_B(m_input_map->m_type)
    }
}

void UberNode_TwoArgs::SetArg(InputMap::Ptr arg, std::uint32_t arg_number)
{
    if (arg_number > MAX_ARGS - 1)
    {
        throw std::logic_error("UberNode_TwoArgs::SetArg(...):"
                               "'arg_number' can not be bigger than 1");
    }

    if (!arg)
    {
        return;
    }

    if (arg_number == 0)
    {
        SET_ARG_HANDLER_A(arg, m_input_map->m_type)
    }
    else
    {
        SET_ARG_HANDLER_B(arg, m_input_map->m_type)
    }
}

////////////////////////////////////////
// UberNode_Lerp implementation
////////////////////////////////////////

// get control parameter (not child argument)
Baikal::InputMap::Ptr UberNode_Lerp::GetControl()
{
    auto input_map = std::dynamic_pointer_cast<InputMap_Lerp>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Lerp::GetControl(...):"
                                 "invalid dynamic_cast");
    }

    return input_map->GetControl();
}

// set control parameter (not child argument)
void UberNode_Lerp::SetControl(Baikal::InputMap::Ptr control)
{
    auto input_map = std::dynamic_pointer_cast<InputMap_Lerp>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Lerp::GetControl(...):"
                                 "invalid dynamic_cast");
    }

    return input_map->SetControl(control);
}

////////////////////////////////////////
// UberNode_Shuffle2 implementation
////////////////////////////////////////

// get mask parameter (Not InputMap)
std::array<uint32_t, 4> UberNode_Shuffle2::GetMask() const
{
    auto input_map = std::dynamic_pointer_cast<InputMap_Shuffle2>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Shuffle2::GetMask(...):"
                                 "invalid dynamic_cast");
    }

    return input_map->GetMask();
}

// set mask parameter (Not InputMap)
void UberNode_Shuffle2::SetMask(const std::array<uint32_t, 4>& mask)
{
    auto input_map = std::dynamic_pointer_cast<InputMap_Shuffle2>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Shuffle2::GetMask(...):"
                                 "invalid dynamic_cast");
    }

    input_map->SetMask(mask);
}

////////////////////////////////////
// UberNode_Float implementation
////////////////////////////////////

float UberNode_Float::GetValue() const
{
    auto input_map = std::dynamic_pointer_cast<InputMap_ConstantFloat>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Float::GetValue(...):"
                                 "invalid dynamic_cast");
    }

    return input_map->GetValue();
}

void UberNode_Float::SetValue(float value)
{
    auto input_map = std::dynamic_pointer_cast<InputMap_ConstantFloat>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Float::SetValue(...):"
                                 "invalid dynamic_cast");
    }

    input_map->SetValue(value);
}

////////////////////////////////////
// UberNode_Float3 implementation
////////////////////////////////////

RadeonRays::float3 UberNode_Float3::GetValue() const
{
    auto input_map = std::dynamic_pointer_cast<InputMap_ConstantFloat3>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Float3::GetValue(...):"
                                 "invalid dynamic_cast");
    }

    return input_map->GetValue();
}

void UberNode_Float3::SetValue(RadeonRays::float3 value)
{
    auto input_map = std::dynamic_pointer_cast<InputMap_ConstantFloat3>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Float3::SetValue(...):"
                                 "invalid dynamic_cast");
    }

    input_map->SetValue(value);
}

////////////////////////////////////
// UberNode_Sampler implementation
////////////////////////////////////

Texture::Ptr UberNode_Sampler::GetValue() const
{
    auto input_map = std::dynamic_pointer_cast<InputMap_Sampler>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Sampler::GetValue(...):"
                                 "invalid dynamic_cast");
    }

    return input_map->GetTexture();
}

void UberNode_Sampler::SetValue(Texture::Ptr value)
{
    auto input_map = std::dynamic_pointer_cast<InputMap_Sampler>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_Sampler::SetValue(...):"
                                 "invalid dynamic_cast");
    }

    input_map->SetTexture(value);
}

////////////////////////////////////
// UberNode_ThreeArgs
////////////////////////////////////

InputMap::Ptr UberNode_ThreeArgs::GetArg(std::uint32_t arg_number)
{
    if (arg_number > MAX_ARGS - 1)
    {
        throw std::logic_error("UberNode_ThreeArgs::GetArg(...):"
                               "'arg_number can not be bigger than two");
    }

    auto input_map = std::dynamic_pointer_cast<InputMap_Remap>(m_input_map);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_ThreeArgs::GetArg_(...):"
                                 "Dynamic_cast failure");
    }

    if (arg_number == 0)
    {
        return input_map->GetSourceRange();
    }
    else if (arg_number == 1)
    {
        return input_map->GetDestinationRange();
    }
    else
    {
        return input_map->GetData();
    }
}

// Set UberNode_ThreeArgs child
void UberNode_ThreeArgs::SetArg(InputMap::Ptr arg, std::uint32_t arg_number)
{
    if (arg_number > MAX_ARGS - 1)
    {
        throw std::logic_error("UberNode_ThreeArgs::SetArg(...):"
                               "'arg_number can not be bigger than two");
    }

    auto input_map = std::dynamic_pointer_cast<InputMap_Remap>(arg);

    if (!input_map)
    {
        throw std::runtime_error("UberNode_ThreeArgs::SetArg(...):"
                                 "dynamic_cast failure");
    }

    if (arg_number == 0)
    {
        input_map->SetSourceRange(input_map);
    }
    else if (arg_number == 1)
    {
        input_map->SetDestinationRange(input_map);
    }
    else
    {
        input_map->SetData(input_map);
    }
}


namespace {
    struct UberNode_ArgConcrete : public UberNode_Arg
    {
        UberNode_ArgConcrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_Arg(input_map, parent) {}
    };

    struct UberNode_SelectConcrete : public UberNode_Select
    {
        UberNode_SelectConcrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_Select(input_map, parent) {}
    };

    struct UberNode_ShuffleConcrete : public UberNode_Shuffle
    {
        UberNode_ShuffleConcrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_Shuffle(input_map, parent) {}
    };

    struct UberNode_MatmulConcrete : public UberNode_Matmul
    {
        UberNode_MatmulConcrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_Matmul(input_map, parent) {}
    };

    struct UberNode_TwoArgsConcrete : public UberNode_TwoArgs
    {
        UberNode_TwoArgsConcrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_TwoArgs(input_map, parent) {}
    };

    struct UberNode_LerpConcrete : public UberNode_Lerp
    {
        UberNode_LerpConcrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_Lerp(input_map, parent) {}
    };

    struct UberNode_Shuffle2Concrete : public UberNode_Shuffle2
    {
        UberNode_Shuffle2Concrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_Shuffle2(input_map, parent) {}
    };

    struct UberNode_ThreeArgsConcrete : public UberNode_ThreeArgs
    {
        UberNode_ThreeArgsConcrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_ThreeArgs(input_map, parent) {}
    };

    struct UberNode_FloatConcrete : public UberNode_Float
    {
        UberNode_FloatConcrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_Float(input_map, parent) {}
    };

    struct UberNode_Float3Concrete : public UberNode_Float3
    {
        UberNode_Float3Concrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_Float3(input_map, parent) {}
    };

    struct UberNode_SamplerConcrete : public UberNode_Sampler
    {
        UberNode_SamplerConcrete(InputMap::Ptr input_map, Ptr parent)
        : UberNode_Sampler(input_map, parent) {}
    };
}

////////////////////////////////////
// UberNode implementation
////////////////////////////////////

InputMap::Ptr UberNode::GetArg(std::uint32_t arg_number)
{
    return nullptr;
}

void UberNode::SetArg(InputMap::Ptr, std::uint32_t) { }

UberNode::Ptr UberNode::GetChildren(std::uint32_t arg_number) const
{
    if ((size_t)arg_number > m_children.size())
    {
        throw std::logic_error("UberNode::GetChildren(...):"
                                "attempt to get nonexistent 'arg_number'");
    }

    return m_children[arg_number];
}

// input map data type accessor
InputMap::InputMapType UberNode::GetDataType() const
{
    return m_input_map->m_type;
}

std::string UberNode::GetDataTypeText() const
{
    switch (m_input_map->m_type)
    {
    case InputMap::InputMapType::kConstantFloat3:
        return "kConstantFloat3";
    case InputMap::InputMapType::kConstantFloat:
        return "kConstantFloat";
    case InputMap::InputMapType::kSampler:
        return "kSampler";
    case InputMap::InputMapType::kAdd:
        return "kAdd";
    case InputMap::InputMapType::kSub:
        return "kSub";
    case InputMap::InputMapType::kMul:
        return "kMul";
    case InputMap::InputMapType::kDiv:
        return "kDiv";
    case InputMap::InputMapType::kSin:
        return "kSin";
    case InputMap::InputMapType::kCos:
        return "kCos";
    case InputMap::InputMapType::kTan:
        return "kTan";
    case InputMap::InputMapType::kSelect:
        return "kSelect";
    case InputMap::InputMapType::kDot3:
        return "kDot3";
    case InputMap::InputMapType::kCross3:
        return "kCross3";
    case InputMap::InputMapType::kLength3:
        return "kLength3";
    case InputMap::InputMapType::kNormalize3:
        return "kNormalize3";
    case InputMap::InputMapType::kPow:
        return "kPow";
    case InputMap::InputMapType::kAcos:
        return "kAcos";
    case InputMap::InputMapType::kAsin:
        return "kAsin";
    case InputMap::InputMapType::kAtan:
        return "kAtan";
    case InputMap::InputMapType::kLerp:
        return "kLerp";
    case InputMap::InputMapType::kMin:
        return "kMin";
    case InputMap::InputMapType::kMax:
        return "kMax";
    case InputMap::InputMapType::kFloor:
        return "kFloor";
    case InputMap::InputMapType::kMod:
        return "kMod";
    case InputMap::InputMapType::kAbs:
        return "kAbs";
    case InputMap::InputMapType::kShuffle:
        return "kShuffle";
    case InputMap::InputMapType::kShuffle2:
        return "kShuffle2";
    case InputMap::InputMapType::kDot4:
        return "kDot4";
    case InputMap::InputMapType::kCross4:
        return "kCross4";
    case InputMap::InputMapType::kSamplerBumpmap:
        return "kSamplerBumpmap";
    case InputMap::InputMapType::kMatMul:
        return "kMatMul";
    case InputMap::InputMapType::kRemap:
        return "kRemap";
    default:
        return ""; // empty string
    }
}

std::uint32_t UberNode::GetArgNumber()
{
    switch (GetType())
    {
        case NodeType::kNoneArgs:
            return 0;
        case NodeType::kOneArg:
            return 1;
        case NodeType::kTwoArgs:
            return 2;
        case NodeType::kThreeArgs:
            return 3;
        default:
            return 0;
    }
}

int UberNode::GetParentId() const
{
    if (m_parent)
    {
        return m_parent->GetId();
    }

    return -1;
}

UberNode::UberNode(InputMap::Ptr input_map, UberNode::Ptr parent)
    :m_input_map(input_map), m_parent(parent), m_id(m_next_id++)
{  }

UberNode::~UberNode() = default;

UberNode_Arg::UberNode_Arg(InputMap::Ptr input_map, UberNode::Ptr parent)
              : UberNode(input_map, parent)
{
    m_children.push_back(nullptr);
}

UberNode_Select::UberNode_Select(InputMap::Ptr input_map, UberNode::Ptr parent)
                 : UberNode_Arg(input_map, parent) {}

UberNode_Shuffle::UberNode_Shuffle(InputMap::Ptr input_map, UberNode::Ptr parent)
                  : UberNode_Arg(input_map, parent) {}

UberNode_Matmul::UberNode_Matmul(InputMap::Ptr input_map, UberNode::Ptr parent)
                 : UberNode_Arg(input_map, parent) {}

UberNode_TwoArgs::UberNode_TwoArgs(InputMap::Ptr input_map, UberNode::Ptr parent)
                  : UberNode(input_map, parent)
{
    m_children.push_back(nullptr);
    m_children.push_back(nullptr);
}

UberNode_Lerp::UberNode_Lerp(InputMap::Ptr input_map, UberNode::Ptr parent)
    : UberNode_TwoArgs(input_map, parent) {}

UberNode_Shuffle2::UberNode_Shuffle2(InputMap::Ptr input_map, UberNode::Ptr parent)
    : UberNode_TwoArgs(input_map, parent) {}

UberNode_ThreeArgs::UberNode_ThreeArgs(InputMap::Ptr input_map, UberNode::Ptr parent)
    : UberNode(input_map, parent)
{
    m_children.push_back(nullptr);
    m_children.push_back(nullptr);
    m_children.push_back(nullptr);
}

UberNode_Float::UberNode_Float(InputMap::Ptr input_map, UberNode::Ptr parent)
    : UberNode(input_map, parent) {}

UberNode_Float3::UberNode_Float3(InputMap::Ptr input_map, UberNode::Ptr parent)
    : UberNode(input_map, parent) {}

UberNode_Sampler::UberNode_Sampler(InputMap::Ptr input_map, UberNode::Ptr parent)
    : UberNode(input_map, parent) {}

UberNode::Ptr UberNode::Create(InputMap::Ptr input_map, Ptr parent)
{
    switch (input_map->m_type)
    {
        // one arg input maps
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
            return std::make_shared<UberNode_ArgConcrete>(input_map, parent);
        case InputMap::InputMapType::kSelect:
            return std::make_shared<UberNode_SelectConcrete>(input_map, parent);
        case InputMap::InputMapType::kShuffle:
            return std::make_shared<UberNode_ShuffleConcrete>(input_map, parent);
        case InputMap::InputMapType::kMatMul:
            return std::make_shared<UberNode_MatmulConcrete>(input_map, parent);
        // two args input maps
        case InputMap::InputMapType::kAdd:
        case InputMap::InputMapType::kSub:
        case InputMap::InputMapType::kMul:
        case InputMap::InputMapType::kDiv:
        case InputMap::InputMapType::kMin:
        case InputMap::InputMapType::kMax:
        case InputMap::InputMapType::kDot3:
        case InputMap::InputMapType::kCross3:
        case InputMap::InputMapType::kDot4:
        case InputMap::InputMapType::kCross4:
        case InputMap::InputMapType::kPow:
        case InputMap::InputMapType::kMod:
            return std::make_shared<UberNode_TwoArgsConcrete>(input_map, parent);
        case InputMap::InputMapType::kLerp:
            return std::make_shared<UberNode_LerpConcrete>(input_map, parent);
        case InputMap::InputMapType::kShuffle2:
            return std::make_shared<UberNode_Shuffle2Concrete>(input_map, parent);
        default:
        {
            if (std::dynamic_pointer_cast<InputMap_Remap>(input_map) != nullptr)
            {
                return std::make_shared<UberNode_ThreeArgsConcrete>(input_map, parent);
            }

            // leaf input maps
            if (std::dynamic_pointer_cast<InputMap_ConstantFloat>(input_map) != nullptr)
            {
                return std::make_shared<UberNode_FloatConcrete>(input_map, parent);
            }

            if (std::dynamic_pointer_cast<InputMap_ConstantFloat3>(input_map) != nullptr)
            {
                return std::make_shared<UberNode_Float3Concrete>(input_map, parent);
            }

            if (std::dynamic_pointer_cast<InputMap_Sampler>(input_map) != nullptr)
            {
                return std::make_shared<UberNode_SamplerConcrete>(input_map, parent);
            }
        }
    }

    return nullptr;
}