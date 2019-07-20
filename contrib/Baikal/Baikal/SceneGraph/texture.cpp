#include "texture.h"

#include "Utils/half.h"

namespace Baikal
{
    RadeonRays::float3 Texture::ComputeAverageValue() const
    {
        auto avg = RadeonRays::float3();

        switch (m_format) {
        case Format::kRgba8:
        {
            auto data = reinterpret_cast<std::uint8_t*>(m_data.get());
            auto num_elements = m_size.x * m_size.y * m_size.z;


            for (auto i = 0; i < num_elements; ++i)
            {
                float r = data[4 * i] / 255.f;
                float g = data[4 * i + 1] / 255.f;
                float b = data[4 * i + 2] / 255.f;
                avg += RadeonRays::float3(r, g, b);
            }

            avg *= (1.f / num_elements);
            break;
        }
        case Format::kRgba16:
        {
            auto data = reinterpret_cast<std::uint16_t*>(m_data.get());
            auto num_elements = m_size.x * m_size.y * m_size.z;

            for (auto i = 0; i < num_elements; ++i)
            {
                auto r = data[4 * i];
                auto g = data[4 * i + 1];
                auto b = data[4 * i + 2];

                half hr, hg, hb;
                hr.setBits(r);
                hg.setBits(g);
                hb.setBits(b);

                avg += RadeonRays::float3(hr, hg, hb);
            }

            avg *= (1.f / num_elements);
            break;
        }
        case Format::kRgba32:
        {
            auto data = reinterpret_cast<float*>(m_data.get());
            auto num_elements = m_size.x * m_size.y * m_size.z;

            for (auto i = 0; i < num_elements; ++i)
            {
                auto r = data[4 * i];
                auto g = data[4 * i + 1];
                auto b = data[4 * i + 2];

                avg += RadeonRays::float3(r, g, b);
            }

            avg *= (1.f / num_elements);
            break;
        }
        default:
            break;
        }

        return avg;
    }

    namespace {
        struct TextureConcrete : public Texture {
            TextureConcrete() = default;
            TextureConcrete(char* data, RadeonRays::int3 size, Format format) :
                Texture(data, size, format) {}
        };
    }

    Texture::Ptr Texture::Create() {
        return std::make_shared<TextureConcrete>();
    }

    Texture::Ptr Texture::Create(char* data, RadeonRays::int3 size, Format format) {
        return std::make_shared<TextureConcrete>(data, size, format);
    }
}
