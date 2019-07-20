#include "image_io.h"
#include "SceneGraph/texture.h"

#include "OpenImageIO/imageio.h"

#ifdef __linux__
#include "file_utils.h"
#endif

namespace Baikal
{
    class Oiio : public ImageIo
    {
    public:
        Texture::Ptr LoadImage(std::string const& filename) const override;
        void SaveImage(std::string const& filename, Texture::Ptr texture) const override;
    };

    static Texture::Format GetTextureFormat(OIIO_NAMESPACE::ImageSpec const& spec)
    {
        OIIO_NAMESPACE_USING

        if (spec.format.basetype == TypeDesc::UINT8)
            return Texture::Format::kRgba8;
        else if (spec.format.basetype == TypeDesc::HALF)
            return Texture::Format::kRgba16;
        else
            return Texture::Format::kRgba32;
    }

    static OIIO_NAMESPACE::TypeDesc GetTextureFormat(Texture::Format fmt)
    {
        OIIO_NAMESPACE_USING

        if (fmt == Texture::Format::kRgba8)
            return  TypeDesc::UINT8;
        else if (fmt == Texture::Format::kRgba16)
            return TypeDesc::HALF;
        else
            return TypeDesc::FLOAT;
    }

    Texture::Ptr Oiio::LoadImage(const std::string &filename) const
    {
        OIIO_NAMESPACE_USING

        std::string actual_filename = filename;

#ifdef __linux__
        if (!FindFilenameFromCaseInsensitive(filename, actual_filename))
        {
            throw std::runtime_error("Image " + filename + " doesn't exist");
        }
#endif

        std::unique_ptr<ImageInput> input{ImageInput::open(actual_filename)};

        if (!input)
        {
            throw std::runtime_error("Can't load " + filename + " image");
        }

        ImageSpec const& spec = input->spec();

        auto fmt = GetTextureFormat(spec);
        char* texturedata = nullptr;

        if (fmt == Texture::Format::kRgba8)
        {
            auto size = spec.width * spec.height * spec.depth * 4;

            texturedata = new char[size];
            memset(texturedata, 0, size);

            // Read data to storage
            input->read_image(TypeDesc::UINT8, texturedata, sizeof(char) * 4);

            if (spec.nchannels == 1)
            {
                // set B, G and A components to 
                for (auto i = 0; i < size; i += 4)
                {
                    texturedata[i + 1] = texturedata[i];
                    texturedata[i + 2] = texturedata[i];
                    texturedata[i + 3] = texturedata[i];
                }

            }

            // Close handle
            input->close();
        }
        else if (fmt == Texture::Format::kRgba16)
        {
            auto size = spec.width * spec.height * spec.depth * sizeof(float) * 2;

            // Resize storage
            texturedata = new char[size];
            memset(texturedata, 0, size);

            // Read data to storage
            input->read_image(TypeDesc::HALF, texturedata, sizeof(float) * 2);

            // Close handle
            input->close();
        }
        else
        {
            auto size = spec.width * spec.height * spec.depth * sizeof(RadeonRays::float3);

            // Resize storage
            texturedata = new char[size];
            memset(texturedata, 0, size);

            // Read data to storage
            input->read_image(TypeDesc::FLOAT, texturedata, sizeof(RadeonRays::float3));

            // Close handle
            input->close();
        }

        auto tex = Texture::Create(texturedata, RadeonRays::int3(spec.width, spec.height, spec.depth), fmt);;
        tex->SetName(filename);
        return tex;
    }

    void Oiio::SaveImage(std::string const& filename, Texture::Ptr texture) const
    {
        OIIO_NAMESPACE_USING;

        std::unique_ptr<ImageOutput> out{ImageOutput::create(filename)};

        if (!out)
        {
            throw std::runtime_error("Can't create image file on disk");
        }

        auto dim = texture->GetSize();
        auto fmt = GetTextureFormat(texture->GetFormat());

        ImageSpec spec(dim.x, dim.y, 4, fmt);

        out->open(filename, spec);

        out->write_image(fmt, texture->GetData());

        out->close();
    }

    std::unique_ptr<ImageIo> ImageIo::CreateImageIo()
    {
        return std::make_unique<Oiio>();
    }
}
