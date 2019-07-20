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
#include "ImageMaterialObject.h"
#include "WrapObject/Exception.h"

#ifdef BAIKAL_ENABLE_IO
#include <OpenImageIO/imageio.h>
#include "image_io.h"
#endif


using namespace RadeonRays;
using namespace Baikal;

ImageMaterialObject::ImageMaterialObject(rpr_image_format const in_format, rpr_image_desc const * in_image_desc, void const * in_data)
    : MaterialObject(Type::kImage)
{
    //tex size
    int2 tex_size(in_image_desc->image_width, in_image_desc->image_height);

    //texture takes ownership of its data array
    //so need to copy input data
    int pixels_count = tex_size.x * tex_size.y;

    //bytes per pixel
    int pixel_bytes = in_format.num_components;
    int component_bytes = 1;
    Texture::Format data_format = Texture::Format::kRgba8;
    switch (in_format.type)
    {
    case RPR_COMPONENT_TYPE_UINT8:
        break;
    case RPR_COMPONENT_TYPE_FLOAT16:
        component_bytes = 2;
        data_format = Texture::Format::kRgba16;
        break;
    case RPR_COMPONENT_TYPE_FLOAT32:
        component_bytes = 4;
        data_format = Texture::Format::kRgba32;
        break;
    default:
        throw Exception(RPR_ERROR_INVALID_PARAMETER, "TextureObject: invalid format type.");
    }
    pixel_bytes *= component_bytes;
    int data_size = 4 * component_bytes * pixels_count;//4 component baikal texture
    char* data = new char[data_size];
    if (in_format.num_components == 4)
    {
        //copy data
        memcpy(data, in_data, data_size);
    }
    else
    {
        //copy to 4component texture
        const char* in_data_cast = static_cast<const char*>(in_data);
        for (int i = 0; i < pixels_count; ++i)
        {
            //copy
            for (unsigned int comp_ind = 0; comp_ind < in_format.num_components; ++comp_ind)
            {
                int index = comp_ind * component_bytes;
                memcpy(&data[i * 4 * component_bytes + index], &in_data_cast[i * in_format.num_components * component_bytes + index], component_bytes);
            }
            //clean other colors
            for (unsigned int comp_ind = in_format.num_components; comp_ind < 4; ++comp_ind)
            {
                int index = comp_ind * component_bytes;
                memset(&data[i * 4 * component_bytes + index], 0, component_bytes);
            }
        }
    }
    m_tex = Texture::Create(data, tex_size, data_format);
}

ImageMaterialObject::ImageMaterialObject(const std::string& in_path)
    : MaterialObject(Type::kImage)
{
#ifdef BAIKAL_ENABLE_IO
    //load texture using oiio
    std::unique_ptr<ImageIo> oiio = ImageIo::CreateImageIo();
    Texture::Ptr texture = nullptr;
    try
    {
        texture = oiio->LoadImage(in_path);
    }
    catch (...) {
        throw Exception(RPR_ERROR_IO_ERROR, "TextureObject: failed to load image.");
    }

    m_tex = texture;
#else
	throw Exception(RPR_ERROR_IO_ERROR, "TextureObject: failed to load image.");
#endif
}

Baikal::Texture::Ptr ImageMaterialObject::GetTexture()
{ 
    return m_tex; 
}
