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

#include "inputmap.h"

#include <assert.h>
#include <array>

#include "math/float3.h"
#include "math/matrix.h"


namespace Baikal
{
    class InputMap_ConstantFloat3 : public InputMap
    {
    public:
        using Ptr = std::shared_ptr<InputMap_ConstantFloat3>;
        static Ptr Create(RadeonRays::float3 value)
        {
            return Ptr(new InputMap_ConstantFloat3(value));
        }

        void CollectTextures(std::set<Texture::Ptr> &textures) override
        {
        }

        void SetValue(RadeonRays::float3 value)
        {
            m_value = value;
            SetDirty(true);
        }

        RadeonRays::float3 GetValue() const
        {
            return m_value;
        }

        bool IsLeaf() const override
        {
            return true;
        }

    private:
        RadeonRays::float3 m_value;

        explicit InputMap_ConstantFloat3(RadeonRays::float3 v) :
        InputMap(InputMapType::kConstantFloat3),
        m_value(v)
        {
            SetDirty(true);
        }
    };

    class InputMap_ConstantFloat : public InputMap
    {
    public:
        using Ptr = std::shared_ptr<InputMap_ConstantFloat>;
        static Ptr Create(float value)
        {
            return Ptr(new InputMap_ConstantFloat(value));
        }

        void CollectTextures(std::set<Texture::Ptr> &textures) override
        {
        }

        void SetValue(float value)
        {
            m_value = value;
            SetDirty(true);
        }

        float GetValue() const
        {
            return m_value;
        }

        bool IsLeaf() const override
        {
            return true;
        }

    private:
        float m_value;

        explicit InputMap_ConstantFloat(float v) :
        InputMap(InputMapType::kConstantFloat),
        m_value(v)
        {
            SetDirty(true);
        }
    };

    class InputMap_Sampler : public InputMap
    {
    public:
        using Ptr = std::shared_ptr<InputMap_Sampler>;
        static Ptr Create(Texture::Ptr texture)
        {
            return Ptr(new InputMap_Sampler(texture));
        }

        void CollectTextures(std::set<Texture::Ptr> &textures) override
        {
            textures.insert(m_texture);
        }

        virtual void SetTexture(Texture::Ptr texture)
        {
            m_texture = texture;
            assert(m_texture);
            SetDirty(true);
        }

        virtual Texture::Ptr GetTexture() const
        {
            return m_texture;
        }

        bool IsDirty() const override
        {
            return InputMap::IsDirty() || m_texture->IsDirty();
        }

        bool IsLeaf() const override
        {
            return true;
        }

    protected:
        Texture::Ptr m_texture;

        explicit InputMap_Sampler(Texture::Ptr texture) :
        InputMap(InputMapType::kSampler),
        m_texture(texture)
        {
            SetDirty(true);
        }
    };

    class InputMap_SamplerBumpMap : public InputMap_Sampler
    {
    public:
        using Ptr = std::shared_ptr<InputMap_SamplerBumpMap>;
        static Ptr Create(Texture::Ptr texture)
        {
            return Ptr(new InputMap_SamplerBumpMap(texture));
        }

    protected:
        explicit InputMap_SamplerBumpMap(Texture::Ptr texture) :
            InputMap_Sampler(texture)
        {
            SetDirty(true);
            m_type = InputMapType::kSamplerBumpmap;
        }

    };
    
    template<InputMap::InputMapType type>
    class InputMap_TwoArg : public InputMap
    {
    public:
        using Ptr = std::shared_ptr<InputMap_TwoArg<type>>;
        static Ptr Create(InputMap::Ptr a, InputMap::Ptr b)
        {
            return Ptr(new InputMap_TwoArg(a, b));
        }

        void CollectTextures(std::set<Texture::Ptr> &textures) override
        {
            m_a->CollectTextures(textures);
            m_b->CollectTextures(textures);
        }

        void SetA(InputMap::Ptr a)
        {
            m_a = a;
            assert(m_a);
            SetDirty(true);
        }

        void SetB(InputMap::Ptr b)
        {
            m_b = b;
            assert(m_b);
            SetDirty(true);
        }

        InputMap::Ptr GetA() const
        {
            return m_a;
        }

        InputMap::Ptr GetB() const
        {
            return m_b;
        }

        bool IsDirty() const override
        {
            return InputMap::IsDirty() || m_a->IsDirty() || m_b->IsDirty();
        }

        void SetDirty(bool dirty) const override
        {
            SceneObject::SetDirty(dirty);
            m_a->SetDirty(dirty);
            m_b->SetDirty(dirty);
        }

        void GetLeafs(std::set<InputMap::Ptr> & leafs) override
        {
            if (m_a->IsLeaf()) leafs.insert(m_a);
            else m_a->GetLeafs(leafs);

            if (m_b->IsLeaf()) leafs.insert(m_b);
            else m_b->GetLeafs(leafs);
        }


    protected:
        InputMap::Ptr m_a;
        InputMap::Ptr m_b;

        InputMap_TwoArg(InputMap::Ptr a, InputMap::Ptr b) :
            InputMap(type),
            m_a(a),
            m_b(b)
        {
            assert(m_a && m_b);
            SetDirty(true);
        }
    };

    typedef InputMap_TwoArg<InputMap::InputMapType::kAdd> InputMap_Add;
    typedef InputMap_TwoArg<InputMap::InputMapType::kSub> InputMap_Sub;
    typedef InputMap_TwoArg<InputMap::InputMapType::kMul> InputMap_Mul;
    typedef InputMap_TwoArg<InputMap::InputMapType::kDiv> InputMap_Div;
    typedef InputMap_TwoArg<InputMap::InputMapType::kMin> InputMap_Min;
    typedef InputMap_TwoArg<InputMap::InputMapType::kMax> InputMap_Max;
    typedef InputMap_TwoArg<InputMap::InputMapType::kDot3> InputMap_Dot3;
    typedef InputMap_TwoArg<InputMap::InputMapType::kCross3> InputMap_Cross3;
    typedef InputMap_TwoArg<InputMap::InputMapType::kDot4> InputMap_Dot4;
    typedef InputMap_TwoArg<InputMap::InputMapType::kCross4> InputMap_Cross4;
    typedef InputMap_TwoArg<InputMap::InputMapType::kPow> InputMap_Pow;
    typedef InputMap_TwoArg<InputMap::InputMapType::kMod> InputMap_Mod;

    template<InputMap::InputMapType type>
    class InputMap_OneArg : public InputMap
    {
    public:
        using Ptr = std::shared_ptr<InputMap_OneArg<type>>;
        static Ptr Create(InputMap::Ptr arg)
        {
            return Ptr(new InputMap_OneArg(arg));
        }

        void CollectTextures(std::set<Texture::Ptr> &textures) override
        {
            m_arg->CollectTextures(textures);
        }

        void SetArg(InputMap::Ptr arg)
        {
            m_arg = arg;
            assert(m_arg);
            SetDirty(true);
        }

        InputMap::Ptr GetArg() const
        {
            return m_arg;
        }

        bool IsDirty() const override
        {
            return InputMap::IsDirty() || m_arg->IsDirty();
        }

        void GetLeafs(std::set<InputMap::Ptr> & leafs) override
        {
            if (m_arg->IsLeaf()) leafs.insert(m_arg);
            else m_arg->GetLeafs(leafs);
        }

        void SetDirty(bool dirty) const override
        {
            SceneObject::SetDirty(dirty);
            m_arg->SetDirty(dirty);
        }

    protected:
        InputMap::Ptr m_arg;

        InputMap_OneArg(InputMap::Ptr arg) :
        InputMap(type),
        m_arg(arg)
        {
            assert(m_arg);
            SetDirty(true);
        }
    };

    typedef InputMap_OneArg<InputMap::InputMapType::kSin> InputMap_Sin;
    typedef InputMap_OneArg<InputMap::InputMapType::kCos> InputMap_Cos;
    typedef InputMap_OneArg<InputMap::InputMapType::kTan> InputMap_Tan;
    typedef InputMap_OneArg<InputMap::InputMapType::kAsin> InputMap_Asin;
    typedef InputMap_OneArg<InputMap::InputMapType::kAcos> InputMap_Acos;
    typedef InputMap_OneArg<InputMap::InputMapType::kAtan> InputMap_Atan;
    typedef InputMap_OneArg<InputMap::InputMapType::kLength3> InputMap_Length3;
    typedef InputMap_OneArg<InputMap::InputMapType::kNormalize3> InputMap_Normalize3;
    typedef InputMap_OneArg<InputMap::InputMapType::kFloor> InputMap_Floor;
    typedef InputMap_OneArg<InputMap::InputMapType::kAbs> InputMap_Abs;

    class InputMap_Lerp : public InputMap_TwoArg<InputMap::InputMapType::kLerp>
    {
    public:
        using Ptr = std::shared_ptr<InputMap_Lerp>;
        static Ptr Create(InputMap::Ptr a, InputMap::Ptr b, InputMap::Ptr control)
        {
            return Ptr(new InputMap_Lerp(a, b, control));
        }

        void SetControl(InputMap::Ptr control)
        {
            m_control = control;
            assert(m_control);
            SetDirty(true);
        }

        InputMap::Ptr GetControl() const
        {
            return m_control;
        }

        void CollectTextures(std::set<Texture::Ptr> &textures) override
        {
            InputMap_TwoArg::CollectTextures(textures);
            m_control->CollectTextures(textures);
        }

        bool IsDirty() const override
        {
            return InputMap_TwoArg::IsDirty() || m_control->IsDirty();
        }

        void GetLeafs(std::set<InputMap::Ptr> & leafs) override
        {
            InputMap_TwoArg::GetLeafs(leafs);
            if (m_control->IsLeaf()) leafs.insert(m_control);
            else m_control->GetLeafs(leafs);
        }

        void SetDirty(bool dirty) const override
        {
            InputMap_TwoArg::SetDirty(dirty);
            m_control->SetDirty(dirty);
        }


    private:
        InputMap::Ptr m_control;

        InputMap_Lerp(InputMap::Ptr a, InputMap::Ptr b, InputMap::Ptr control) :
        InputMap_TwoArg(a, b),
        m_control(control)
        {
            assert(m_control);
            SetDirty(true);
        }
    };

    class InputMap_Select : public InputMap_OneArg<InputMap::InputMapType::kSelect>
    {
    public:
        using Ptr = std::shared_ptr<InputMap_Select>;
        enum class Selection
        {
            kX = 0,
            kY = 1,
            kZ = 2,
            kW = 3
        };
        static Ptr Create(InputMap::Ptr arg, Selection selection)
        {
            return Ptr(new InputMap_Select(arg, selection));
        }

        void SetSelection(Selection selection)
        {
            m_selection = selection;
            SetDirty(true);
        }

        Selection GetSelection() const
        {
            return m_selection;
        }

    private:
        Selection m_selection;

        InputMap_Select(InputMap::Ptr arg, Selection selection) :
        InputMap_OneArg(arg),
        m_selection(selection)
        {
            SetDirty(true);
        }
    };

    class InputMap_Shuffle : public InputMap_OneArg<InputMap::InputMapType::kShuffle>
    {
    public:
        using Ptr = std::shared_ptr<InputMap_Shuffle>;
        static Ptr Create(InputMap::Ptr arg, const std::array<uint32_t, 4> &mask)
        {
            return Ptr(new InputMap_Shuffle(arg, mask));
        }

        void SetMask(const std::array<uint32_t, 4> &mask)
        {
            m_mask = mask;
            SetDirty(true);
        }

        std::array<uint32_t, 4> GetMask() const
        {
            return m_mask;
        }


    private:
        std::array<uint32_t, 4> m_mask;

        InputMap_Shuffle(InputMap::Ptr arg, const std::array<uint32_t, 4> &mask) :
        InputMap_OneArg(arg),
        m_mask(mask)
        {
            SetDirty(true);
        }
    };

    class InputMap_Shuffle2 : public InputMap_TwoArg<InputMap::InputMapType::kShuffle2>
    {
    public:
        using Ptr = std::shared_ptr<InputMap_Shuffle2>;
        static Ptr Create(InputMap::Ptr a, InputMap::Ptr b, const std::array<uint32_t, 4> &mask)
        {
            return Ptr(new InputMap_Shuffle2(a, b, mask));
        }

        void SetMask(const std::array<uint32_t, 4> &mask)
        {
            m_mask = mask;
            SetDirty(true);
        }

        std::array<uint32_t, 4> GetMask() const
        {
            return m_mask;
        }

    private:
        std::array<uint32_t, 4> m_mask;

        InputMap_Shuffle2(InputMap::Ptr a, InputMap::Ptr b, const std::array<uint32_t, 4> &mask) :
        InputMap_TwoArg(a, b),
        m_mask(mask)
        {
            SetDirty(true);
        }
    };

    class InputMap_MatMul : public InputMap_OneArg<InputMap::InputMapType::kMatMul>
    {
    public:
        using Ptr = std::shared_ptr<InputMap_MatMul>;
        static Ptr Create(InputMap::Ptr arg, const RadeonRays::matrix &mat4)
        {
            return Ptr(new InputMap_MatMul(arg, mat4));
        }

        void SetMatrix(const RadeonRays::matrix &mat4)
        {
            m_mat4 = mat4;
            SetDirty(true);
        }

        RadeonRays::matrix GetMatrix() const
        {
            return m_mat4;
        }

    private:
        RadeonRays::matrix m_mat4;

        InputMap_MatMul(InputMap::Ptr arg, const RadeonRays::matrix &mat4) :
        InputMap_OneArg(arg),
        m_mat4(mat4)
        {
            SetDirty(true);
        }
    };

    class InputMap_Remap : public InputMap
    {
    public:
        using Ptr = std::shared_ptr<InputMap_Remap>;
        static Ptr Create(InputMap::Ptr source_range, InputMap::Ptr destination_range, 
            InputMap::Ptr data)
        {
            return Ptr(new InputMap_Remap(source_range, destination_range, data));
        }

        void CollectTextures(std::set<Texture::Ptr> &textures) override
        {
            m_source_range->CollectTextures(textures);
            m_destination_range->CollectTextures(textures);
            m_data->CollectTextures(textures);
        }

        void SetSourceRange(InputMap::Ptr source_range)
        {
            m_source_range = source_range;
            assert(m_source_range);
            SetDirty(true);
        }

        void SetDestinationRange(InputMap::Ptr destination_range)
        {
            m_destination_range = destination_range;
            assert(m_destination_range);
            SetDirty(true);
        }

        InputMap::Ptr GetSourceRange() const
        {
            return m_source_range;
        }

        InputMap::Ptr GetDestinationRange() const
        {
            return m_destination_range;
        }

        void SetData(InputMap::Ptr data)
        {
            m_data = data;
            assert(m_data);
            SetDirty(true);
        }

        InputMap::Ptr GetData() const
        {
            return m_data;
        }

        bool IsDirty() const override
        {
            return InputMap::IsDirty() || m_source_range->IsDirty() || 
                m_destination_range->IsDirty() || m_data->IsDirty();
        }

        void SetDirty(bool dirty) const override
        {
            SceneObject::SetDirty(dirty);
            m_source_range->SetDirty(dirty);
            m_destination_range->SetDirty(dirty);
            m_data->SetDirty(dirty);
        }

        void GetLeafs(std::set<InputMap::Ptr> & leafs) override
        {
            if (m_source_range->IsLeaf()) leafs.insert(m_source_range);
            else m_source_range->GetLeafs(leafs);

            if (m_destination_range->IsLeaf()) leafs.insert(m_destination_range);
            else m_destination_range->GetLeafs(leafs);

            if (m_data->IsLeaf()) leafs.insert(m_data);
            else m_data->GetLeafs(leafs);
        }


    private:
        InputMap::Ptr m_source_range;
        InputMap::Ptr m_destination_range;
        InputMap::Ptr m_data;

        InputMap_Remap(InputMap::Ptr source_range, InputMap::Ptr destination_range,
            InputMap::Ptr data) :
            InputMap(InputMapType::kRemap),
            m_source_range(source_range),
            m_destination_range(destination_range),
            m_data(data)
        {
            SetDirty(true);
            assert(m_source_range && m_destination_range && m_data);
        }
    };

}
