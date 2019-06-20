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
#include <OpenImageIO/imageio.h>
#include <map>
#include <memory>

#include "TextureMaterialObject.h"
#include "ImageMaterialObject.h"
#include "WrapObject/Exception.h"


using namespace RadeonRays;
using namespace Baikal;

TextureMaterialObject::TextureMaterialObject(MaterialObject::Type type)
    : MaterialObject(type)
    , m_tex(Baikal::Texture::Create())
{
}

void TextureMaterialObject::SetInputImage(const std::string& input_name, ImageMaterialObject* input)
{
    if (input_name == "data")
    {
        CopyData(input);
    }
    else
    {
        throw Exception(RPR_ERROR_INVALID_TAG, "MaterialObject: invalid tag for texture.");
    }
}
void TextureMaterialObject::SetInputTexture(const std::string& input_name, TextureMaterialObject* input)
{
    if (input_name == "albedo" && IsMap())
    {
        CopyData(input);
    }
    else
    {
        throw Exception(RPR_ERROR_INVALID_TAG, "MaterialObject: invalid tag for texture.");
    }
}

void TextureMaterialObject::CopyData(MaterialObject* in)
{
    //copy image data
    auto tex = in->GetTexture();
    const char* data = tex->GetData();
    auto size = tex->GetSize();
    auto format = tex->GetFormat();
    char* tex_data = new char[tex->GetSizeInBytes()];
    memcpy(tex_data, data, tex->GetSizeInBytes());

    m_tex->SetData(tex_data, size, format);
}

rpr_image_desc TextureMaterialObject::GetImageDesc() const
{
    auto size = m_tex->GetSize();
    rpr_uint depth = (rpr_uint)m_tex->GetSizeInBytes() / size.x / size.y;
    return{ (rpr_uint)size.x, (rpr_uint)size.y, depth, 0, 0 };
}
char const* TextureMaterialObject::GetImageData() const
{
    return m_tex->GetData();
}
rpr_image_format TextureMaterialObject::GetImageFormat() const
{
    rpr_component_type type;
    switch (m_tex->GetFormat())
    {
    case Baikal::Texture::Format::kRgba8:
        type = RPR_COMPONENT_TYPE_UINT8;
        break;
    case Baikal::Texture::Format::kRgba16:
        type = RPR_COMPONENT_TYPE_FLOAT16;
        break;
    case Baikal::Texture::Format::kRgba32:
        type = RPR_COMPONENT_TYPE_FLOAT32;
        break;
    default:
        throw Exception(RPR_ERROR_INTERNAL_ERROR, "MaterialObject: invalid image format.");
    }
    //only 4component textures used
    return{ 4, type };
}

Baikal::Texture::Ptr TextureMaterialObject::GetTexture() 
{
    return m_tex;
}
