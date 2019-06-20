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
#include "OpenImageIO/imageio.h"
#include "image_io.h"

#include "Application/cl_render.h"
#include "Application/gl_render.h"

#include "SceneGraph/scene1.h"
#include "SceneGraph/camera.h"
#include "SceneGraph/material.h"
#include "scene_io.h"
#include "material_io.h"
#include "SceneGraph/material.h"

#include "Renderers/monte_carlo_renderer.h"
#include "Renderers/adaptive_renderer.h"

#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include "Application/scene_load_utils.h"

#ifdef ENABLE_DENOISER
#include "PostEffects/wavelet_denoiser.h"
#endif
#include "Utils/clw_class.h"

namespace Baikal
{
    AppClRender::AppClRender(AppSettings& settings, GLuint tex) : m_tex(tex), m_output_type(Renderer::OutputType::kColor)
    {
        InitCl(settings, m_tex);
        InitScene(settings);
    }

    void AppClRender::InitCl(AppSettings& settings, GLuint tex)
    {
        // Create cl context
        ConfigManager::CreateConfigs(
            settings.mode,
            settings.interop,
            m_cfgs,
            settings.num_bounces,
            settings.platform_index,
            settings.device_index);

        m_width = (std::uint32_t)settings.width;
        m_height = (std::uint32_t)settings.height;

        std::cout << "Running on devices: \n";

        for (std::size_t i = 0; i < m_cfgs.size(); ++i)
        {
            const auto& device = m_cfgs[i].context.GetDevice(0);
            std::cout << i << ". name: " << device.GetName()
                << ", vendor: " << device.GetVendor()
                << ", version: " << device.GetVersion()
                << "\n";
        }

        settings.interop = false;

        m_outputs.resize(m_cfgs.size());
        m_ctrl.reset(new ControlData[m_cfgs.size()]);

        for (std::size_t i = 0; i < m_cfgs.size(); ++i)
        {
            if (m_cfgs[i].type == ConfigManager::kPrimary)
            {
                m_primary = static_cast<int>(i);

                if (m_cfgs[i].caninterop)
                {
                    m_cl_interop_image = m_cfgs[i].context.CreateImage2DFromGLTexture(tex);
                    settings.interop = true;
                }
            }

            m_ctrl[i].clear.store(1);
            m_ctrl[i].stop.store(0);
            m_ctrl[i].newdata.store(0);
            m_ctrl[i].idx = static_cast<int>(i);
            m_ctrl[i].scene_state = 0;
        }

        if (settings.interop)
        {
            std::cout << "OpenGL interop mode enabled\n";
        }
        else
        {
            std::cout << "OpenGL interop mode disabled\n";
        }

        //create renderer
        for (std::size_t i = 0; i < m_cfgs.size(); ++i)
        {
            m_outputs[i].output = m_cfgs[i].factory->CreateOutput(m_width, m_height);
            m_outputs[i].dummy_output = m_cfgs[i].factory->CreateOutput(m_width, m_height);

#ifdef ENABLE_DENOISER
            if (m_cfgs[i].type == ConfigManager::kPrimary)
            {
                CreateDenoiserOutputs(i, settings.width, settings.height);
                SetDenoiserOutputs(i);
                m_outputs[i].denoiser = m_cfgs[i].factory->CreatePostEffect(Baikal::RenderFactory<Baikal::ClwScene>::PostEffectType::kWaveletDenoiser);
            }
#endif
            m_cfgs[i].renderer->SetOutput(Baikal::Renderer::OutputType::kColor, m_outputs[i].output.get());

            m_outputs[i].fdata.resize(settings.width * settings.height);
            m_outputs[i].udata.resize(settings.width * settings.height * 4);

            if (m_cfgs[i].type == ConfigManager::kPrimary)
            {
                m_outputs[i].copybuffer = m_cfgs[i].context.CreateBuffer<RadeonRays::float3>(m_width * m_height, CL_MEM_READ_WRITE);
            }
        }

        m_shape_id_data.output = m_cfgs[m_primary].factory->CreateOutput(m_width, m_height);
        m_cfgs[m_primary].renderer->Clear(RadeonRays::float3(0, 0, 0), *m_outputs[m_primary].output);
        m_cfgs[m_primary].renderer->Clear(RadeonRays::float3(0, 0, 0), *m_shape_id_data.output);
    }


    void AppClRender::InitScene(AppSettings& settings)
    {
        rand_init();

        m_scene = LoadScene(settings);

        switch (settings.camera_type)
        {
        case CameraType::kPerspective:
            m_camera = Baikal::PerspectiveCamera::Create(
                settings.camera_pos
                , settings.camera_at
                , settings.camera_up);

            break;
        case CameraType::kOrthographic:
            m_camera = Baikal::OrthographicCamera::Create(
                settings.camera_pos
                , settings.camera_at
                , settings.camera_up);
            break;
        default:
            throw std::runtime_error("AppClRender::InitCl(...): unsupported camera type");
        }

        m_scene->SetCamera(m_camera);

        // Adjust sensor size based on current aspect ratio
        float aspect = (float)settings.width / settings.height;
        settings.camera_sensor_size.y = settings.camera_sensor_size.x / aspect;

        m_camera->SetSensorSize(settings.camera_sensor_size);
        m_camera->SetDepthRange(settings.camera_zcap);

        auto perspective_camera = std::dynamic_pointer_cast<Baikal::PerspectiveCamera>(m_camera);

        // if camera mode is kPerspective
        if (perspective_camera)
        {
            perspective_camera->SetFocalLength(settings.camera_focal_length);
            perspective_camera->SetFocusDistance(settings.camera_focus_distance);
            perspective_camera->SetAperture(settings.camera_aperture);
            std::cout << "Camera type: " << (perspective_camera->GetAperture() > 0.f ? "Physical" : "Pinhole") << "\n";
            std::cout << "Lens focal length: " << perspective_camera->GetFocalLength() * 1000.f << "mm\n";
            std::cout << "Lens focus distance: " << perspective_camera->GetFocusDistance() << "m\n";
            std::cout << "F-Stop: " << 1.f / (perspective_camera->GetAperture() * 10.f) << "\n";
        }

        std::cout << "Sensor size: " << settings.camera_sensor_size.x * 1000.f << "x" << settings.camera_sensor_size.y * 1000.f << "mm\n";
    }

    void AppClRender::UpdateScene()
    {
        for (std::size_t i = 0; i < m_cfgs.size(); ++i)
        {
            if (i == static_cast<std::size_t>(m_primary))
            {
                m_cfgs[i].renderer->Clear(float3(0, 0, 0), *m_outputs[i].output);
                m_cfgs[i].controller->CompileScene(m_scene);
                ++m_ctrl[i].scene_state;

#ifdef ENABLE_DENOISER
                ClearDenoiserOutputs(i);
#endif

            }
            else
                m_ctrl[i].clear.store(true);
        }
    }

    void AppClRender::Update(AppSettings& settings)
    {
        ++settings.samplecount;

        for (std::size_t i = 0; i < m_cfgs.size(); ++i)
        {
            if (m_cfgs[i].type == ConfigManager::kPrimary)
                continue;

            int desired = 1;
            if (std::atomic_compare_exchange_strong(&m_ctrl[i].newdata, &desired, 0))
            {
                if (m_ctrl[i].scene_state != m_ctrl[m_primary].scene_state)
                {
                    // Skip update if worker has sent us non-actual data
                    continue;
                }

                {
                    m_cfgs[m_primary].context.WriteBuffer(0, m_outputs[m_primary].copybuffer, &m_outputs[i].fdata[0], settings.width * settings.height);
                }

                auto acckernel = static_cast<Baikal::MonteCarloRenderer*>(m_cfgs[m_primary].renderer.get())->GetAccumulateKernel();

                int argc = 0;
                acckernel.SetArg(argc++, m_outputs[m_primary].copybuffer);
                acckernel.SetArg(argc++, settings.width * settings.width);
                acckernel.SetArg(argc++, static_cast<Baikal::ClwOutput*>(m_outputs[m_primary].output.get())->data());

                int globalsize = settings.width * settings.height;
                m_cfgs[m_primary].context.Launch1D(0, ((globalsize + 63) / 64) * 64, 64, acckernel);
                settings.samplecount += m_ctrl[i].new_samples_count;
            }
        }

        if (!settings.interop)
        {
#ifdef ENABLE_DENOISER
            m_outputs[m_primary].output_denoised->GetData(&m_outputs[m_primary].fdata[0]);
#else
            m_outputs[m_primary].output->GetData(&m_outputs[m_primary].fdata[0]);
#endif

            float gamma = 2.2f;
            for (int i = 0; i < (int)m_outputs[m_primary].fdata.size(); ++i)
            {
                m_outputs[m_primary].udata[4 * i] = (unsigned char)clamp(clamp(pow(m_outputs[m_primary].fdata[i].x / m_outputs[m_primary].fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255);
                m_outputs[m_primary].udata[4 * i + 1] = (unsigned char)clamp(clamp(pow(m_outputs[m_primary].fdata[i].y / m_outputs[m_primary].fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255);
                m_outputs[m_primary].udata[4 * i + 2] = (unsigned char)clamp(clamp(pow(m_outputs[m_primary].fdata[i].z / m_outputs[m_primary].fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255);
                m_outputs[m_primary].udata[4 * i + 3] = 1;
            }

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_tex);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_outputs[m_primary].output->width(), m_outputs[m_primary].output->height(), GL_RGBA, GL_UNSIGNED_BYTE, &m_outputs[m_primary].udata[0]);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            std::vector<cl_mem> objects;
            objects.push_back(m_cl_interop_image);
            m_cfgs[m_primary].context.AcquireGLObjects(0, objects);

            auto copykernel = static_cast<Baikal::MonteCarloRenderer*>(m_cfgs[m_primary].renderer.get())->GetCopyKernel();

#ifdef ENABLE_DENOISER
            auto output = m_outputs[m_primary].output_denoised.get();
#else
            auto output = m_outputs[m_primary].output.get();
#endif

            int argc = 0;

            copykernel.SetArg(argc++, static_cast<Baikal::ClwOutput*>(output)->data());
            copykernel.SetArg(argc++, output->width());
            copykernel.SetArg(argc++, output->height());
            copykernel.SetArg(argc++, 2.2f);
            copykernel.SetArg(argc++, m_cl_interop_image);

            int globalsize = output->width() * output->height();
            m_cfgs[m_primary].context.Launch1D(0, ((globalsize + 63) / 64) * 64, 64, copykernel);

            m_cfgs[m_primary].context.ReleaseGLObjects(0, objects);
            m_cfgs[m_primary].context.Finish(0);
        }


        if (settings.benchmark)
        {
            auto& scene = m_cfgs[m_primary].controller->CompileScene(m_scene);
            static_cast<Baikal::MonteCarloRenderer*>(m_cfgs[m_primary].renderer.get())->Benchmark(scene, settings.stats);

            settings.benchmark = false;
            settings.rt_benchmarked = true;
        }

    }

    void AppClRender::Render(int sample_cnt)
    {
#ifdef ENABLE_DENOISER
        WaveletDenoiser* wavelet_denoiser = dynamic_cast<WaveletDenoiser*>(m_outputs[m_primary].denoiser.get());

        if (wavelet_denoiser != nullptr)
        {
            wavelet_denoiser->Update(static_cast<PerspectiveCamera*>(m_camera.get()));
        }
#endif

        auto& scene = m_cfgs[m_primary].controller->GetCachedScene(m_scene);
        m_cfgs[m_primary].renderer->Render(scene);

        if (m_shape_id_requested)
        {
            // offset in OpenCl memory till necessary item
            auto offset = (std::uint32_t)(m_width * (m_height - m_shape_id_pos.y) + m_shape_id_pos.x);
            // copy shape id elem from OpenCl
            float4 shape_id;
            m_shape_id_data.output->GetData((float3*)&shape_id, offset, 1);
            m_promise.set_value(static_cast<int>(shape_id.x));
            // clear output to stop tracking shape id map in openCl
            m_cfgs[m_primary].renderer->SetOutput(Renderer::OutputType::kShapeId, nullptr);
            m_shape_id_requested = false;
        }

#ifdef ENABLE_DENOISER
        Baikal::PostEffect::InputSet input_set;
        input_set[Baikal::Renderer::OutputType::kColor] = m_outputs[m_primary].output.get();
        input_set[Baikal::Renderer::OutputType::kWorldShadingNormal] = m_outputs[m_primary].output_normal.get();
        input_set[Baikal::Renderer::OutputType::kWorldPosition] = m_outputs[m_primary].output_position.get();
        input_set[Baikal::Renderer::OutputType::kAlbedo] = m_outputs[m_primary].output_albedo.get();
        input_set[Baikal::Renderer::OutputType::kMeshID] = m_outputs[m_primary].output_mesh_id.get();

        auto radius = 10U - RadeonRays::clamp((sample_cnt / 16), 1U, 9U);
        auto position_sensitivity = 5.f + 10.f * (radius / 10.f);

        const bool is_bilateral_denoiser = dynamic_cast<BilateralDenoiser*>(m_outputs[m_primary].denoiser.get()) != nullptr;

        if (is_bilateral_denoiser)
        {

            auto normal_sensitivity = 0.1f + (radius / 10.f) * 0.15f;
            auto color_sensitivity = (radius / 10.f) * 2.f;
            auto albedo_sensitivity = 0.5f + (radius / 10.f) * 0.5f;
            m_outputs[m_primary].denoiser->SetParameter("radius", static_cast<float>(radius));
            m_outputs[m_primary].denoiser->SetParameter("color_sensitivity", color_sensitivity);
            m_outputs[m_primary].denoiser->SetParameter("normal_sensitivity", normal_sensitivity);
            m_outputs[m_primary].denoiser->SetParameter("position_sensitivity", position_sensitivity);
            m_outputs[m_primary].denoiser->SetParameter("albedo_sensitivity", albedo_sensitivity);
        }

        m_outputs[m_primary].denoiser->Apply(input_set, *m_outputs[m_primary].output_denoised);
#endif
    }

    void AppClRender::SaveFrameBuffer(AppSettings& settings)
    {
        std::vector<RadeonRays::float3> data;

        //read cl output in case of iterop
        std::vector<RadeonRays::float3> output_data;
        if (settings.interop)
        {
            auto output = m_outputs[m_primary].output.get();
            auto buffer = static_cast<Baikal::ClwOutput*>(output)->data();
            output_data.resize(buffer.GetElementCount());
            m_cfgs[m_primary].context.ReadBuffer(0, static_cast<Baikal::ClwOutput*>(output)->data(), &output_data[0], output_data.size()).Wait();
        }

        //use already copied to CPU cl data in case of no interop
        auto& fdata = settings.interop ? output_data : m_outputs[m_primary].fdata;

        data.resize(fdata.size());
        std::transform(fdata.cbegin(), fdata.cend(), data.begin(),
            [](RadeonRays::float3 const& v)
        {
            float invw = 1.f / v.w;
            return v * invw;
        });

        std::stringstream oss;
        auto camera_position = m_camera->GetPosition();
        auto camera_direction = m_camera->GetForwardVector();
        oss << "../Output/" << settings.modelname << "_p" << camera_position.x << camera_position.y << camera_position.z <<
            "_d" << camera_direction.x << camera_direction.y << camera_direction.z <<
            "_s" << settings.num_samples << ".exr";

        SaveImage(oss.str(), settings.width, settings.height, data.data());
    }

    void AppClRender::SaveImage(const std::string& name, int width, int height, const RadeonRays::float3* data)
    {
        OIIO_NAMESPACE_USING;

        std::vector<float3> tempbuf(width * height);
        tempbuf.assign(data, data + width * height);

        for (auto y = 0; y < height; ++y)
            for (auto x = 0; x < width; ++x)
            {

                float3 val = data[(height - 1 - y) * width + x];
                tempbuf[y * width + x] = (1.f / val.w) * val;

                tempbuf[y * width + x].x = std::pow(tempbuf[y * width + x].x, 1.f / 2.2f);
                tempbuf[y * width + x].y = std::pow(tempbuf[y * width + x].y, 1.f / 2.2f);
                tempbuf[y * width + x].z = std::pow(tempbuf[y * width + x].z, 1.f / 2.2f);
            }

        ImageOutput* out = ImageOutput::create(name);

        if (!out)
        {
            throw std::runtime_error("Can't create image file on disk");
        }

        ImageSpec spec(width, height, 3, TypeDesc::FLOAT);

        out->open(name, spec);
        out->write_image(TypeDesc::FLOAT, &tempbuf[0], sizeof(float3));
        out->close();
    }

    void AppClRender::RenderThread(ControlData& cd)
    {
        auto renderer = m_cfgs[cd.idx].renderer.get();
        auto controller = m_cfgs[cd.idx].controller.get();
        auto output = m_outputs[cd.idx].output.get();

        auto updatetime = std::chrono::high_resolution_clock::now();

        std::uint32_t scene_state = 0;
        std::uint32_t new_samples_count = 0;

        while (!cd.stop.load())
        {
            int result = 1;
            bool update = false;
            if (std::atomic_compare_exchange_strong(&cd.clear, &result, 0))
            {
                renderer->Clear(float3(0, 0, 0), *output);
                controller->CompileScene(m_scene);
                scene_state = m_ctrl[m_primary].scene_state;
                update = true;
            }

            auto& scene = controller->GetCachedScene(m_scene);
            renderer->Render(scene);
            ++new_samples_count;

            auto now = std::chrono::high_resolution_clock::now();

            update = update || (std::chrono::duration_cast<std::chrono::seconds>(now - updatetime).count() > 1);

            if (update)
            {
                m_outputs[cd.idx].output->GetData(&m_outputs[cd.idx].fdata[0]);
                updatetime = now;
                m_ctrl[cd.idx].scene_state = scene_state;
                m_ctrl[cd.idx].new_samples_count = new_samples_count;
                new_samples_count = 0;
                cd.newdata.store(1);
            }

            m_cfgs[cd.idx].context.Finish(0);
        }
    }

    void AppClRender::StartRenderThreads()
    {
        for (std::size_t i = 0; i < m_cfgs.size(); ++i)
        {
            if (i != static_cast<std::size_t>(m_primary))
            {
                m_renderthreads.push_back(std::thread(&AppClRender::RenderThread, this, std::ref(m_ctrl[i])));
            }
        }

        std::cout << m_cfgs.size() << " OpenCL submission threads started\n";
    }

    void AppClRender::StopRenderThreads()
    {
        for (std::size_t i = 0; i < m_cfgs.size(); ++i)
        {
            if (i == static_cast<std::size_t>(m_primary))
            {
                continue;
            }

            m_ctrl[i].stop.store(true);
        }

        for (std::size_t i = 0; i < m_renderthreads.size(); ++i)
        {
            m_renderthreads[i].join();
        }

    }

    void AppClRender::RunBenchmark(AppSettings& settings)
    {
        std::cout << "Running general benchmark...\n";

        auto time_bench_start_time = std::chrono::high_resolution_clock::now();
        for (auto i = 0U; i < 512; ++i)
        {
            Render(0);
        }

        m_cfgs[m_primary].context.Finish(0);

        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::high_resolution_clock::now() - time_bench_start_time).count();

        settings.time_benchmark_time = delta / 1000.f;

        m_outputs[m_primary].output->GetData(&m_outputs[m_primary].fdata[0]);
        float gamma = 2.2f;
        for (int i = 0; i < (int)m_outputs[m_primary].fdata.size(); ++i)
        {
            m_outputs[m_primary].udata[4 * i] = (unsigned char)clamp(clamp(pow(m_outputs[m_primary].fdata[i].x / m_outputs[m_primary].fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255);
            m_outputs[m_primary].udata[4 * i + 1] = (unsigned char)clamp(clamp(pow(m_outputs[m_primary].fdata[i].y / m_outputs[m_primary].fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255);
            m_outputs[m_primary].udata[4 * i + 2] = (unsigned char)clamp(clamp(pow(m_outputs[m_primary].fdata[i].z / m_outputs[m_primary].fdata[i].w, 1.f / gamma), 0.f, 1.f) * 255, 0, 255);
            m_outputs[m_primary].udata[4 * i + 3] = 1;
        }

        auto& fdata = m_outputs[m_primary].fdata;
        std::vector<RadeonRays::float3> data(fdata.size());
        std::transform(fdata.cbegin(), fdata.cend(), data.begin(),
            [](RadeonRays::float3 const& v)
        {
            float invw = 1.f / v.w;
            return v * invw;
        });

        std::stringstream oss;
        oss << "../Output/" << settings.modelname << ".exr";

        SaveImage(oss.str(), settings.width, settings.height, &data[0]);

        std::cout << "Running RT benchmark...\n";

        auto& scene = m_cfgs[m_primary].controller->GetCachedScene(m_scene);
        static_cast<MonteCarloRenderer*>(m_cfgs[m_primary].renderer.get())->Benchmark(scene, settings.stats);
    }

    void AppClRender::SetNumBounces(int num_bounces)
    {
        for (std::size_t i = 0; i < m_cfgs.size(); ++i)
        {
            static_cast<Baikal::MonteCarloRenderer*>(m_cfgs[i].renderer.get())->SetMaxBounces(num_bounces);
        }
    }

    void AppClRender::SetOutputType(Renderer::OutputType type)
    {
        for (std::size_t i = 0; i < m_cfgs.size(); ++i)
        {
#ifdef ENABLE_DENOISER
            RestoreDenoiserOutput(i, m_output_type);
#else
            m_cfgs[i].renderer->SetOutput(m_output_type, nullptr);
#endif
            if (type == Renderer::OutputType::kOpacity || type == Renderer::OutputType::kVisibility)
            {
                m_cfgs[i].renderer->SetOutput(Renderer::OutputType::kColor, m_outputs[i].dummy_output.get());
            }
            else
            {
                m_cfgs[i].renderer->SetOutput(Renderer::OutputType::kColor, nullptr);
            }
            m_cfgs[i].renderer->SetOutput(type, m_outputs[i].output.get());
        }
        m_output_type = type;
    }


    std::future<int> AppClRender::GetShapeId(std::uint32_t x, std::uint32_t y)
    {
        m_promise = std::promise<int>();
        if (x >= m_width || y >= m_height)
            throw std::logic_error(
                "AppClRender::GetShapeId(...): x or y cords beyond the size of image");

        if (m_cfgs.empty())
            throw std::runtime_error("AppClRender::GetShapeId(...): config vector is empty");

        // enable aov shape id output from OpenCl
        m_cfgs[m_primary].renderer->SetOutput(
            Renderer::OutputType::kShapeId, m_shape_id_data.output.get());
        m_shape_id_pos = RadeonRays::float2((float)x, (float)y);
        // request shape id from render
        m_shape_id_requested = true;
        return m_promise.get_future();
    }

    Baikal::Shape::Ptr AppClRender::GetShapeById(int shape_id)
    {
        if (shape_id < 0)
            return nullptr;

        // find shape in scene by its id
        for (auto iter = m_scene->CreateShapeIterator(); iter->IsValid(); iter->Next())
        {
            auto shape = iter->ItemAs<Shape>();
            if (shape->GetId() == static_cast<std::size_t>(shape_id))
                return shape;
        }
        return nullptr;
    }

#ifdef ENABLE_DENOISER
    void AppClRender::SetDenoiserFloatParam(const std::string& name, const float4& value)
    {
        m_outputs[m_primary].denoiser->SetParameter(name, value);
    }

    float4 AppClRender::GetDenoiserFloatParam(const std::string& name)
    {
        return m_outputs[m_primary].denoiser->GetParameter(name);
    }

    void AppClRender::CreateDenoiserOutputs(std::size_t cfg_index, int width, int height)
    {
        m_outputs[cfg_index].output_denoised = m_cfgs[cfg_index].factory->CreateOutput(width, height);
        m_outputs[cfg_index].output_normal = m_cfgs[cfg_index].factory->CreateOutput(width, height);
        m_outputs[cfg_index].output_position = m_cfgs[cfg_index].factory->CreateOutput(width, height);
        m_outputs[cfg_index].output_albedo = m_cfgs[cfg_index].factory->CreateOutput(width, height);
        m_outputs[cfg_index].output_mesh_id = m_cfgs[cfg_index].factory->CreateOutput(width, height);
    }

    void AppClRender::SetDenoiserOutputs(std::size_t cfg_index) const
    {
        m_cfgs[cfg_index].renderer->SetOutput(Renderer::OutputType::kWorldShadingNormal, m_outputs[cfg_index].output_normal.get());
        m_cfgs[cfg_index].renderer->SetOutput(Renderer::OutputType::kWorldPosition, m_outputs[cfg_index].output_position.get());
        m_cfgs[cfg_index].renderer->SetOutput(Renderer::OutputType::kAlbedo, m_outputs[cfg_index].output_albedo.get());
        m_cfgs[cfg_index].renderer->SetOutput(Renderer::OutputType::kMeshID, m_outputs[cfg_index].output_mesh_id.get());
    }

    void AppClRender::ClearDenoiserOutputs(std::size_t cfg_index) const
    {
        m_cfgs[cfg_index].renderer->Clear(float3(0, 0, 0), *m_outputs[cfg_index].output_normal);
        m_cfgs[cfg_index].renderer->Clear(float3(0, 0, 0), *m_outputs[cfg_index].output_position);
        m_cfgs[cfg_index].renderer->Clear(float3(0, 0, 0), *m_outputs[cfg_index].output_albedo);
        m_cfgs[cfg_index].renderer->Clear(float3(0, 0, 0), *m_outputs[cfg_index].output_mesh_id);
    }

    void AppClRender::RestoreDenoiserOutput(std::size_t cfg_index, Renderer::OutputType type) const
    {
        switch (type)
        {
        case Renderer::OutputType::kWorldShadingNormal:
            m_cfgs[cfg_index].renderer->SetOutput(type, m_outputs[cfg_index].output_normal.get());
            break;
        case Renderer::OutputType::kWorldPosition:
            m_cfgs[cfg_index].renderer->SetOutput(type, m_outputs[cfg_index].output_position.get());
            break;
        case Renderer::OutputType::kAlbedo:
            m_cfgs[cfg_index].renderer->SetOutput(type, m_outputs[cfg_index].output_albedo.get());
            break;
        case Renderer::OutputType::kMeshID:
            m_cfgs[cfg_index].renderer->SetOutput(type, m_outputs[cfg_index].output_mesh_id.get());
            break;
        default:
            // Nothing to restore
            m_cfgs[cfg_index].renderer->SetOutput(type, nullptr);
            break;
        }
    }
#endif
} // Baikal
