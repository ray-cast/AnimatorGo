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

/**
 \file camera.h
 \author Dmitry Kozlov
 \version 1.0
 \brief Contains declaration of camera types supported by the renderer.
 */
#pragma once

#include "math/float3.h"
#include "math/float2.h"

#include "scene_object.h"
#include "material.h"

namespace Baikal
{
    class Camera : public SceneObject
    {
    public:
        using Ptr = std::shared_ptr<Camera>;
        
        Camera() = default;
        virtual ~Camera() = 0;
        
        // Pass camera position, camera aim, camera up vector
        void LookAt(RadeonRays::float3 const& eye,
                    RadeonRays::float3 const& at,
                    RadeonRays::float3 const& up);
        
        // Rotate camera around world Z axis
        void Rotate(float angle);

        // Tilt camera
        void Tilt(float angle);
        // Move along camera Z direction
        void MoveForward(float distance);
        void Zoom(float distance);
        // Move along camera X direction
        void MoveRight(float distance);
        // Move along camera Y direction
        void MoveUp(float distance);
        
        RadeonRays::float3 GetForwardVector() const;
        RadeonRays::float3 GetUpVector() const;
        RadeonRays::float3 GetRightVector() const;
        RadeonRays::float3 GetPosition() const;
        
        // Set camera depth range.
        // Does not really make sence for physical camera
        void SetDepthRange(RadeonRays::float2 const& range);
        RadeonRays::float2 GetDepthRange() const;

        float GetAspectRatio() const;
        
        // Set camera sensor size in meters.
        // This distinguishes APC-S vs full-frame, etc
        void SetSensorSize(RadeonRays::float2 const& size);
        RadeonRays::float2 GetSensorSize() const;

        // Get/Set camera volume index
        void SetVolume(VolumeMaterial::Ptr shape);
        VolumeMaterial::Ptr GetVolume() const;

    protected:
        // Rotate camera around world Z axis
        void Rotate(RadeonRays::float3, float angle);
        void RotateOnOrbit(RadeonRays::float3, float angle);

        Camera(RadeonRays::float3 const& eye,
               RadeonRays::float3 const& at,
               RadeonRays::float3 const& up);
        
        // Camera coordinate frame
        RadeonRays::float3 m_forward;
        RadeonRays::float3 m_right;
        RadeonRays::float3 m_up;
        RadeonRays::float3 m_p;
        RadeonRays::float3 m_at;
        
        // Image plane width & hight in scene units
        RadeonRays::float2 m_dim;
        
        // Near and far Z
        RadeonRays::float2 m_zcap;

        // Volume index
        VolumeMaterial::Ptr m_volume;
    };
    
    class OrthographicCamera : public Camera
    {
    public:
        using Ptr = std::shared_ptr<OrthographicCamera>;
        
        static Ptr Create(RadeonRays::float3 const& eye,
                          RadeonRays::float3 const& at,
                          RadeonRays::float3 const& up);
        
        ~OrthographicCamera() = default;
        
        
    protected:
        OrthographicCamera(RadeonRays::float3 const& eye,
                           RadeonRays::float3 const& at,
                           RadeonRays::float3 const& up)
        : Camera(eye, at, up)
        {
        }
    };
    
    class PerspectiveCamera : public Camera
    {
    public:
        using Ptr = std::shared_ptr<PerspectiveCamera>;
        static Ptr Create(RadeonRays::float3 const& eye,
                          RadeonRays::float3 const& at,
                          RadeonRays::float3 const& up);
        
        
        // Set camera focus distance in meters,
        // this is essentially a distance from the lens to the focal plane.
        // Altering this is similar to rotating the focus ring on real lens.
        void SetFocusDistance(float distance);
        float GetFocusDistance() const;
        
        // Set camera focal length in meters,
        // this is essentially a distance between a camera sensor and a lens.
        // Altering this is similar to rotating zoom ring on a lens.
        void SetFocalLength(float length);
        float GetFocalLength() const;
        
        // Set aperture value in meters.
        // This is a radius of a lens.
        void SetAperture(float aperture);
        float GetAperture() const;
        
    protected:
        // Pass camera position, camera aim, camera up vector, depth limits, vertical field of view
        // and image plane aspect ratio
        PerspectiveCamera(RadeonRays::float3 const& eye,
                          RadeonRays::float3 const& at,
                          RadeonRays::float3 const& up);
        
    private:
        float  m_focal_length;
        float  m_focus_distance;
        float  m_aperture;
        
        friend std::ostream& operator << (std::ostream& o, PerspectiveCamera const& p);
    };
    
    inline Camera::~Camera()
    {
    }
    
    inline void PerspectiveCamera::SetFocusDistance(float distance)
    {
        m_focus_distance = distance;
        SetDirty(true);
    }
    
    inline void PerspectiveCamera::SetFocalLength(float length)
    {
        m_focal_length = length;
        SetDirty(true);
    }
    
    inline void PerspectiveCamera::SetAperture(float aperture)
    {
        m_aperture = aperture;
        SetDirty(true);
    }
    
    inline float PerspectiveCamera::GetFocusDistance() const
    {
        return m_focus_distance;
    }
    
    inline float PerspectiveCamera::GetFocalLength() const
    {
        return m_focal_length;
    }
    
    inline float PerspectiveCamera::GetAperture() const
    {
        return m_aperture;
    }
    
    inline RadeonRays::float2 Camera::GetSensorSize() const
    {
        return m_dim;
    }
    
    inline void Camera::SetSensorSize(RadeonRays::float2 const& size)
    {
        m_dim = size;
        SetDirty(true);
    }
    
    inline void Camera::SetDepthRange(RadeonRays::float2 const& range)
    {
        m_zcap = range;
        SetDirty(true);
    }
    
    inline RadeonRays::float2 Camera::GetDepthRange() const
    {
        return m_zcap;
    }

    inline void Camera::SetVolume(VolumeMaterial::Ptr volume)
    {
        m_volume = volume;
        SetDirty(true);
    }
    

    inline VolumeMaterial::Ptr Camera::GetVolume() const
    {
        return m_volume;
    }

}
