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

#include "MaterialObject.h"

//materials represented as Baikal::Texture
class TextureMaterialObject
    : public MaterialObject
{
public:
    TextureMaterialObject(Type type);
    Baikal::Texture::Ptr GetTexture() override;

    //rprImageGetInfo:
    virtual rpr_image_desc GetImageDesc() const override;
    virtual char const* GetImageData() const override;
    virtual rpr_image_format GetImageFormat() const override;
protected:

    virtual void SetInputTexture(const std::string& input_name, TextureMaterialObject* input) override;
    virtual void SetInputImage(const std::string& input_name, ImageMaterialObject* input) override;
private:

    void CopyData(MaterialObject* in);

    Baikal::Texture::Ptr m_tex;
};
