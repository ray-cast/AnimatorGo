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

#include <list>

#include "RadeonProRender.h"
#include "WrapObject.h"
#include "SceneGraph/camera.h"

#include "math/matrix.h"
#include "RadeonProRender.h"

namespace Baikal {
    class PerspectiveCamera;
}

class SceneObject;

//this class represent rpr_camera
class RPR_API_ENTRY CameraObject
    : public WrapObject
{
public:
    CameraObject();
    virtual ~CameraObject();

    //camera data
    //Note: some values are converted between meters and mm
    void SetFocalLength(rpr_float flen);
    rpr_float GetFocalLength() { return m_camera_focal_length * 1000.f; }

    void SetFocusDistance(rpr_float fdist);
    rpr_float GetFocusDistance() { return m_camera_focus_distance; }

    void SetSensorSize(RadeonRays::float2 size);
    RadeonRays::float2 GetSensorSize() { return m_camera_sensor_size * 1000.f; }

    void SetOrthoWidth(float width);
    float GetOrthoWidth() { return m_camera_sensor_size.x; }

    void SetOrthoHeight(float height);
    float GetOrthoHeight() { return m_camera_sensor_size.y; }

    void GetLookAt(RadeonRays::float3& eye,
                RadeonRays::float3& at,
                RadeonRays::float3& up);

    void LookAt(RadeonRays::float3 const& eye,
        RadeonRays::float3 const& at,
        RadeonRays::float3 const& up);

    void SetAperture(rpr_float fstop);
    rpr_float GetAperture() { return m_camera_aperture * 1000.f; }

    void SetTransform(const RadeonRays::matrix& m);
    void SetMode(rpr_camera_mode mode);
    rpr_camera_mode GetMode() const { return m_mode; }

    Baikal::Camera::Ptr GetCamera();

    void AddToScene(::SceneObject* scene);
    void RemoveFromScene(::SceneObject* scene);

    CameraObject(CameraObject&&) = default;
    CameraObject& operator= (CameraObject&&) = default;

private:
    void UpdateCameraParams();
    Baikal::Camera::Ptr m_cam = 0;
    rpr_camera_mode m_mode = RPR_CAMERA_MODE_PERSPECTIVE;

    RadeonRays::float3 m_eye = { 0.f, 0.f, 0.f };
    RadeonRays::float3 m_at = { 0.f , 0.f , -1.f };
    RadeonRays::float3 m_up = { 0.f , 1.f , 0.f };
    RadeonRays::float2 m_camera_sensor_size = { 0.036f, 0.024f };  // default full frame sensor 36x24 mm
    RadeonRays::float2 m_camera_zcap = { 0.0f, 100000.f };
    float m_camera_focal_length = 0.035f; // 35mm lens
    float m_camera_focus_distance = 1.f;
    float m_camera_aperture = 0.f;

    std::list<::SceneObject*> m_scenes;
};
