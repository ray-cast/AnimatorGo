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

#include "WrapObject/WrapObject.h"
#include "RadeonProRender.h"
#include "math/float3.h"

#include <string>
#include <map>
#include <set>

#include "SceneGraph/texture.h"
#include "SceneGraph/material.h"

class TextureMaterialObject;
class ImageMaterialObject;

//represent rpr_material_node
class RPR_API_ENTRY MaterialObject
    : public WrapObject
{
public:
    enum Type
    {
        //represent rpr_image:
        kImage, 
        //textures:
        kNormalMap = RPR_MATERIAL_NODE_NORMAL_MAP,
        kImageTexture = RPR_MATERIAL_NODE_IMAGE_TEXTURE,
        kNoise2dTexture = RPR_MATERIAL_NODE_NOISE2D_TEXTURE,
        kDotTexture = RPR_MATERIAL_NODE_DOT_TEXTURE,
        kGradientTexture = RPR_MATERIAL_NODE_GRADIENT_TEXTURE,
        kCheckerTexture = RPR_MATERIAL_NODE_CHECKER_TEXTURE,
        kConstantTexture = RPR_MATERIAL_NODE_CONSTANT_TEXTURE,
        kBumpMap = RPR_MATERIAL_NODE_BUMP_MAP,
        //materials:
        kDiffuse = RPR_MATERIAL_NODE_DIFFUSE,
        kDiffuseRefraction = RPR_MATERIAL_NODE_DIFFUSE_REFRACTION,
        kMicrofacet = RPR_MATERIAL_NODE_MICROFACET,
        kReflection = RPR_MATERIAL_NODE_REFLECTION,
        kRefraction = RPR_MATERIAL_NODE_REFRACTION,
        kMicrofacetRefraction = RPR_MATERIAL_NODE_MICROFACET_REFRACTION,
        kTransparent = RPR_MATERIAL_NODE_TRANSPARENT,
        kEmissive = RPR_MATERIAL_NODE_EMISSIVE,
        kWard = RPR_MATERIAL_NODE_WARD,
        kBlend = RPR_MATERIAL_NODE_BLEND,
        kFresnel = RPR_MATERIAL_NODE_FRESNEL,
        kFresnelShlick = RPR_MATERIAL_NODE_FRESNEL_SCHLICK,
        kStandard = RPR_MATERIAL_NODE_STANDARD,
        kPassthrough = RPR_MATERIAL_NODE_PASSTHROUGH,
        kOrennayar = RPR_MATERIAL_NODE_ORENNAYAR,
        kArithmetic = RPR_MATERIAL_NODE_ARITHMETIC,
        kUberV2 = 0x100
    };

    //initialize methods
    static MaterialObject* CreateImage(rpr_image_format const in_format, rpr_image_desc const * in_image_desc, void const * in_data);
    static MaterialObject* CreateImage(const std::string& in_path);  
    static MaterialObject* CreateMaterial(rpr_material_node_type in_type);

    virtual ~MaterialObject() = default;

    bool IsImg() { return m_type == Type::kImage;}
    bool IsMap() { return   m_type == Type::kBumpMap || 
                            m_type == Type::kNormalMap || 
                            m_type == Type::kDotTexture; }
    bool IsArithmetic() const { return m_type == Type::kArithmetic; }

    bool IsTexture() { return IsImg() || IsMap(); }

    //inputs
    void SetInputValue(const std::string& input_name, MaterialObject* input);
    void SetInputValue(const std::string& input_name, const RadeonRays::float4& val);
    void SetInputValue(const std::string& input_name, rpr_uint val);

    //Get*
    Type GetType() { return m_type; }
    virtual Baikal::Texture::Ptr GetTexture();
    virtual Baikal::Material::Ptr GetMaterial();

    //rprMaterialGetInfo:
    uint64_t GetInputCount();
    rpr_uint GetInputType(int i);
    virtual void GetInput(int i, void* out, size_t* out_size);
    //get input by index.
    //Note: RPR related input name
    std::string GetInputName(int i);

    //get input name by material
    std::string GetInputTranslatedName(MaterialObject* mat);

    //rprImageGetInfo:
    virtual rpr_image_desc GetImageDesc() const;
    virtual char const* GetImageData() const;
    virtual rpr_image_format GetImageFormat() const;
protected:
    MaterialObject(Type type);
    //type - is type of input material
    std::string TranslatePropName(const std::string& in, Type type = Type::kDiffuse);

    //this call by SetInputMaterial based on input type
    virtual void SetInputMaterial(const std::string& input_name, MaterialObject* input);
    virtual void SetInputTexture(const std::string& input_name, TextureMaterialObject* input);
    virtual void SetInputImage(const std::string& input_name, ImageMaterialObject* input);
    virtual void SetInputF(const std::string& input_name, const RadeonRays::float4& val);
    virtual void SetInputU(const std::string& input_name, rpr_uint val);
private:

    //handle input materials, it need for correct rprMaterialGet* methods.
    //Note: input_name is RPR input name
    void SetInput(MaterialObject* input_mat, const std::string& input_name);

    //add and remove output materials
    void AddOutput(MaterialObject* mat);
    void RemoveOutput(MaterialObject* mat);
    void Notify();
    virtual void Update(MaterialObject* mat);



    Type m_type;
    //output materials
    std::set<MaterialObject*> m_out_mats;
    //input material + RPR input name. Required for rprMaterialGet* methods.
    std::map<std::string, MaterialObject*> m_inputs;
};
