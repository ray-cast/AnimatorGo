#include "shape.h"
#include <cassert>

namespace Baikal
{
    Mesh::Mesh() :
    m_aabb_cached(false)
    {
    }
    
    void Mesh::SetIndices(std::uint32_t const* indices, std::size_t num_indices)
    {
        assert(indices);
        assert(num_indices != 0);
        
        // Resize internal array and copy data
        m_indices.resize(num_indices);
        
        std::copy(indices, indices + num_indices, &m_indices[0]);
        
        SetDirty(true);
    }

    void Mesh::SetIndices(std::vector<std::uint32_t>&& indices)
    {
        m_indices = std::move(indices);
    }

    std::size_t Mesh::GetNumIndices() const
    {
        return m_indices.size();
        
    }
    std::uint32_t const* Mesh::GetIndices() const
    {
        return &m_indices[0];
    }
    
    void Mesh::SetVertices(RadeonRays::float3 const* vertices, std::size_t num_vertices)
    {
        assert(vertices);
        assert(num_vertices != 0);
        
        // Resize internal array and copy data
        m_vertices.resize(num_vertices);

        std::copy(vertices, vertices + num_vertices, &m_vertices[0]);

        SetDirty(true);
    }
    
    void Mesh::SetVertices(float const* vertices, std::size_t num_vertices)
    {
        assert(vertices);
        assert(num_vertices != 0);
        
        // Resize internal array and copy data
        m_vertices.resize(num_vertices);
        
        for (std::size_t i = 0; i < num_vertices; ++i)
        {
            m_vertices[i].x = vertices[3 * i];
            m_vertices[i].y = vertices[3 * i + 1];
            m_vertices[i].z = vertices[3 * i + 2];
            m_vertices[i].w = 1;
        }

        SetDirty(true);
    }

    void Mesh::SetVertices(std::vector<RadeonRays::float3>&& vertices)
    {
        m_vertices = std::move(vertices);
    }

    
    std::size_t Mesh::GetNumVertices() const
    {
        return m_vertices.size();
    }
    
    RadeonRays::float3 const* Mesh::GetVertices() const
    {
        return &m_vertices[0];
    }
    
    void Mesh::SetNormals(RadeonRays::float3 const* normals, std::size_t num_normals)
    {
        assert(normals);
        assert(num_normals != 0);
        
        // Resize internal array and copy data
        m_normals.resize(num_normals);

        std::copy(normals, normals + num_normals, &m_normals[0]);

        SetDirty(true);
    }
    
    void Mesh::SetNormals(float const* normals, std::size_t num_normals)
    {
        assert(normals);
        assert(num_normals != 0);
        
        // Resize internal array and copy data
        m_normals.resize(num_normals);
        
        for (std::size_t i = 0; i < num_normals; ++i)
        {
            m_normals[i].x = normals[3 * i];
            m_normals[i].y = normals[3 * i + 1];
            m_normals[i].z = normals[3 * i + 2];
            m_normals[i].w = 0;
        }

        SetDirty(true);
    }

    void Mesh::SetNormals(std::vector<RadeonRays::float3>&& normals)
    {
        m_normals = std::move(normals);
    }

    
    std::size_t Mesh::GetNumNormals() const
    {
        return m_normals.size();
    }

    RadeonRays::float3 const* Mesh::GetNormals() const
    {
        return &m_normals[0];
    }

    void Mesh::SetUVs(RadeonRays::float2 const* uvs, std::size_t num_uvs)
    {
        assert(uvs);
        assert(num_uvs != 0);
        
        // Resize internal array and copy data
        m_uvs.resize(num_uvs);

        std::copy(uvs, uvs + num_uvs, &m_uvs[0]);

        SetDirty(true);
    }
    
    void Mesh::SetUVs(float const* uvs, std::size_t num_uvs)
    {
        assert(uvs);
        assert(num_uvs != 0);
        
        // Resize internal array and copy data
        m_uvs.resize(num_uvs);
        
        for (std::size_t i = 0; i < num_uvs; ++i)
        {
            m_uvs[i].x = uvs[2 * i];
            m_uvs[i].y = uvs[2 * i + 1];
        }

        SetDirty(true);
    }

    void Mesh::SetUVs(std::vector<RadeonRays::float2>&& uvs)
    {
        m_uvs = std::move(uvs);
    }

    std::size_t Mesh::GetNumUVs() const
    {
        return m_uvs.size();
    }
    
    RadeonRays::float2 const* Mesh::GetUVs() const
    {
        return &m_uvs[0];
    }

    RadeonRays::bbox Shape::GetWorldAABB() const
    {
        RadeonRays::bbox result;
        auto local_aabb = GetLocalAABB();
        auto transform = GetTransform();

        auto p0 = local_aabb.pmin;
        auto p1 = local_aabb.pmax;
        auto p2 = RadeonRays::float3(p0.x, p0.y, p1.z);
        auto p3 = RadeonRays::float3(p0.x, p1.y, p0.z);
        auto p4 = RadeonRays::float3(p1.x, p0.y, p0.z);
        auto p5 = RadeonRays::float3(p0.x, p1.y, p1.z);
        auto p6 = RadeonRays::float3(p1.x, p1.y, p0.z);
        auto p7 = RadeonRays::float3(p1.x, p0.y, p1.z);

        result.grow(transform * p0);
        result.grow(transform * p1);
        result.grow(transform * p2);
        result.grow(transform * p3);
        result.grow(transform * p4);
        result.grow(transform * p5);
        result.grow(transform * p6);
        result.grow(transform * p7);

        return result;
    }

    RadeonRays::bbox Mesh::GetLocalAABB() const
    {
        if (!m_aabb_cached)
        {
            m_aabb = RadeonRays::bbox();
            for (std::size_t i = 0; i < m_indices.size(); ++i)
            {
                m_aabb.grow(m_vertices[m_indices[i]]);
            }
            m_aabb_cached = true;
        }

        return m_aabb;
    }

    void Mesh::SetDirty(bool dirty) const
    {
        Shape::SetDirty(dirty);
        m_aabb_cached = false;
    }

    RadeonRays::bbox Instance::GetLocalAABB() const
    {
        return m_base_shape->GetLocalAABB();
    }
    
    namespace {
        struct InstanceConcrete : public Instance {
            InstanceConcrete(Shape::Ptr base_shape) :
            Instance(base_shape) {}
        };
        
        struct MeshConcrete : public Mesh {
        };
    }
    
    Mesh::Ptr Mesh::Create() {
        return std::make_shared<MeshConcrete>();
    }
    
    Instance::Ptr Instance::Create(Shape::Ptr base_shape) {
        return std::make_shared<InstanceConcrete>(base_shape);
    }
}
