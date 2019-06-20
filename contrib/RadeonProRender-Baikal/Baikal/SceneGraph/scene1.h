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
 \file scene.h
 \author Dmitry Kozlov
 \version 1.0
 \brief Contains declaration of Baikal::Scene1 class, core interface representing representing the scene.
 */
#pragma once

#include <memory>
#include "math/bbox.h"

#include "light.h"
#include "shape.h"
#include "camera.h"

namespace Baikal
{
    class Shape;
    class Volume;
    class Camera;
    class Iterator;
    class SceneObject;
    
    /**
     \brief Scene class.
     
     Scene represents a collection of objects such as ligths, meshes, volumes, etc. It also has a functionality
     to add, remove and change these objects.
     */
    class Scene1
    {
    public:
        using Ptr = std::shared_ptr<Scene1>;
        static Ptr Create();
        
        // Dirty flags are used to perform partial buffer updates to save traffic
        using DirtyFlags = std::uint32_t;
        enum
        {
            kNone,
            kLights,
            kShapes,
            kShapeTransforms,
            kCamera,
            kBackground
        };

        struct EnvironmentOverride
        {
            ImageBasedLight::Ptr m_reflection;
            ImageBasedLight::Ptr m_refraction;
            ImageBasedLight::Ptr m_transparency;
            ImageBasedLight::Ptr m_background;
        };
      
        // Destructor
        virtual ~Scene1();

        // Add or remove lights
        void AttachLight(Light::Ptr light);
        void DetachLight(Light::Ptr light);
        
        // Get the number of lights in the scene
        std::size_t GetNumLights() const;
        // Get light iterator
        std::unique_ptr<Iterator> CreateLightIterator() const;
        
        // Add or remove shapes
        void AttachShape(Shape::Ptr shape);
        void DetachShape(Shape::Ptr shape);
        
        // Get number of shapes in the scene
        std::size_t GetNumShapes() const;
        // Get shape iterator
        std::unique_ptr<Iterator> CreateShapeIterator() const;

        // Set and get camera
        void SetCamera(Camera::Ptr camera);
        Camera::Ptr GetCamera() const;

        // Get state change since last clear
        DirtyFlags GetDirtyFlags() const;
        // Set specified flag in dirty state
        void SetDirtyFlag(DirtyFlags flag) const;
        // Clear all flags
        void ClearDirtyFlags() const;

        // Check if the scene is ready for rendering
        bool IsValid() const;

        // World space AABB
        RadeonRays::bbox GetWorldAABB() const;

        // World space bounding sphere radius
        float GetRadius() const;

        // Background image override
        void SetBackgroundImage(Baikal::Texture::Ptr texture);
        Baikal::Texture::Ptr GetBackgroundImage() const;

        void SetEnvironmentOverride(const EnvironmentOverride& env_override);
        const EnvironmentOverride& GetEnvironmentOverride() const;
        
        // Forbidden stuff
        Scene1(Scene1 const&) = delete;
        Scene1& operator = (Scene1 const&) = delete;

        // Acquire or release scene when scene controller compiles it
        void Acquire(std::uint32_t controller_id);
        void Release();
    
    protected:
        // Constructor
        Scene1();
        
    private:
        struct SceneImpl;
        std::unique_ptr<SceneImpl> m_impl;
    };
    
}
