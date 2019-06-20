/**********************************************************************
Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.

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

#include "data_generator_impl.h"

#include "filesystem.h"
#include "devices.h"
#include "logging.h"
#include "utils.h"

#include "Baikal/Output/clwoutput.h"
#include "Baikal/Renderers/monte_carlo_renderer.h"
#include "Baikal/Renderers/renderer.h"
#include "Baikal/RenderFactory/clw_render_factory.h"

#include "OpenImageIO/imageio.h"

#include "XML/tinyxml2.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <vector>


struct OutputInfo
{
    Baikal::Renderer::OutputType type;
    std::string name;
    // channels number can be only 1 or 3
    int channels_num;
};


namespace {

    // if you need to add new output for saving to disk
    // for iterations number counted in spp.xml file
    // just put its description in this collection
    const std::vector<OutputInfo> kMultipleIteratedOutputs
    {
        {Baikal::Renderer::OutputType::kColor, "color", 3},
        {Baikal::Renderer::OutputType::kAlbedo, "albedo", 3},
        {Baikal::Renderer::OutputType::kGloss, "gloss", 1}
    };

    // if you need to add new output for saving to disk
    // only for the one time
    // just put its description in this collection
    const std::vector<OutputInfo> kSingleIteratedOutputs
    {
        {Baikal::Renderer::OutputType::kViewShadingNormal, "view_shading_normal", 3},
        {Baikal::Renderer::OutputType::kDepth, "view_shading_depth", 1}
    };

} // namespace


DataGeneratorImpl::DataGeneratorImpl(SceneObject* scene,
                                     std::string const& scene_name,
                                     unsigned width, unsigned height,
                                     unsigned num_bounces,
                                     unsigned device_idx,
                                     const std::vector<unsigned>& sorted_spp,
                                     const std::filesystem::path& output_dir,
                                     bool gamma_correction_enable)
    : m_scene(scene),
      m_scene_name(scene_name),
      m_width(width),
      m_height(height),
      m_num_bounces(num_bounces),
      m_device_idx(device_idx),
      m_output_dir(output_dir),
      m_sorted_spp(sorted_spp),
      m_gamma_correction_enabled(gamma_correction_enable)
{
    auto devices = GetDevices();
    if (devices.empty())
    {
        THROW_EX("Cannot find any device");
    }
    if (device_idx >= devices.size())
    {
        THROW_EX("Cannot find device with index " << device_idx);
    }

    m_context = std::make_unique<CLWContext>(CLWContext::Create(devices[device_idx]));

    m_factory = std::make_unique<Baikal::ClwRenderFactory>(*m_context, "cache");

    auto render = m_factory->CreateRenderer(
        Baikal::ClwRenderFactory::RendererType::kUnidirectionalPathTracer);
    m_renderer.reset(dynamic_cast<Baikal::MonteCarloRenderer*>(render.release()));

    m_controller = m_factory->CreateSceneController();

    for (auto& output_info : kMultipleIteratedOutputs)
    {
        m_outputs.push_back(m_factory->CreateOutput(width, height));
        m_renderer->SetOutput(output_info.type, m_outputs.back().get());
    }
    for (auto& output_info : kSingleIteratedOutputs)
    {
        m_outputs.push_back(m_factory->CreateOutput(width, height));
        m_renderer->SetOutput(output_info.type, m_outputs.back().get());
    }

    m_renderer->SetMaxBounces(num_bounces);
}

void DataGeneratorImpl::SaveMetadata() const
{
    tinyxml2::XMLDocument doc;

    auto bdg_metadata_file_name = m_output_dir;
    bdg_metadata_file_name.append("metadata.xml");

    auto* root = doc.NewElement("metadata");
    doc.InsertFirstChild(root);

    auto* scene = doc.NewElement("scene");
    scene->SetAttribute("file", m_scene_name.c_str());
    root->InsertEndChild(scene);

    auto* outputs_list = doc.NewElement("outputs");
    outputs_list->SetAttribute("width", m_width);
    outputs_list->SetAttribute("height", m_height);
    root->InsertEndChild(outputs_list);

    std::vector<OutputInfo> outputs = kSingleIteratedOutputs;
    outputs.insert(outputs.end(), kMultipleIteratedOutputs.begin(), kMultipleIteratedOutputs.end());

    for (const auto& output : outputs)
    {
        auto* outputs_item = doc.NewElement("output");
        outputs_item->SetAttribute("name", output.name.c_str());
        outputs_item->SetAttribute("type", "float32");
        outputs_item->SetAttribute("channels", output.channels_num);
        if (output.type == Baikal::Renderer::OutputType::kColor)
        {
            outputs_item->SetAttribute("gamma_correction", m_gamma_correction_enabled);
        }
        outputs_list->InsertEndChild(outputs_item);
    }

    // log render settings
    auto* renderer_attribute = doc.NewElement("renderer");
    renderer_attribute->SetAttribute("num_bounces", m_num_bounces);
    root->InsertEndChild(renderer_attribute);

    auto* device_attribute = doc.NewElement("device");
    auto device = GetDevices().at(m_device_idx);
    device_attribute->SetAttribute("idx", m_device_idx);
    device_attribute->SetAttribute("name", device.GetName().c_str());
    device_attribute->SetAttribute("vendor", device.GetVendor().c_str());
    device_attribute->SetAttribute("version", device.GetVersion().c_str());
    root->InsertEndChild(device_attribute);

    doc.SaveFile(bdg_metadata_file_name.string().c_str());
}

void DataGeneratorImpl::GenerateCameraData(CameraObject* camera, unsigned camera_idx)
{
    m_scene->SetCamera(camera);

    for (const auto& output : m_outputs)
    {
        output->Clear({});
    }

    // recompile scene cause of changing camera pos and settings
    auto& scene = m_controller->CompileScene(m_scene->GetScene());

    auto spp_iter = m_sorted_spp.begin();

    for (auto spp = 1u; spp <= m_sorted_spp.back(); spp++)
    {
        m_renderer->Render(scene);

        if (spp == 1)
        {
            for (const auto& output : kSingleIteratedOutputs)
            {
                std::stringstream ss;

                ss << "cam_" << camera_idx << "_"
                   << output.name << ".bin";

                SaveOutput(output,
                           ss.str(),
                           m_gamma_correction_enabled,
                           m_output_dir);
            }
        }

        if (*spp_iter == spp)
        {
            for (const auto& output : kMultipleIteratedOutputs)
            {
                std::stringstream ss;

                ss << "cam_" << camera_idx << "_"
                   << output.name << "_spp_" << spp << ".bin";

                SaveOutput(output,
                           ss.str(),
                           m_gamma_correction_enabled,
                           m_output_dir);
            }
            ++spp_iter;
        }
    }
}

void DataGeneratorImpl::SaveOutput(const OutputInfo& info,
                                   const std::string& name,
                                   bool gamma_correction_enabled,
                                   const std::filesystem::path& output_dir)
{
    using RadeonRays::float3;

    OIIO_NAMESPACE_USING;

    auto output = m_renderer->GetOutput(info.type);

    assert(output);

    auto buffer = dynamic_cast<Baikal::ClwOutput*>(output)->data();

    std::vector<RadeonRays::float3> output_data(buffer.GetElementCount());

    output->GetData(output_data.data());

    std::vector<float> image_data(info.channels_num * m_width * m_height);

    float* dst_row = image_data.data();

    if (gamma_correction_enabled &&
        (info.type == Baikal::Renderer::OutputType::kColor) &&
        (info.channels_num == 3))
    {
        for (auto y = 0u; y < m_height; ++y)
        {
            for (auto x = 0u; x < m_width; ++x)
            {
                float3 val = output_data[y * m_width + x];
                // "The 4-th pixel component is a count of accumulated samples.
                // It can be different for every pixel in case of adaptive sampling.
                // So, we need to normalize pixel values here".
                val *= (1.f / val.w);
                // gamma corection
                dst_row[info.channels_num * x] = std::pow(val.x, 1.f / 2.2f);
                dst_row[info.channels_num * x + 1] = std::pow(val.y, 1.f / 2.2f);
                dst_row[info.channels_num * x + 2] = std::pow(val.z, 1.f / 2.2f);
            }
            dst_row += info.channels_num * m_width;
        }
    }
    else
    {
        for (auto y = 0u; y < m_height; ++y)
        {
            for (auto x = 0u; x < m_width; ++x)
            {
                float3 val = output_data[y * m_width + x];
                // "The 4-th pixel component is a count of accumulated samples.
                // It can be different for every pixel in case of adaptive sampling.
                // So, we need to normalize pixel values here".
                val *= (1.f / val.w);

                if (info.channels_num == 3)
                {
                    // invert the image
                    dst_row[info.channels_num * x] = val.x;
                    dst_row[info.channels_num * x + 1] = val.y;
                    dst_row[info.channels_num * x + 2] = val.z;
                }
                else // info.channels_num = 1
                {
                    // invert the image
                    dst_row[x] = val.x;
                }
            }
            dst_row += info.channels_num * m_width;
        }
    }

    auto file_name = output_dir / name;

    std::ofstream f (file_name.string(), std::ofstream::binary);

    f.write(reinterpret_cast<const char*>(image_data.data()),
            sizeof(float) * image_data.size());
}

// Enable forward declarations for types stored in unique_ptr
DataGeneratorImpl::~DataGeneratorImpl() = default;
