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

#include "WrapObject/CameraObject.h"
#include "SceneGraph/camera.h"
#include "radeon_rays.h"

#include <assert.h>

#include "WrapObject/SceneObject.h"

using namespace Baikal;
using namespace RadeonRays;

CameraObject::CameraObject()
    : m_cam(nullptr)
{
}

CameraObject::~CameraObject()
{
}

void CameraObject::SetTransform(const RadeonRays::matrix& m)
{
    //default values
    RadeonRays::float4 eye(0.f, 0.f, 0.f, 1.0f);
    RadeonRays::float4 at(0.f, 0.f, -1.f, 0.0f);
    RadeonRays::float4 up(0.f, 1.f, 0.f, 0.0f);
    m_eye = m * eye;
    m_at = m * at;
    m_up = m * up;
    UpdateCameraParams();
}

void CameraObject::GetLookAt(RadeonRays::float3& eye,
    RadeonRays::float3& at,
    RadeonRays::float3& up)
{
    eye = m_eye;
    at = m_at;
    up = m_up;
}

Baikal::Camera::Ptr CameraObject::GetCamera() 
{ 
    if (m_mode == RPR_CAMERA_MODE_PERSPECTIVE)
    {
        Baikal::PerspectiveCamera::Ptr camera = PerspectiveCamera::Create(m_eye, m_at, -m_up);
        camera->SetSensorSize(m_camera_sensor_size);
        camera->SetDepthRange(m_camera_zcap);
        camera->SetFocalLength(m_camera_focal_length);
        camera->SetFocusDistance(m_camera_focus_distance);
        camera->SetAperture(m_camera_aperture);
        m_cam = camera;
        return m_cam;
    }
    else if (m_mode == RPR_CAMERA_MODE_ORTHOGRAPHIC)
    {
        Baikal::OrthographicCamera::Ptr camera = OrthographicCamera::Create(m_eye, m_at, m_up);
        camera->SetSensorSize(m_camera_sensor_size);
        camera->SetDepthRange(m_camera_zcap);
        m_cam = camera;
        return m_cam;
    }

    assert(!"Usupported camera type");
    return nullptr; 
}

void CameraObject::LookAt(RadeonRays::float3 const& eye,
    RadeonRays::float3 const& at,
    RadeonRays::float3 const& up)
{
    m_eye = eye;
    m_at = at;
    m_up = up;
    UpdateCameraParams();
}

void CameraObject::SetMode(rpr_camera_mode mode)
{
    m_mode = mode;
    UpdateCameraParams();
}

void CameraObject::UpdateCameraParams()
{
    // Replace camera in all scenes that have it
    for (::SceneObject* scene : m_scenes)
    {
        scene->SetCamera(this);
    }
}

void CameraObject::AddToScene(::SceneObject* scene)
{
    auto it = std::find(m_scenes.begin(), m_scenes.end(), scene);
    if (it != m_scenes.end()) return;

    m_scenes.push_back(scene);
}

void CameraObject::RemoveFromScene(::SceneObject* scene)
{
    m_scenes.remove(scene);
}

void CameraObject::SetFocalLength(rpr_float flen) 
{ 
    m_camera_focal_length = flen / 1000.f;
    UpdateCameraParams(); 
}

void CameraObject::SetFocusDistance(rpr_float fdist) 
{ 
    m_camera_focus_distance = fdist; 
    UpdateCameraParams(); 
}

void CameraObject::SetSensorSize(RadeonRays::float2 size) 
{ 
    m_camera_sensor_size = size * 0.001f;
    UpdateCameraParams(); 
}

void CameraObject::SetOrthoWidth(float width) 
{ 
    m_camera_sensor_size.x = width;
    UpdateCameraParams(); 
}

void CameraObject::SetOrthoHeight(float height) 
{ 
    m_camera_sensor_size.y = height;
    UpdateCameraParams(); 
}

void CameraObject::SetAperture(rpr_float fstop) 
{ 
    if (fstop > 1000.f)
    {
        fstop = 0.f;
    }
    m_camera_aperture = fstop / 1000.f;
    UpdateCameraParams(); 
}
