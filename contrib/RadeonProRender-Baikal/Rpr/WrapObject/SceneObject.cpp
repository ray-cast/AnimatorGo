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

#include "WrapObject/SceneObject.h"
#include "WrapObject/ShapeObject.h"
#include "WrapObject/LightObject.h"
#include "WrapObject/CameraObject.h"
#include "SceneGraph/scene1.h"
#include "SceneGraph/light.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/material.h"
#include "SceneGraph/iterator.h"

#include <assert.h>

SceneObject::SceneObject()
    : m_scene(nullptr)
{
    m_scene = Baikal::Scene1::Create();
}

SceneObject::~SceneObject()
{
}

void SceneObject::SetScene(Baikal::Scene1::Ptr scene)
{
    m_scene = std::move(scene);
}

void SceneObject::Clear()
{
    m_shapes.clear();
    m_lights.clear();

    //remove lights
    for (std::unique_ptr<Baikal::Iterator> it_light(m_scene->CreateLightIterator()); it_light->IsValid();)
    {
        m_scene->DetachLight(it_light->ItemAs<Baikal::Light>());
        it_light = m_scene->CreateLightIterator();
    }

    //remove shapes
    for (std::unique_ptr<Baikal::Iterator> it_shape(m_scene->CreateShapeIterator()); it_shape->IsValid();)
    {
        m_scene->DetachShape(it_shape->ItemAs<Baikal::Mesh>());
        it_shape = m_scene->CreateShapeIterator();
    }

    if (m_current_camera) m_current_camera->RemoveFromScene(this);
}

void SceneObject::AttachShape(ShapeObject* shape)
{
    //check is mesh already in scene
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it != m_shapes.end())
    {
        return;
    }
    m_shapes.push_back(shape);

    m_scene->AttachShape(shape->GetShape());
}

void SceneObject::DetachShape(ShapeObject* shape)
{
    //check is mesh in scene
    auto it = std::find(m_shapes.begin(), m_shapes.end(), shape);
    if (it == m_shapes.end())
    {
        return;
    }
    m_shapes.erase(it);
    m_scene->DetachShape(shape->GetShape());
}

void SceneObject::AttachLight(LightObject* light)
{
    //check is light already in scene
    auto it = std::find(m_lights.begin(), m_lights.end(), light);
    if (it != m_lights.end())
    {
        return;
    }
    m_lights.push_back(light);

    m_scene->AttachLight(light->GetLight());
}

void SceneObject::DetachLight(LightObject* light)
{
    //check is light in scene
    auto it = std::find(m_lights.begin(), m_lights.end(), light);
    if (it == m_lights.end())
    {
        return;
    }
    m_lights.erase(it);
    m_scene->DetachLight(light->GetLight());
}

void SceneObject::SetCamera(CameraObject* cam)
{
    m_current_camera = cam;
    if (m_current_camera) cam->AddToScene(this);
    auto baikal_cam = cam ? cam->GetCamera() : nullptr;
    m_scene->SetCamera(baikal_cam);
}

void SceneObject::GetShapeList(void* out_list)
{
    memcpy(out_list, m_shapes.data(), m_shapes.size() * sizeof(ShapeObject*));
}

void SceneObject::GetLightList(void* out_list)
{
    memcpy(out_list, m_lights.data(), m_lights.size() * sizeof(LightObject*));
}

RadeonRays::bbox SceneObject::GetBBox()
{
    return m_scene->GetWorldAABB();
}


void SceneObject::AddEmissive()
{
    //TODO: check scene isn't changed
    //recreate amissives if scene is dirty
    if (!m_scene->GetDirtyFlags())
    {
        return;
    }
    RemoveEmissive();

    //handle emissive materials
    for (std::unique_ptr<Baikal::Iterator> it_shape(m_scene->CreateShapeIterator()); it_shape->IsValid(); it_shape->Next())
    {
        auto shape = it_shape->ItemAs<Baikal::Shape>();
        auto mesh = std::dynamic_pointer_cast<Baikal::Mesh>(shape);
        //instance case
        if (!mesh)
        {
            auto inst = std::dynamic_pointer_cast<Baikal::Instance>(shape);
            assert(inst);
            mesh = std::dynamic_pointer_cast<Baikal::Mesh>(inst->GetBaseShape());
        }
        assert(mesh);

        auto mat = mesh->GetMaterial();
        if (!mat)
        {
            continue;
        }
        //fine shapes with emissive material
        if (mat->HasEmission())
        {
            // Add area light for each polygon of emissive mesh
            for (std::size_t l = 0; l < mesh->GetNumIndices() / 3; ++l)
            {
                auto light = Baikal::AreaLight::Create(mesh, l);
                m_scene->AttachLight(light);
                m_emmisive_lights.push_back(light);
            }
        }
    }
}

void SceneObject::RemoveEmissive()
{
    for (auto light : m_emmisive_lights)
    {
        m_scene->DetachLight(light);
    }
    
    m_emmisive_lights.clear();
}

bool SceneObject::IsDirty()
{
    auto dirty = m_scene->GetDirtyFlags();
    return dirty != Baikal::Scene1::kNone;
}

void SceneObject::SetBackgroundImage(MaterialObject* image)
{
    m_background_image = image;
    if (m_background_image && m_background_image->IsTexture())
        m_scene->SetBackgroundImage(m_background_image->GetTexture());
    else
       m_scene->SetBackgroundImage(nullptr);
}

MaterialObject* SceneObject::GetBackgroundImage() const
{
    return m_background_image;
}

void SceneObject::SetEnvironmentOverride(OverrideType overrride, LightObject* light)
{
    switch (overrride)
    {
    case OverrideType::kBackground:
        m_environment_override.m_background = light;
        break;
    case OverrideType::kReflection:
        m_environment_override.m_reflection = light;
        break;
    case OverrideType::kRefraction:
        m_environment_override.m_refraction = light;
        break;
    case OverrideType::kTransparency:
        m_environment_override.m_transparency = light;
        break;
    }

    m_scene->SetEnvironmentOverride(m_environment_override.ToScene1EnvironmentOverride());
}

LightObject* SceneObject::GetEnvironmentOverride(OverrideType overrride)
{
    switch (overrride)
    {
    case OverrideType::kBackground:
        return m_environment_override.m_background;
    case OverrideType::kReflection:
        return m_environment_override.m_reflection;
    case OverrideType::kRefraction:
        return m_environment_override.m_refraction;
    case OverrideType::kTransparency:
        return m_environment_override.m_transparency;
    }
	return nullptr;
}

Baikal::Scene1::EnvironmentOverride SceneObject::EnvironmentOverride::ToScene1EnvironmentOverride() const
{
    Baikal::Scene1::EnvironmentOverride out;
    out.m_background = m_background ? 
        std::static_pointer_cast<Baikal::ImageBasedLight>(m_background->GetLight()) : nullptr;

    out.m_reflection = m_reflection ? 
        std::static_pointer_cast<Baikal::ImageBasedLight>(m_reflection->GetLight()) : nullptr;

    out.m_refraction = m_refraction ? 
        std::static_pointer_cast<Baikal::ImageBasedLight>(m_refraction->GetLight()) : nullptr;

    out.m_transparency = m_transparency ? 
        std::static_pointer_cast<Baikal::ImageBasedLight>(m_transparency->GetLight()) : nullptr;

    return out;
}
