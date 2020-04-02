#include <octoon/lightmap/lightmap.h>
#include <octoon/lightmap/lightmap_pack.h>

namespace octoon::bake
{
    float orient2d(const math::float2& e1, const math::float2& e2, const math::float2& p)
    {
        auto u = e2 - e1;
        auto v = p - e1;
        return u.x * v.y - u.y * v.x;
    }

    bool isRightOf(const math::float2& e1, const math::float2& e2, const math::float2& p)
    {
        return orient2d(e1, e2, p) < -math::EPSILON_E5;
    }

    static constexpr auto FloatEpsilon = 0.0001f;

    inline bool closeTo(float a, float b)
    {
        auto delta = b - a;
        return -FloatEpsilon <= delta && delta <= FloatEpsilon;
    }

    inline bool closeTo(const math::float3& a, const math::float3& b)
    {
        return closeTo(a.x, b.x) && closeTo(a.y, b.y) && closeTo(a.z, b.z);
    }

    static math::float2 barycentric(math::float2 p1, math::float2 p2, math::float2 p3, math::float2 p)
    {
        // http://www.blackpawn.com/texts/pointinpoly/
        // Compute vectors
        math::float2 v0 = p3 - p1;
        math::float2 v1 = p2 - p1;
        math::float2 v2 = p - p1;

        // Compute dot products
        float dot00 = math::dot(v0, v0);
        float dot01 = math::dot(v0, v1);
        float dot02 = math::dot(v0, v2);
        float dot11 = math::dot(v1, v1);
        float dot12 = math::dot(v1, v2);

        // Compute barycentric coordinates
        float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
        return math::float2(u, v);
    }


	Lightmap::Lightmap() noexcept
        : width_(100)
        , height_(100)
	{
	}

	Lightmap::~Lightmap() noexcept
	{
	}

    std::uint32_t
    Lightmap::width() const noexcept
    {
        return this->width_;
    }

    std::uint32_t
    Lightmap::height() const noexcept
    {
        return this->height_;
    }

    const std::vector<math::float3>&
    Lightmap::fronBuffer() const noexcept
    {
        return this->colorBuffer_;
    }

    void
    Lightmap::render()
    {
        this->computeIndirectLightBounce();

        for (std::size_t i = 0; i < this->indirectLightBuffer_.size(); i++)
            this->colorBuffer_[i] = this->indirectLightBuffer_[i];
    }

    void
    Lightmap::renderLight(const light::DirectionalLight& light)
    {
        this->computeDirectLight(light);
    }

    void
    Lightmap::renderLight(const light::EnvironmentLight& light)
    {
        for (auto& patch : this->patches_)
        {
            this->directLightBuffer_[patch.texelIndex] += light.getColor() * light.getIntensity();
        }
    }

    void
    Lightmap::computeRadiosityFactors()
    {
        viewFactors_.resize(patches_.size() * patches_.size(), 0.0f);
        auto columns = patches_.size();

#		pragma omp parallel for
        for (std::int32_t i = 0; i < patches_.size(); ++i)
        {
            auto& sourcePatch = patches_[i];

            for (std::size_t j = 0; j < patches_.size(); ++j)
            {
                if (i == j)
                    continue;

                auto& destPatch = patches_[j];
                if (!closeTo(destPatch.position, sourcePatch.position))
                {
                    auto L = math::normalize(destPatch.position - sourcePatch.position);
                    auto destPatchVisibilityFactor = math::dot(destPatch.normal, -L);
                    auto sourcePatchVisibilityFactor = math::dot(sourcePatch.normal, L);
                    if (sourcePatchVisibilityFactor > 0 && destPatchVisibilityFactor > 0)
                    {
                        math::Raycast ray(patches_[i].position + patches_[i].normal * 0.01f, destPatch.position);
                        mesh::RaycastHit hit;
                        if (!this->mesh_->raycast(ray, hit))
                            viewFactors_[i * columns + j] = destPatchVisibilityFactor * sourcePatchVisibilityFactor;
                    }
                }
            }
        }

        size_t rowStart = 0;
        viewFactorsDen_.resize(patches_.size());
        for (size_t i = 0; i < patches_.size(); ++i, rowStart += columns)
        {
            float viewFactorSum = 0;
            for (size_t j = 0; j < columns; ++j)
                viewFactorSum += viewFactors_[rowStart + j];
            viewFactorsDen_[i] = 1.0f / viewFactorSum;
        }
    }

    void
    Lightmap::computeDirectLight(const light::DirectionalLight& light)
    {
        std::memset(this->directLightBuffer_.data(), this->directLightBuffer_.size() * sizeof(math::float3), 0);

        for (auto& patch : this->patches_)
        {
            auto lightDir = math::normalize(light.getForward());
            auto L = lightDir;

            math::Raycast ray;
            ray.origin = patch.position + lightDir * 0.01f;
            ray.normal = lightDir;

            auto nl = std::max(math::dot(L, patch.normal), 0.0f);
            if (nl > 0.0f)
            {
                mesh::RaycastHit hit;
                if (!this->mesh_->raycast(ray, hit))
                {
                    auto lightColor = light.getColor() * light.getIntensity() * nl * math::PI * 4.0f;
                    this->directLightBuffer_[patch.texelIndex] += lightColor * patch.color;
                }
            }
        }
    }

    void
    Lightmap::computeIndirectLightBounce()
    {
        auto columns = patches_.size();

        for (std::int32_t i = 0; i < columns; ++i)
        {
            auto& sourcePatch = this->patches_[i];
            auto& dest = this->indirectLightBuffer_[sourcePatch.texelIndex];

            math::float3 value = math::float3::Zero;
            for (std::size_t j = 0; j < columns; ++j)
            {
                auto factor = viewFactors_[i * columns + j];
                if (factor > 0)
                {
                    auto& destPatch = this->patches_[j];
                    auto& indirect = this->indirectLightBuffer_[destPatch.texelIndex];
                    auto& direct = this->directLightBuffer_[destPatch.texelIndex];

                    value += (indirect + direct) * viewFactorsDen_[i] * factor / math::sqrDistance(destPatch.position, sourcePatch.position) * math::PI_2;
                }
            }

            dest = sourcePatch.color * value;
        }
    }

    void
    Lightmap::build(const geometry::Geometry& geometry) noexcept
    {
        mesh_ = geometry.getMesh();

        auto& vertices = mesh_->getVertexArray();
        auto& normals = mesh_->getNormalArray();
        auto& texcoords = mesh_->getTexcoordArray(1);

        if (!texcoords.empty())
        {
            this->usedTexels.resize(this->width_ * this->height_);
            this->directLightBuffer_.resize(this->width_ * this->height_);
            this->indirectLightBuffer_.resize(this->width_ * this->height_);
            this->colorBuffer_.resize(this->width_ * this->height_);

            for (std::size_t i = 0; i < mesh_->getNumSubsets(); i++)
            {
                auto& indices = mesh_->getIndicesArray(i);
                auto& material = geometry.getMaterial(i);

                for (std::size_t i = 0; i < indices.size(); i += 3)
                {
                    auto i1 = indices[i];
                    auto i2 = indices[i + 1];
                    auto i3 = indices[i + 2];

                    auto& v1 = vertices[i1];
                    auto& v2 = vertices[i2];
                    auto& v3 = vertices[i3];

                    math::float3 diffuse;
                    if (material->get("diffuse", diffuse))
                    {
                        this->buildPatches(i / 3,
                            v1, v2, v3,
                            texcoords[i1], texcoords[i2], texcoords[i3],
                            normals[i1], normals[i2], normals[i3], diffuse);
                    }
                }
            }
        }

        this->computeRadiosityFactors();
    }

    void
    Lightmap::buildPatches(std::size_t face,
        const math::float3 &p1, const math::float3 &p2, const math::float3 &p3,
        const math::float2 &tc1, const math::float2 &tc2, const math::float2 &tc3,
        const math::float3 &n1, const math::float3 &n2, const math::float3 &n3, const math::float3& color) noexcept
    {
        if (orient2d(tc1, tc2, tc3) < 0)
            return buildPatches(face, p3, p2, p1, tc3, tc2, tc1, n3, n2, n1, color);

        auto scale = math::float2(this->width_, this->height_);
        auto uv1 = tc1 * scale;
        auto uv2 = tc2 * scale;
        auto uv3 = tc3 * scale;

        math::Box2 box;
        box.encapsulate(uv1);
        box.encapsulate(uv2);
        box.encapsulate(uv3);

        auto min = math::floor(box.min);
        auto max = math::ceil(box.max);

        min = math::max(min - math::float2::One, math::float2::Zero);
        max = math::min(max + math::float2::One, scale);

        math::float2 f12(uv1.y - uv2.y, uv2.x - uv1.x);
        math::float2 f23(uv2.y - uv3.y, uv3.x - uv2.x);
        math::float2 f31(uv3.y - uv1.y, uv1.x - uv3.x);

        auto rowW1 = orient2d(uv2, uv3, min);
        auto rowW2 = orient2d(uv3, uv1, min);
        auto rowW3 = orient2d(uv1, uv2, min);

        for (float y = min.y; y <= max.y; ++y)
        {
            auto w1 = rowW1;
            auto w2 = rowW2;
            auto w3 = rowW3;

            for (float x = min.x; x <= max.x; ++x)
            {
                if (w1 >= 0 && w2 >= 0 && w3 >= 0)
                {
                    std::size_t texelIndex = y * this->width_ + x;
                    if (!usedTexels[texelIndex])
                    {
                        auto normFactor = w1 + w2 + w3;
                        auto pw1 = w1 / normFactor;
                        auto pw2 = w2 / normFactor;
                        auto pw3 = w3 / normFactor;

                        LightmapPatch patch;
                        patch.texelIndex = texelIndex;
                        patch.normal = math::normalize(n1 * pw1 + n2 * pw2 + n3 * pw3);
                        patch.position = p1*pw1 + p2*pw2 + p3*pw3;
                        patch.surfaceIndex = face;
                        patch.color = color;
                        patch.v1 = p1;
                        patch.v2 = p2;
                        patch.v3 = p3;
                        this->patches_.push_back(patch);

                        usedTexels[texelIndex] = true;
                    }
                }

                w1 += f23.x;
                w2 += f31.x;
                w3 += f12.x;
            }

            rowW1 += f23.y;
            rowW2 += f31.y;
            rowW3 += f12.y;
        }
    }
}