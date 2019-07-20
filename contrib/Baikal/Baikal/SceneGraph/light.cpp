#include "light.h"
#include "SceneGraph/scene1.h"
#include "SceneGraph/texture.h"

namespace Baikal
{
    AreaLight::AreaLight(Shape::Ptr shape, std::size_t idx)
        : m_shape(shape)
        , m_prim_idx(idx)
    {
    }

    RadeonRays::float3 Light::GetPosition() const
    {
        return m_p;
    }

    void Light::SetPosition(RadeonRays::float3 const& p)
    {
        m_p = p;
        SetDirty(true);
    }

    RadeonRays::float3 Light::GetDirection() const
    {
        return m_d;
    }

    void Light::SetDirection(RadeonRays::float3 const& d)
    {
        m_d = normalize(d);
        SetDirty(true);
    }

    std::unique_ptr<Iterator> Light::CreateTextureIterator() const
    {
        return std::make_unique<EmptyIterator>();
    }

    RadeonRays::float3 Light::GetEmittedRadiance() const
    {
        return m_e;
    }

    void Light::SetEmittedRadiance(RadeonRays::float3 const& e)
    {
        m_e = e;
        SetDirty(true);
    }

    void SpotLight::SetConeShape(RadeonRays::float2 angles)
    {
        m_angles = angles;
        SetDirty(true);
    }

    RadeonRays::float2 SpotLight::GetConeShape() const
    {
        return m_angles;
    }

    void ImageBasedLight::SetTexture(Texture::Ptr texture)
    {
        m_texture = texture;
        SetDirty(true);
    }

    Texture::Ptr ImageBasedLight::GetTexture() const
    {
        return m_texture;
    }

    std::size_t AreaLight::GetPrimitiveIdx() const
    {
        return m_prim_idx;
    }

    Shape::Ptr AreaLight::GetShape() const
    {
        return m_shape;
    }

    ImageBasedLight::ImageBasedLight()
        : m_texture(nullptr)
        , m_reflection_texture(nullptr)
        , m_refraction_texture(nullptr)
        , m_transparency_texture(nullptr)
        , m_background_texture(nullptr)
        , m_multiplier(1.f)
        , mirror_x_(false)
    {
    }

    float ImageBasedLight::GetMultiplier() const
    {
        return m_multiplier;
    }

    void ImageBasedLight::SetMultiplier(float m)
    {
        m_multiplier = m;
        SetDirty(true);
    }


    void ImageBasedLight::SetReflectionTexture(Texture::Ptr texture)
    {
        m_reflection_texture = texture;
        SetDirty(true);
    }

    Texture::Ptr ImageBasedLight::GetReflectionTexture() const
    {
        return m_reflection_texture;
    }

    void ImageBasedLight::SetRefractionTexture(Texture::Ptr texture)
    {
        m_refraction_texture = texture;
        SetDirty(true);
    }

    Texture::Ptr ImageBasedLight::GetRefractionTexture() const
    {
        return m_refraction_texture;
    }

    void ImageBasedLight::SetTransparencyTexture(Texture::Ptr texture)
    {
        m_transparency_texture = texture;
        SetDirty(true);
    }

    Texture::Ptr ImageBasedLight::GetTransparencyTexture() const
    {
        return m_transparency_texture;
    }

    void ImageBasedLight::SetBackgroundTexture(Texture::Ptr texture)
    {
        m_background_texture = texture;
        SetDirty(true);
    }

    Texture::Ptr ImageBasedLight::GetBackgroundTexture() const
    {
        return m_background_texture;
    }

    std::unique_ptr<Iterator> ImageBasedLight::CreateTextureIterator() const
    {
        std::set<Texture::Ptr> result;

        if (m_texture)
        {
            result.insert(m_texture);
        }

        if (m_reflection_texture)
        {
            result.insert(m_reflection_texture);
        }

        if (m_refraction_texture)
        {
            result.insert(m_refraction_texture);
        }

        if (m_transparency_texture)
        {
            result.insert(m_transparency_texture);
        }

        if (m_background_texture)
        {
            result.insert(m_background_texture);
        }

        return std::make_unique<ContainerIterator<std::set<Texture::Ptr>>>(std::move(result));
    }


    RadeonRays::float3 PointLight::GetPower(Scene1 const& scene) const
    {
        return 4.f * PI * GetEmittedRadiance();
    }

    RadeonRays::float3 SpotLight::GetPower(Scene1 const& scene) const
    {
        auto cone = GetConeShape();
        return 2.f * PI * GetEmittedRadiance() * (1.f - 0.5f * (cone.x + cone.y));
    }

    RadeonRays::float3 DirectionalLight::GetPower(Scene1 const& scene) const
    {
        auto scene_radius = scene.GetRadius();
        return PI * GetEmittedRadiance() * scene_radius * scene_radius;
    }

    RadeonRays::float3 ImageBasedLight::GetPower(Scene1 const& scene) const
    {
        auto scene_radius = scene.GetRadius();
        auto avg = RadeonRays::float3();
        auto cnt = 0u;
        if (m_texture)
        {
            avg += m_texture->ComputeAverageValue();
            ++cnt;
        }

        if (m_reflection_texture)
        {
            avg += m_reflection_texture->ComputeAverageValue();
            ++cnt;
        }

        if (m_refraction_texture)
        {
            avg += m_refraction_texture->ComputeAverageValue();
            ++cnt;
        }

        if (m_transparency_texture)
        {
            avg += m_transparency_texture->ComputeAverageValue();
            ++cnt;
        }

        // We do not count background texture here since it is only for missing primary rays
        return PI * avg * (1.f / cnt) * scene_radius * scene_radius;
    }

    void ImageBasedLight::SetMirrorX(bool mirror_x)
    {
        mirror_x_ = mirror_x;
    }

    bool ImageBasedLight::GetMirrorX() const
    {
        return mirror_x_;
    }


    RadeonRays::float3 AreaLight::GetPower(Scene1 const& scene) const
    {
        auto mesh = std::static_pointer_cast<Mesh>(m_shape);
        auto indices = mesh->GetIndices();
        auto vertices = mesh->GetVertices();

        auto i0 = indices[m_prim_idx * 3];
        auto i1 = indices[m_prim_idx * 3 + 1];
        auto i2 = indices[m_prim_idx * 3 + 2];

        auto v0 = vertices[i0];
        auto v1 = vertices[i1];
        auto v2 = vertices[i2];

        float area = 0.5f * std::sqrt(cross(v2 - v0, v1 - v0).sqnorm());
        return PI * GetEmittedRadiance() * area;
    }
    
    namespace {
        struct PointLightConcrete : public PointLight {
        };
        struct DirectionalLightConcrete : public DirectionalLight {
        };
        struct SpotLightConcrete : public SpotLight {
        };
        struct ImageBasedLightConcrete: public ImageBasedLight {
        };
        struct AreaLightConcrete: public AreaLight {
            AreaLightConcrete(Shape::Ptr shape, std::size_t idx) :
            AreaLight(shape, idx) {}
        };
    }
    
    PointLight::Ptr PointLight::Create() {
        return std::make_shared<PointLightConcrete>();
    }
    
    DirectionalLight::Ptr DirectionalLight::Create() {
        return std::make_shared<DirectionalLightConcrete>();
    }
    
    SpotLight::Ptr SpotLight::Create() {
        return std::make_shared<SpotLightConcrete>();
    }
    
    ImageBasedLight::Ptr ImageBasedLight::Create() {
        return std::make_shared<ImageBasedLightConcrete>();
    }
    
    AreaLight::Ptr AreaLight::Create(Shape::Ptr shape, std::size_t idx) {
        return std::make_shared<AreaLightConcrete>(shape, idx);
    }
}
