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

#include "RadeonProRender.h"
#include "WrapObject/WrapObject.h"
#include "SceneGraph/scene1.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/light.h"

#include <vector>

class ShapeObject;
class LightObject;
class CameraObject;
class MaterialObject;

//this class represent rpr_context
class RPR_API_ENTRY SceneObject
    : public WrapObject
{
public:
    enum class OverrideType
    {
        kReflection,
        kRefraction,
        kTransparency,
        kBackground
    };

    SceneObject();
    virtual ~SceneObject();

    void Clear();
    
    //shape
    void AttachShape(ShapeObject* shape);
    void DetachShape(ShapeObject* shape);

    //light
    void AttachLight(LightObject* light);
    void DetachLight(LightObject* light);
    
    //camera
    void SetCamera(CameraObject* cam);
    CameraObject* GetCamera() { return m_current_camera; }

    void GetShapeList(void* out_list);
    size_t GetShapeCount() { return m_scene->GetNumShapes(); }
    
    void GetLightList(void* out_list);
    size_t GetLightCount() { return m_scene->GetNumLights(); }

    RadeonRays::bbox GetBBox();

    // Image override
    void SetBackgroundImage(MaterialObject* image);
    MaterialObject* GetBackgroundImage() const;

    // Environment override
    void SetEnvironmentOverride(OverrideType overrride, LightObject* light);
    LightObject* GetEnvironmentOverride(OverrideType overrride);

    void AddEmissive();
    void RemoveEmissive();
    bool IsDirty();

    void SetScene(Baikal::Scene1::Ptr scene);
    Baikal::Scene1::Ptr GetScene() { return m_scene; };

private:
    Baikal::Scene1::Ptr m_scene;
    CameraObject* m_current_camera = nullptr;
    std::vector<Baikal::AreaLight::Ptr> m_emmisive_lights;//area lights fro emissive shapes
    std::vector<ShapeObject*> m_shapes;
    std::vector<LightObject*> m_lights;
    MaterialObject *m_background_image = nullptr;

    struct EnvironmentOverride
    {
        LightObject* m_reflection = nullptr;
        LightObject* m_refraction = nullptr;
        LightObject* m_transparency = nullptr;
        LightObject* m_background = nullptr;
        Baikal::Scene1::EnvironmentOverride ToScene1EnvironmentOverride() const;
    } m_environment_override;
};
