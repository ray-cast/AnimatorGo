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
 \file light.h
 \author Dmitry Kozlov
 \version 1.0
 \brief Contains declaration of various light types supported by the renderer.
 */
#pragma once

#include "math/float3.h"
#include "math/float2.h"
#include "math/mathutils.h"
#include <memory>
#include <string>
#include <set>

#include "iterator.h"

#include "scene_object.h"
#include "shape.h"
#include "texture.h"

namespace Baikal
{
    class Scene1;

    /**
     \brief Light base interface.
     
     High-level interface all light classes need to implement.
     */
    class Light : public SceneObject
    {
    public:
        using Ptr = std::shared_ptr<Light>;
        
        // Destructor
        virtual ~Light() = 0;

        // Get total radiant power (integral)
        //virtual RadeonRays::float3 GetRadiantPower() const = 0;

        // Set and get position
        RadeonRays::float3 GetPosition() const;
        void SetPosition(RadeonRays::float3 const& p);

        // Set and get direction
        RadeonRays::float3 GetDirection() const;
        void SetDirection(RadeonRays::float3 const& d);

        // Set and get emitted radiance (differential)
        RadeonRays::float3 GetEmittedRadiance() const;
        void SetEmittedRadiance(RadeonRays::float3 const& e);

        // Iterator for all the textures used by the light
        virtual std::unique_ptr<Iterator> CreateTextureIterator() const;

        virtual RadeonRays::float3 GetPower(Scene1 const& scene) const = 0;
        
    protected:
        // Constructor
        Light();
        
    private:
        // Position
        RadeonRays::float3 m_p;
        // Direction
        RadeonRays::float3 m_d;
        // Emmited radiance
        RadeonRays::float3 m_e;
    };
    
    inline Light::Light()
    : m_d(0.f, -1.f, 0.f)
    , m_e(1.f, 1.f, 1.f)
    {
    }
    
    inline Light::~Light()
    {
    }
    
    /**
     \brief Simple point light source.
     
     Non-physical light emitting power from a single point in all directions.
     */
    class PointLight: public Light
    {
    public:
        using Ptr = std::shared_ptr<PointLight>;
        static Ptr Create();
        
        RadeonRays::float3 GetPower(Scene1 const& scene) const override;

    protected:
        PointLight(){}

    };
    
    /**
     \brief Simple directional light source.
     
     Non-physical light emitting power from a single direction.
     */
    class DirectionalLight: public Light
    {
    public:
        using Ptr = std::shared_ptr<DirectionalLight>;
        static Ptr Create();
        
        RadeonRays::float3 GetPower(Scene1 const& scene) const override;
        
    protected:
        DirectionalLight(){}
    };
    
    /**
     \brief Simple cone-shaped light source.
     
     Non-physical light emitting power from a single point in a cone of directions.
     */
    class SpotLight: public Light
    {
    public:
        using Ptr = std::shared_ptr<SpotLight>;
        static Ptr Create();
        
        // Get and set inner and outer falloff angles: they are set as cosines of angles between light direction
        // and cone opening.
        void SetConeShape(RadeonRays::float2 angles);
        RadeonRays::float2 GetConeShape() const;

        RadeonRays::float3 GetPower(Scene1 const& scene) const override;
        
    protected:
        SpotLight();
        
    private:
        // Opening angles (x - inner, y - outer)
        RadeonRays::float2 m_angles;
    };
    
    inline SpotLight::SpotLight()
    : m_angles(0.25f, 0.5f)
    {
    }
    
    /**
     \brief Image-based distant light source.
     
     Physical light emitting from all spherical directions.
     */
    class ImageBasedLight: public Light
    {
    public:
        using Ptr = std::shared_ptr<ImageBasedLight>;
        static Ptr Create();

        // Get and set illuminant texture
        void SetTexture(Texture::Ptr texture);
        Texture::Ptr GetTexture() const;

        void SetReflectionTexture(Texture::Ptr texture);
        Texture::Ptr GetReflectionTexture() const;

        void SetRefractionTexture(Texture::Ptr texture);
        Texture::Ptr GetRefractionTexture() const;

        void SetTransparencyTexture(Texture::Ptr texture);
        Texture::Ptr GetTransparencyTexture() const;

        void SetBackgroundTexture(Texture::Ptr texture);
        Texture::Ptr GetBackgroundTexture() const;

        // Get and set multiplier.
        // Multiplier is used to adjust emissive power.
        float GetMultiplier() const;
        void SetMultiplier(float m);

        // Iterator for all the textures used by the light
        std::unique_ptr<Iterator> CreateTextureIterator() const override;
        
        RadeonRays::float3 GetPower(Scene1 const& scene) const override;

        // Get and set mirror status for texture around Y axis. (switch X axis direction)
        void SetMirrorX(bool mirror_x);
        bool GetMirrorX() const;
    protected:
        ImageBasedLight();

    private:
        // Illuminant texture
        Texture::Ptr m_texture;
        Texture::Ptr m_reflection_texture;
        Texture::Ptr m_refraction_texture;
        Texture::Ptr m_transparency_texture;
        Texture::Ptr m_background_texture;
        // Emissive multiplier
        float m_multiplier;
        bool mirror_x_;
    };
    
    // Area light
    class AreaLight: public Light
    {
    public:
        using Ptr = std::shared_ptr<AreaLight>;
        static Ptr Create(Shape::Ptr shape, std::size_t idx);
        
        // Get parent shape
        Shape::Ptr GetShape() const;
        // Get parent prim idx
        std::size_t GetPrimitiveIdx() const;

        RadeonRays::float3 GetPower(Scene1 const& scene) const override;

    protected:
        AreaLight(Shape::Ptr shape, std::size_t idx);

    private:
        // Parent shape
        Shape::Ptr m_shape;
        // Parent primitive index
        std::size_t m_prim_idx;
    };
}
