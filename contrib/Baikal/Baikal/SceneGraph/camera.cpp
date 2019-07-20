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
#include "camera.h"

#include <cmath>
#include <cassert>

#include "math/quaternion.h"
#include "math/matrix.h"
#include "math/mathutils.h"


namespace Baikal
{
    using namespace RadeonRays;
    
    Camera::Camera(float3 const& eye, float3 const& at, float3 const& up) 
        : m_volume(nullptr)
    {
        LookAt(eye, at, up);
    }
    
    PerspectiveCamera::PerspectiveCamera(float3 const& eye, float3 const& at, float3 const& up)
    : Camera(eye, at, up)
    , m_focal_length(0.f)
    , m_focus_distance(0.f)
    , m_aperture(0.f)
    {
    }

    void Camera::LookAt(RadeonRays::float3 const& eye,
                        RadeonRays::float3 const& at,
                        RadeonRays::float3 const& up)
    {
        m_p = eye;
        m_forward = normalize(at - eye);
        m_right = normalize(cross(m_forward, up));
        m_up = cross(m_right, m_forward);
        m_at = at;
        SetDirty(true);
    }
    
    // Rotate camera around world Z axis, use for FPS camera
    void Camera::Rotate(float angle)
    {
        //Rotate(float3(0.f, 1.f, 0.f), angle);
        RotateOnOrbit(float3(0.f, 1.f, 0.f), angle);
        SetDirty(true);
    }
    
    void Camera::Rotate(float3 v, float angle)
    {
        /// matrix should have basis vectors in rows
        /// to be used for quaternion construction
        /// would be good to add several options
        /// to quaternion class
        matrix cam_matrix = matrix(
                                   m_up.x, m_up.y, m_up.z, 0.f,
                                   m_right.x, m_right.y, m_right.z, 0.f,
                                   m_forward.x, m_forward.y, m_forward.z, 0.f,
                                   0.f, 0.f, 0.f, 1.f);
        
        // Create camera orientation quaternion
        quaternion q = normalize(quaternion(cam_matrix));
        
        // Rotate camera frame around v
        q = q * rotation_quaternion(v, -angle);
        
        // Uncomress back to lookat
        q.to_matrix(cam_matrix);
        
        m_up = normalize(float3(cam_matrix.m00, cam_matrix.m01, cam_matrix.m02));
        m_right = normalize(float3(cam_matrix.m10, cam_matrix.m11, cam_matrix.m12));
        m_forward = normalize(float3(cam_matrix.m20, cam_matrix.m21, cam_matrix.m22));
        SetDirty(true);
    }

    void Camera::RotateOnOrbit(float3 v, float angle)
    {
        /// matrix should have basis vectors in rows
        /// to be used for quaternion construction
        /// would be good to add several options
        /// to quaternion class
        auto direction = m_p - m_at;
        auto up = m_up;

        // Rotate camera frame around v
        auto q = rotation_quaternion(v, angle);
        auto new_direction = rotate_vector(direction, q);
        auto new_up = rotate_vector(up, q);

        LookAt(m_at + new_direction, m_at, new_up);

        SetDirty(true);
    }
    
    // Tilt camera
    void Camera::Tilt(float angle)
    {
        RotateOnOrbit(m_right, angle);
        //Rotate(m_right, angle);
        SetDirty(true);
    }
    
    // Move along camera Z direction
    void Camera::MoveForward(float distance)
    {
        m_p += distance * m_forward;
        m_at += distance * m_forward;
        SetDirty(true);
    }

    void Camera::Zoom(float distance)
    {
        if (distance < 0.f || (m_p - m_at).sqnorm() > 1.f)
        {
            m_p += distance * m_forward;
            LookAt(m_p, m_at, m_up);
            SetDirty(true);
        }
    }
    
    // Move along camera X direction
    void Camera::MoveRight(float distance)
    {
        m_p += distance * m_right;
        m_at += distance * m_right;
        SetDirty(true);
    }
    
    // Move along camera Y direction
    void Camera::MoveUp(float distance)
    {
        m_p += distance * m_up;
        m_at += distance * m_up;
        SetDirty(true);
    }
    
    RadeonRays::float3 Camera::GetForwardVector() const
    {
        return m_forward;
    }
    
    RadeonRays::float3 Camera::GetUpVector() const
    {
        return m_up;
    }
    
    RadeonRays::float3 Camera::GetRightVector() const
    {
        return m_right;
    }
    
    RadeonRays::float3 Camera::GetPosition() const
    {
        return m_p;
    }
    
    float Camera::GetAspectRatio() const
    {
        return m_dim.x / m_dim.y;
    }
    
    namespace {
        struct PerspectiveCameraConcrete : public PerspectiveCamera {
            PerspectiveCameraConcrete(float3 const& eye, float3 const& at, float3 const& up) :
            PerspectiveCamera(eye, at, up) {}
        };
        
        struct OrthographicCameraConcrete : public OrthographicCamera {
            OrthographicCameraConcrete(float3 const& eye, float3 const& at, float3 const& up) :
            OrthographicCamera(eye, at, up) {}
        };
    }
    
    PerspectiveCamera::Ptr PerspectiveCamera::Create(RadeonRays::float3 const& eye,
               RadeonRays::float3 const& at,
               RadeonRays::float3 const& up) {
        return std::make_shared<PerspectiveCameraConcrete>(eye, at, up);
        
    }
    
    OrthographicCamera::Ptr OrthographicCamera::Create(RadeonRays::float3 const& eye,
                                                     RadeonRays::float3 const& at,
                                                     RadeonRays::float3 const& up) {
        return std::make_shared<OrthographicCameraConcrete>(eye, at, up);
    }
}
