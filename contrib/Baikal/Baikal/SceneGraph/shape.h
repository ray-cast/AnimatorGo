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
 \file shape.h
 \author Dmitry Kozlov
 \version 1.0
 \brief Contains declaration of various shape types supported by the renderer.
 */
#pragma once

#include "math/float3.h"
#include "math/float2.h"
#include "math/matrix.h"
#include "math/bbox.h"
#include <memory>
#include <string>
#include <vector>

#include "scene_object.h"
#include "material.h"

namespace Baikal
{
    class Material;
    
    /**
     \brief Shape base interface.
     
     High-level interface all shape classes need to implement.
     */
    class Shape : public SceneObject
    {
    public:
        using Ptr = std::shared_ptr<Shape>;
        
        struct Visibility
        {
            enum VisibilityFlags
            {
                kPrimary = 0x1,
                kShadow = (0x1 << 15)
            };
            
            static VisibilityFlags VisibleForAll() { return (VisibilityFlags)0xffffffffu; }
            static VisibilityFlags InvisibleForAll() { return (VisibilityFlags)0x0u; }
            static VisibilityFlags VisibleForPrimary() { return kPrimary; }
            static VisibilityFlags VisibleForPrimaryShadow() { return kShadow; }
            static VisibilityFlags VisibleForBounce(int i) { return (VisibilityFlags)(kPrimary << i); }
            static VisibilityFlags VisibleForBounceShadow(int i) { return (VisibilityFlags)(kShadow << i); }
        };
        
        // Destructor
        virtual ~Shape() = 0;

        // Set and get group id
        void SetGroupId(std::uint32_t id);
        std::uint32_t GetGroupId() const;

        // Get and set material
        void SetMaterial(Material::Ptr material);
        Material::Ptr GetMaterial() const;

        // Get and set volume material
        void SetVolumeMaterial(VolumeMaterial::Ptr volume_mat);
        VolumeMaterial::Ptr GetVolumeMaterial() const;

        // Get and set transform
        void SetTransform(RadeonRays::matrix const& t);
        RadeonRays::matrix GetTransform() const;

        // Set whether a shape casts shadow or not
        void SetShadow(bool shadow);
        bool GetShadow() const;
        
        // Set visibility properties for a shape
        void SetVisibilityMask(std::uint32_t mask);
        std::uint32_t GetVisibilityMask() const;

        // Local AABB
        virtual RadeonRays::bbox GetLocalAABB() const = 0;
        RadeonRays::bbox GetWorldAABB() const;

        // Forbidden stuff
        Shape(Shape const&) = delete;
        Shape& operator = (Shape const&) = delete;
        
    protected:
        // Constructor
        Shape();
        
    private:
        // Material for the shape
        Material::Ptr m_material;
        // Volume material for the shape
        VolumeMaterial::Ptr m_volume;
        // Transform
        RadeonRays::matrix m_transform;
        // Visibility mask
        std::uint32_t m_visibility_mask;
        // Group id
        std::uint32_t m_group_id;
    };
    
    /**
     \brief Triangle mesh class.
     
     Triangle mesh is a collection of indexed triangle.
     */
    class Mesh : public Shape
    {
    public:
        using Ptr = std::shared_ptr<Mesh>;
        static Ptr Create();

        // Set and get index array
        void SetIndices(std::uint32_t const* indices, std::size_t num_indices);
        void SetIndices(std::vector<std::uint32_t>&& indices);
        std::size_t GetNumIndices() const;
        std::uint32_t const* GetIndices() const;

        // Set and get vertex array
        void SetVertices(RadeonRays::float3 const* vertices, std::size_t num_vertices);
        void SetVertices(float const* vertices, std::size_t num_vertices);
        void SetVertices(std::vector<RadeonRays::float3>&& vertices);

        std::size_t GetNumVertices() const;
        RadeonRays::float3 const* GetVertices() const;

        // Set and get normal array
        void SetNormals(RadeonRays::float3 const* normals, std::size_t num_normals);
        void SetNormals(float const* normals, std::size_t num_normals);
        void SetNormals(std::vector<RadeonRays::float3>&& normals);

        std::size_t GetNumNormals() const;
        RadeonRays::float3 const* GetNormals() const;
        
        // Set and get UV array
        void SetUVs(RadeonRays::float2 const* uvs, std::size_t num_uvs);
        void SetUVs(float const* uvs, std::size_t num_uvs);
        void SetUVs(std::vector<RadeonRays::float2>&& uvs);
        std::size_t GetNumUVs() const;
        RadeonRays::float2 const* GetUVs() const;

        // Local space AABB
        RadeonRays::bbox GetLocalAABB() const override;

        // We need to override it since mesh changes trigger
        // m_aabb_cached flag reset
        void SetDirty(bool dirty) const override;

        // Forbidden stuff
        Mesh(Mesh const&) = delete;
        Mesh& operator = (Mesh const&) = delete;
        
    protected:
        Mesh();
        
    private:
        std::vector<RadeonRays::float3> m_vertices;
        std::vector<RadeonRays::float3> m_normals;
        std::vector<RadeonRays::float2> m_uvs;
        std::vector<std::uint32_t> m_indices;

        mutable RadeonRays::bbox m_aabb;
        mutable bool m_aabb_cached;
    };
    
    inline Shape::~Shape()
    {
    }
    
    inline Shape::Shape() 
        : m_material(nullptr)
        , m_volume(nullptr)
        , m_visibility_mask(0xffffffffu)
        , m_group_id(-1)
    {
    }
    
    inline void Shape::SetGroupId(std::uint32_t id)
    {
        m_group_id = id;
        SetDirty(true);
    }

    inline std::uint32_t Shape::GetGroupId() const
    {
        return m_group_id;
    }

    inline void Shape::SetMaterial(Material::Ptr material)
    {
        m_material = material;
        SetDirty(true);
    }
    
    inline Material::Ptr Shape::GetMaterial() const
    {
        return m_material;
    }

    inline void Shape::SetVolumeMaterial(VolumeMaterial::Ptr volume_mat)
    {
        m_volume = volume_mat;
        SetDirty(true);
    }

    inline VolumeMaterial::Ptr Shape::GetVolumeMaterial() const
    {
        return m_volume;
    }

    inline void Shape::SetTransform(RadeonRays::matrix const& t)
    {
        m_transform = t;
        SetDirty(true);
    }

    inline RadeonRays::matrix Shape::GetTransform() const
    {
        return m_transform;
    }

	inline void Shape::SetShadow(bool shadow)
	{
	}

	inline bool Shape::GetShadow() const
	{
		return true;
	}

    inline void Shape::SetVisibilityMask(std::uint32_t mask)
    {
        m_visibility_mask = mask;
        SetDirty(true);
    }
    
    inline std::uint32_t Shape::GetVisibilityMask() const
    {
        return m_visibility_mask;
    }

    /**
    \brief Instance class.

    Instance references some mesh, but might have different transform and material.
    */
    class Instance : public Shape
    {
    public:
        using Ptr = std::shared_ptr<Instance>;
        static Ptr Create(Shape::Ptr base_shape);

        // Get and set base shape
        void SetBaseShape(Shape::Ptr base_shape);
        Shape::Ptr GetBaseShape() const;

        // Local space AABB
        RadeonRays::bbox GetLocalAABB() const override;

        // Forbidden stuff
        Instance(Instance const&) = delete;
        Instance& operator = (Instance const&) = delete;

    protected:
        Instance(Shape::Ptr base_shape = nullptr);
        
    private:
        Shape::Ptr m_base_shape;
    };

    inline Instance::Instance(Shape::Ptr base_shape)
        : m_base_shape(base_shape)
    {
    }

    inline void Instance::SetBaseShape(Shape::Ptr base_shape)
    {
        m_base_shape = base_shape;
        SetDirty(true);
    }

    inline Shape::Ptr Instance::GetBaseShape() const
    {
        return m_base_shape;
    }
}

