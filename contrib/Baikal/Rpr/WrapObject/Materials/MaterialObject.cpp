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
#include <map>
#include <memory>

#include "math/int2.h"
#include "SceneGraph/texture.h"
#include "SceneGraph/material.h"
#include "SceneGraph/iterator.h"
#include "WrapObject/Materials/MaterialObject.h"
#include "WrapObject/Exception.h"

#include "ImageMaterialObject.h"
#include "TextureMaterialObject.h"
#include "UnsupportedMaterialObject.h"
#include "ArithmeticMaterialObject.h"
#include "UberMaterialObject.h"

using namespace RadeonRays;
using namespace Baikal;

namespace
{
    //contains pairs <rpr input name, baikal input name> of input names
    const std::map<std::string, std::string> kInputNamesDictionary = { 
        { "color" , "albedo" },
        { "normal" , "normal" },
		{ "roughness" , "roughness" },
		{ "roughness_x" , "roughness" },
        { "weight" , "weight" }, 
        { "ior" , "ior" },
        { "color0", "color0" },
        { "color1", "color1" }, 
        { "color2", "color2" },
        { "color3", "color3" },
        { "data", "data" },
        { "sigmas", "sigmas" },
        { "sigmaa", "sigmaa" },
        { "rotation", "rotation" }, 
        { "anisotropic", "anisotropic" },
        { "g", "g" }, 
        { "op", "op"},
        { "multiscatter", "multiscatter" },
        { "uberv2.diffuse.color", "uberv2.diffuse.color" },
        { "uberv2.layers", "uberv2.layers" },
        { "uberv2.reflection.color", "uberv2.reflection.color" },
        { "uberv2.reflection.roughness", "uberv2.reflection.roughness" },
        { "uberv2.reflection.anisotropy", "uberv2.reflection.anisotropy" },
        { "uberv2.reflection.anisotropy_rotation", "uberv2.reflection.anisotropy_rotation" },
        { "uberv2.reflection.ior", "uberv2.reflection.ior" },
        { "uberv2.reflection.metalness", "uberv2.reflection.metalness" },
        { "uberv2.refraction.color", "uberv2.refraction.color" },
        { "uberv2.refraction.roughness", "uberv2.refraction.roughness" },
        { "uberv2.refraction.ior", "uberv2.refraction.ior" },
        { "uberv2.refraction.thin", "uberv2.refraction.thin" },
        { "uberv2.coating.color", "uberv2.coating.color" },
        { "uberv2.coating.ior", "uberv2.coating.ior" },
        { "uberv2.coating.metalness", "uberv2.coating.metalness" },
        { "uberv2.emission.color", "uberv2.emission.color" },
        { "uberv2.emission.mode", "uberv2.emission.mode" },
        { "uberv2.transparency", "uberv2.transparency" },
        { "uberv2.normal", "uberv2.normal" },
        { "uberv2.bump", "uberv2.bump" },
        { "uberv2.displacement", "uberv2.displacement" },
        { "uberv2.sss.absorption_color", "uberv2.sss.absorption_color" },
        { "uberv2.sss.scatter_color", "uberv2.sss.scatter_color" },
        { "uberv2.sss.absorption_distance", "uberv2.sss.absorption_distance" },
        { "uberv2.sss.scatter_distance", "uberv2.sss.scatter_distance" },
        { "uberv2.sss.scatter_direction", "uberv2.sss.scatter_direction" },
        { "uberv2.sss.subsurface_color", "uberv2.sss.subsurface_color" },
        { "uberv2.sss.multiscatter", "uberv2.sss.multiscatter" }};

    std::map<uint32_t, std::string> kMaterialNodeInputStrings = {
        { RPR_MATERIAL_INPUT_COLOR, "color" },
        { RPR_MATERIAL_INPUT_COLOR0, "color0" },
        { RPR_MATERIAL_INPUT_COLOR1, "color1" },
        { RPR_MATERIAL_INPUT_NORMAL, "normal" },
        { RPR_MATERIAL_INPUT_UV, "uv" },
        { RPR_MATERIAL_INPUT_DATA, "data" },
        { RPR_MATERIAL_INPUT_ROUGHNESS, "roughness" },
        { RPR_MATERIAL_INPUT_IOR, "ior" },
        { RPR_MATERIAL_INPUT_ROUGHNESS_X, "roughness_x" },
        { RPR_MATERIAL_INPUT_ROUGHNESS_Y, "roughness_y" },
        { RPR_MATERIAL_INPUT_ROTATION, "rotation" },
        { RPR_MATERIAL_INPUT_WEIGHT, "weight" },
        { RPR_MATERIAL_INPUT_OP, "op" },
        { RPR_MATERIAL_INPUT_INVEC, "invec" },
        { RPR_MATERIAL_INPUT_UV_SCALE, "uv_scale" },
        { RPR_MATERIAL_INPUT_VALUE, "value" },
        { RPR_MATERIAL_INPUT_REFLECTANCE, "reflectance" },
        { RPR_MATERIAL_INPUT_SCALE, "bumpscale" },
        { RPR_MATERIAL_INPUT_SCATTERING, "sigmas" },
        { RPR_MATERIAL_INPUT_ABSORBTION, "sigmaa" },
        { RPR_MATERIAL_INPUT_EMISSION, "emission" },
        { RPR_MATERIAL_INPUT_G, "g" },
        { RPR_MATERIAL_INPUT_MULTISCATTER, "multiscatter" },

        { RPR_MATERIAL_STANDARD_INPUT_DIFFUSE_COLOR, "diffuse.color" },
        { RPR_MATERIAL_STANDARD_INPUT_DIFFUSE_NORMAL, "diffuse.normal" },
        { RPR_MATERIAL_STANDARD_INPUT_GLOSSY_COLOR, "glossy.color" },
        { RPR_MATERIAL_STANDARD_INPUT_GLOSSY_NORMAL, "glossy.normal" },
        { RPR_MATERIAL_STANDARD_INPUT_GLOSSY_ROUGHNESS_X, "glossy.roughness_x" },
        { RPR_MATERIAL_STANDARD_INPUT_GLOSSY_ROUGHNESS_Y, "glossy.roughness_y" },
        { RPR_MATERIAL_STANDARD_INPUT_CLEARCOAT_COLOR, "clearcoat.color" },
        { RPR_MATERIAL_STANDARD_INPUT_CLEARCOAT_NORMAL, "clearcoat.normal" },
        { RPR_MATERIAL_STANDARD_INPUT_REFRACTION_COLOR, "refraction.color" },
        { RPR_MATERIAL_STANDARD_INPUT_REFRACTION_NORMAL, "refraction.normal" },
        { RPR_MATERIAL_STANDARD_INPUT_REFRACTION_ROUGHNESS, "refraction.roughness" },   //  REFRACTION doesn't have roughness parameter.
        { RPR_MATERIAL_STANDARD_INPUT_REFRACTION_IOR, "refraction.ior" },
        { RPR_MATERIAL_STANDARD_INPUT_TRANSPARENCY_COLOR, "transparency.color" },
        { RPR_MATERIAL_STANDARD_INPUT_DIFFUSE_TO_REFRACTION_WEIGHT, "weights.diffuse2refraction" },
        { RPR_MATERIAL_STANDARD_INPUT_GLOSSY_TO_DIFFUSE_WEIGHT, "weights.glossy2diffuse" },
        { RPR_MATERIAL_STANDARD_INPUT_CLEARCOAT_TO_GLOSSY_WEIGHT, "weights.clearcoat2glossy" },
        { RPR_MATERIAL_STANDARD_INPUT_TRANSPARENCY, "weights.transparency" },

    };

    //these material inputs are ignored
    const std::set<std::string> kUnsupportedInputs = { "anisotropic" };
    
    //these materials are unsupported now:
    const std::set<int> kUnsupportedMaterials = {   RPR_MATERIAL_NODE_ADD,
                                                    RPR_MATERIAL_NODE_ARITHMETIC,
                                                    RPR_MATERIAL_NODE_BLEND_VALUE,
                                                    RPR_MATERIAL_NODE_VOLUME,
                                                    RPR_MATERIAL_NODE_INPUT_LOOKUP,
                                                    RPR_MATERIAL_NODE_MICROFACET_ANISOTROPIC_REFLECTION,
                                                    RPR_MATERIAL_NODE_MICROFACET_ANISOTROPIC_REFRACTION,
                                                    RPR_MATERIAL_NODE_TWOSIDED,
                                                    RPR_MATERIAL_NODE_UV_PROCEDURAL,};
}


MaterialObject* MaterialObject::CreateImage(rpr_image_format const in_format, rpr_image_desc const * in_image_desc, void const * in_data)
{
    return new ImageMaterialObject(in_format, in_image_desc, in_data);
}

MaterialObject* MaterialObject::CreateImage(const std::string& in_path)
{
    return new ImageMaterialObject(in_path);
}

MaterialObject* MaterialObject::CreateMaterial(rpr_material_node_type in_type)
{
    Type type = (Type)in_type;
    switch (in_type)
    {
    // SingleBxdf
    case RPR_MATERIAL_NODE_ADD:
    case RPR_MATERIAL_NODE_BLEND_VALUE:
    case RPR_MATERIAL_NODE_VOLUME:
    case RPR_MATERIAL_NODE_INPUT_LOOKUP:
    case RPR_MATERIAL_NODE_UV_PROCEDURAL:

    {
        return new UnsupportedMaterialObject(type);
    }


    case RPR_MATERIAL_NODE_ARITHMETIC:
    {
        return new ArithmeticMaterialObject(type);
    }

    //Textures
    case RPR_MATERIAL_NODE_NORMAL_MAP:
    case RPR_MATERIAL_NODE_IMAGE_TEXTURE:
    case RPR_MATERIAL_NODE_NOISE2D_TEXTURE:
    case RPR_MATERIAL_NODE_DOT_TEXTURE:
    case RPR_MATERIAL_NODE_GRADIENT_TEXTURE:
    case RPR_MATERIAL_NODE_CHECKER_TEXTURE:
    case RPR_MATERIAL_NODE_CONSTANT_TEXTURE:
    case RPR_MATERIAL_NODE_BUMP_MAP:
        return new TextureMaterialObject(type);

    case RPR_MATERIAL_NODE_UBERV2:
    {
        return new UberMaterialObject();
    }
    default:
        throw Exception(RPR_ERROR_INVALID_PARAMETER_TYPE, "MaterialObject: unrecognized material type");
    }

    return nullptr;
}


MaterialObject::MaterialObject(Type type)
    : m_type(type)
{
}

std::string MaterialObject::TranslatePropName(const std::string& in, Type type)
{
    std::string result;
    //if (type == Type::kBumpMap)
    //{
    //    return "bump";
    //}
    //else if (type == Type::kNormalMap)
    //{
    //    return "normal";
    //}

    auto it = kInputNamesDictionary.find(in);
    if (it != kInputNamesDictionary.end())
    {
        result = it->second;
    }
    else
    {
        throw Exception(RPR_ERROR_UNIMPLEMENTED, "MaterialObject: unimplemented input.");

    }

    return result;
}


void MaterialObject::SetInputValue(const std::string& input_name, MaterialObject* input)
{
    ImageMaterialObject* img = dynamic_cast<ImageMaterialObject*>(input);
    TextureMaterialObject* tex = dynamic_cast<TextureMaterialObject*>(input);

    std::string name = TranslatePropName(input_name, GetType());
    if (img)
    {
        SetInputImage(name, img);
    }
    else if (tex)
    {
        SetInputTexture(name, tex);
    }
    else
    {
        SetInputMaterial(name, input);
    }
    
    input->AddOutput(this);
    SetInput(input, input_name);
    Notify();
}

void MaterialObject::SetInputValue(const std::string& input_name, const RadeonRays::float4& val)
{
    std::string name = TranslatePropName(input_name, GetType());
    SetInputF(name, val);

    SetInput(nullptr, input_name);
    Notify();
}

void MaterialObject::SetInputValue(const std::string& input_name, rpr_uint val)
{
    std::string name = TranslatePropName(input_name, GetType());
    SetInputU(name, val);

    SetInput(nullptr, input_name);
    Notify();
}

void MaterialObject::SetInput(MaterialObject* input_mat, const std::string& input_name)
{
    //remove link with previous input
    MaterialObject* old_input = m_inputs[input_name];
    if (old_input)
    {
        old_input->RemoveOutput(this);
    }

    m_inputs[input_name] = input_mat;
}


void MaterialObject::AddOutput(MaterialObject* mat)
{
    m_out_mats.insert(mat);
}

void MaterialObject::RemoveOutput(MaterialObject* mat)
{
    m_out_mats.erase(mat);
}

void MaterialObject::Notify()
{
    for (auto mat : m_out_mats)
    {
        mat->Update(this);
    }
}

void MaterialObject::Update(MaterialObject* mat)
{

}

uint64_t MaterialObject::GetInputCount()
{
    return m_inputs.size();
}

rpr_uint MaterialObject::GetInputType(int i)
{
    rpr_uint result = RPR_MATERIAL_NODE_INPUT_TYPE_NODE;
    //get i input
    auto it = m_inputs.begin();
    for (int index = 0; it != m_inputs.end(); ++it, ++index)
        if (index == i)
        {
            break;
        }

    //means no MaterialObject connected
    if (!it->second)
    {
        result = RPR_MATERIAL_NODE_INPUT_TYPE_FLOAT4;
    }
    //connected input represent rpr_image
    else if(it->second->IsImg())
    {
        result = RPR_MATERIAL_NODE_INPUT_TYPE_IMAGE;
    }

    return result;
}

std::string MaterialObject::GetInputName(int i)
{
    //get i input
    auto it = m_inputs.begin();
    for (int index = 0; it != m_inputs.end(); ++it, ++index)
        if (index == i)
        {
            break;
        }

    return it->first;
}

std::string MaterialObject::GetInputTranslatedName(MaterialObject* mat)
{
    auto input_it = std::find_if(m_inputs.begin(), m_inputs.end(), [mat](const std::pair<std::string, MaterialObject*>& i) { return i.second == mat; });
    if (input_it == m_inputs.end())
    {
        throw Exception(RPR_ERROR_INTERNAL_ERROR, "MaterialObject: failed to find requested input node.");
    }
    return TranslatePropName(input_it->first, mat->GetType());
}


void MaterialObject::GetInput(int i, void* out, size_t* out_size)
{
    //get i input
    auto it = m_inputs.begin();
    for (int index = 0; it != m_inputs.end(); ++it, ++index)
        if (index == i)
        {
            break;
        }

    if (it == m_inputs.end()) return;
    //translated name
    std::string trans_name = TranslatePropName(it->first);
    //means no MaterialObject connected
    if (!it->second)
    {
        Material::Ptr mat = GetMaterial();
        *out_size = sizeof(RadeonRays::float4);
        Baikal::Material::InputValue value = mat->GetInputValue(trans_name);
        if (value.type != Baikal::Material::InputType::kFloat4)
        {
            throw Exception(RPR_ERROR_INTERNAL_ERROR, "MaterialObject: material input type is unexpected.");
        }

        memcpy(out, &value.float_value, *out_size);
    }
    //images, textures and materials
    else
    {
        *out_size = sizeof(rpr_material_node);
        memcpy(out, &it->second, *out_size);
    }
}

//this call by SetInputMaterial based on input type
void MaterialObject::SetInputMaterial(const std::string& input_name, MaterialObject* input)
{
    throw Exception(RPR_ERROR_INVALID_OBJECT, "Material input not supported for the object.");
}

void MaterialObject::SetInputTexture(const std::string& input_name, TextureMaterialObject* input)
{
    throw Exception(RPR_ERROR_INVALID_OBJECT, "Texture input not supported for the object.");
}

void MaterialObject::SetInputImage(const std::string& input_name, ImageMaterialObject* input)
{
    throw Exception(RPR_ERROR_INVALID_OBJECT, "Image input not supported for the object.");
}

void MaterialObject::SetInputF(const std::string& input_name, const RadeonRays::float4& val)
{
    throw Exception(RPR_ERROR_INVALID_OBJECT, "Float4 input not supported for the object.");
}

void MaterialObject::SetInputU(const std::string& input_name, rpr_uint val)
{
    throw Exception(RPR_ERROR_INVALID_OBJECT, "rpr_uint input not supported for the object.");
}

Baikal::Texture::Ptr MaterialObject::GetTexture()
{
    throw Exception(RPR_ERROR_INVALID_OBJECT, "This is not texture object");
}

Baikal::Material::Ptr MaterialObject::GetMaterial()
{
    throw Exception(RPR_ERROR_INVALID_OBJECT, "This is not material object");
}

rpr_image_desc MaterialObject::GetImageDesc() const
{
    throw Exception(RPR_ERROR_INVALID_IMAGE, "This is not image object");
}
char const* MaterialObject::GetImageData() const
{
    throw Exception(RPR_ERROR_INVALID_IMAGE, "This is not image object");

}
rpr_image_format MaterialObject::GetImageFormat() const
{
    throw Exception(RPR_ERROR_INVALID_IMAGE, "This is not image object");
}
