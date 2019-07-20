/**********************************************************************
 Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
 
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

#include "BaikalOld/SceneGraph/material.h"

#include "SceneGraph/uberv2material.h"

#include <set>

class MaterialConverter
{
public:
    static std::set<Baikal::UberV2Material::Ptr> TranslateMaterials(std::set<BaikalOld::Material::Ptr> const& old_materials);

private:
    static Baikal::Texture::Format TranslateFormat(BaikalOld::Texture::Format old_format);
    static Baikal::InputMap::Ptr TranslateInput(BaikalOld::Material::Ptr old_mtl, std::string const& input_name);
    static Baikal::UberV2Material::Ptr TranslateSingleBxdfMaterial(BaikalOld::SingleBxdf::Ptr old_mtl);
    static Baikal::UberV2Material::Ptr MergeFresnelBlendMaterials(Baikal::UberV2Material::Ptr base, Baikal::UberV2Material::Ptr top, BaikalOld::Material::Ptr mtl);
    static Baikal::UberV2Material::Ptr MergeMixMaterials(Baikal::UberV2Material::Ptr base, Baikal::UberV2Material::Ptr top, BaikalOld::Material::Ptr mtl);
    static Baikal::UberV2Material::Ptr TranslateFresnelBlend(BaikalOld::MultiBxdf::Ptr mtl);
    static Baikal::UberV2Material::Ptr TranslateMix(BaikalOld::MultiBxdf::Ptr mtl);
    static Baikal::UberV2Material::Ptr TranslateMultiBxdfMaterial(BaikalOld::MultiBxdf::Ptr mtl);
    static Baikal::UberV2Material::Ptr TranslateMaterial(BaikalOld::Material::Ptr mtl);

};
