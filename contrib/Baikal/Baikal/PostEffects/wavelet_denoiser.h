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
#pragma once
#include "clw_post_effect.h"

#include <SceneGraph/camera.h>
#include <math/matrix.h>
#include <math/mathutils.h>
#include "AreaMap33.h"

#include <limits>

#ifdef BAIKAL_EMBED_KERNELS
#include "embed_kernels.h"
#endif

namespace Baikal
{
    /**
    \brief SVGF wavelet denoiser.

    \details SVGF implements wavelet filter with edge-stopping function. Edge-stopping function is tuned
    by spatio-temporal variance. Temporal component is presented by sample reconstruction from history frames.
    Filter performs multiple passes, inserting pow(2, pass_index - 1) holes in
    kernel on each pass.
    Parameters:
    * color_sensitivity - Higher the sensitivity the more it smoothes out depending on color difference.
    * position_sensitivity - Higher the sensitivity the more it smoothes out depending on position difference.
    Required AOVs in input set:
    * kColor
    * kAlbedo
    * kMeshID
    * kWorldShadingNormal
    * kWorldPosition
    */
    class WaveletDenoiser : public ClwPostEffect
    {
    public:
        // Constructor
        WaveletDenoiser(CLWContext context, const CLProgramManager *program_manager);
        virtual ~WaveletDenoiser();
        // Apply filter
        void Apply(InputSet const& input_set, Output& output) override;
        void Update(PerspectiveCamera* camera);

    private:
        // Find required output
        ClwOutput* FindOutput(InputSet const& input_set, Renderer::OutputType type);

        CLWProgram  m_program;

        // Ping-pong buffers for wavelet pass
        const static uint32_t m_num_tmp_buffers = 2;

        uint32_t            m_current_buffer_index;

        ClwOutput*          m_motion_buffer;

        ClwOutput*          m_colors[m_num_tmp_buffers];
        ClwOutput*          m_positions[m_num_tmp_buffers];
        ClwOutput*          m_normals[m_num_tmp_buffers];
        ClwOutput*          m_mesh_ids[m_num_tmp_buffers];
        ClwOutput*          m_tmp_buffers[m_num_tmp_buffers];
        ClwOutput*          m_updated_variance;
        ClwOutput*          m_moments[m_num_tmp_buffers];

        // MLAA buffers
        ClwOutput*			m_edge_detection;
        ClwOutput*			m_blending_weight_calculation;

        // Number of wavelet passes
        uint32_t            m_max_wavelet_passes;

        RadeonRays::matrix              m_view_proj;
        RadeonRays::matrix              m_prev_view_proj;

        CLWBuffer<float>    m_view_proj_buffer;
        CLWBuffer<float>    m_prev_view_proj_buffer;
        CLWBuffer<float>    m_area_map_buffer;

        uint32_t            m_buffers_width;
        uint32_t            m_buffers_height;

        bool                m_buffers_initialized;
    };

    inline WaveletDenoiser::WaveletDenoiser(CLWContext context, const CLProgramManager *program_manager)
#ifdef BAIKAL_EMBED_KERNELS
        : ClwPostEffect(context, program_manager, "wavelet_denoise", g_wavelet_denoise_opencl, g_wavelet_denoise_opencl_headers)
#else
        : ClwPostEffect(context, program_manager, "../../system/Kernels/CL/wavelet_denoise.cl")
#endif
        , m_current_buffer_index(0) 
        , m_max_wavelet_passes(5)
        , m_buffers_width(0)
        , m_buffers_height(0)
        , m_buffers_initialized(false)
		, m_updated_variance(nullptr)
		, m_motion_buffer(nullptr)
		, m_edge_detection(nullptr)
		, m_blending_weight_calculation(nullptr)
    {
        // Add necessary params
        RegisterParameter("color_sensitivity", RadeonRays::float4(0.07f, 0.f, 0.f, 0.f));
        RegisterParameter("position_sensitivity", RadeonRays::float4(0.03f, 0.f, 0.f, 0.f));
        RegisterParameter("normal_sensitivity", RadeonRays::float4(0.01f, 0.f, 0.f, 0.f));

        for (uint32_t buffer_index = 0; buffer_index < m_num_tmp_buffers; buffer_index++)
        {
            m_tmp_buffers[buffer_index] = nullptr;
            m_colors[buffer_index] = nullptr;
			m_positions[buffer_index] = nullptr;
			m_normals[buffer_index] = nullptr;
			m_mesh_ids[buffer_index] = nullptr;
			m_moments[buffer_index] = nullptr;
        }

        m_view_proj_buffer = context.CreateBuffer<float>(16, CL_MEM_READ_WRITE);
        m_prev_view_proj_buffer = context.CreateBuffer<float>(16, CL_MEM_READ_WRITE);

        // Area map for MLAA weight coefficients
        // Texture object takes ownership on texture data, so we need to copy area map into temporary buffer
        char *buf = new char[sizeof(area_map_33)];
        memcpy(buf, area_map_33, sizeof(area_map_33));
        Texture::Ptr area_map = Texture::Create(buf, RadeonRays::int3(165, 165, 1), Texture::Format::kRgba8);

        if (area_map == nullptr)
            throw std::runtime_error("AreaMap33.dds is missing");

        std::vector<float> area_map_values;

        const size_t area_map_buffer_size_in_bytes = area_map->GetSizeInBytes();
        const size_t pixels_num = area_map->GetSize().x * area_map->GetSize().y;

        area_map_values.reserve(pixels_num);

        const unsigned char* area_map_data = (const unsigned char*)area_map->GetData();

        // Normalize area texture values
        for (size_t i = 0; i < area_map_buffer_size_in_bytes; i += 4)
        {
            float r = static_cast<float>(area_map_data[i]) / 255.0f;
            float g = static_cast<float>(area_map_data[i + 1]) / 255.0f;
            float b = static_cast<float>(area_map_data[i + 2]) / 255.0f;
            float a = static_cast<float>(area_map_data[i + 3]) / 255.0f;

            area_map_values.push_back(r);
            area_map_values.push_back(g);
            area_map_values.push_back(b);
            area_map_values.push_back(a);
        }

        m_area_map_buffer = context.CreateBuffer<float>(area_map_values.size(), CL_MEM_READ_WRITE);

        context.WriteBuffer(0, m_area_map_buffer, &area_map_values[0], area_map_values.size()).Wait();
    }

    inline WaveletDenoiser::~WaveletDenoiser()
    {
        for (uint32_t i = 0; i < m_num_tmp_buffers; i++)
        {
            delete m_tmp_buffers[i];
            delete m_colors[i];
            delete m_positions[i];
            delete m_normals[i];
            delete m_mesh_ids[i];
            delete m_moments[i];
        }

        delete m_updated_variance;
        delete m_motion_buffer;

        delete m_edge_detection;
        delete m_blending_weight_calculation;
    }

    inline ClwOutput* WaveletDenoiser::FindOutput(InputSet const& input_set, Renderer::OutputType type)
    {
        auto iter = input_set.find(type);

        if (iter == input_set.cend())
        {
            return nullptr;
        }

        return static_cast<ClwOutput*>(iter->second);
    }

    inline void WaveletDenoiser::Apply(InputSet const& input_set, Output& output)
    {
        uint32_t prev_buffer_index = m_current_buffer_index;
        m_current_buffer_index = (m_current_buffer_index + 1) % m_num_tmp_buffers;

        auto sigma_color = GetParameter("color_sensitivity").x;
        auto sigma_position = GetParameter("position_sensitivity").x;

        auto color = FindOutput(input_set, Renderer::OutputType::kColor);
        auto normal = FindOutput(input_set, Renderer::OutputType::kWorldShadingNormal);
        auto position = FindOutput(input_set, Renderer::OutputType::kWorldPosition);
        auto albedo = FindOutput(input_set, Renderer::OutputType::kAlbedo);
        auto mesh_id = FindOutput(input_set, Renderer::OutputType::kMeshID);

        auto out_color = static_cast<ClwOutput*>(&output);

        auto color_width = color->width();
        auto color_height = color->height();

        // Create ping pong buffers if they still need to be initialized
        if (!m_buffers_initialized)
        {
            for (uint32_t buffer_index = 0; buffer_index < m_num_tmp_buffers; buffer_index++)
            {
                m_tmp_buffers[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);

                m_colors[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);
                m_positions[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);
                m_normals[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);
                m_moments[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);
                m_mesh_ids[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);

                m_tmp_buffers[buffer_index]->Clear(0.f);
                m_colors[buffer_index]->Clear(0.f);
                m_positions[buffer_index]->Clear(0.f);
                m_normals[buffer_index]->Clear(0.f);
                m_moments[buffer_index]->Clear(0.f);
                m_mesh_ids[buffer_index]->Clear(0.f);
            }

            m_motion_buffer = new ClwOutput(GetContext(), color_width, color_height);
            m_updated_variance = new ClwOutput(GetContext(), color_width, color_height);
            m_edge_detection = new ClwOutput(GetContext(), color_width, color_height);
            m_blending_weight_calculation = new ClwOutput(GetContext(), color_width, color_height);

            m_motion_buffer->Clear(0.f);
            m_updated_variance->Clear(0.f);
            m_edge_detection->Clear(0.f);
            m_blending_weight_calculation->Clear(0.f);

            m_buffers_width = color_width;
            m_buffers_height = color_height;
            m_buffers_initialized = true;
        }

        // Resize AOV if main buffer size has changed
        if (color_width != m_buffers_width || color_height != m_buffers_height)
        {
            for (uint32_t buffer_index = 0; buffer_index < m_num_tmp_buffers; buffer_index++)
            {
                delete m_tmp_buffers[buffer_index];
                m_tmp_buffers[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);
                m_tmp_buffers[buffer_index]->Clear(0.f);

                delete m_colors[buffer_index];
                m_colors[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);
                m_colors[buffer_index]->Clear(0.f);

                delete m_positions[buffer_index];
                m_positions[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);
                m_positions[buffer_index]->Clear(0.f);

                delete m_normals[buffer_index];
                m_normals[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);
                m_normals[buffer_index]->Clear(0.f);

                delete m_moments[buffer_index];
                m_moments[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);
                m_moments[buffer_index]->Clear(0.f);

                delete m_mesh_ids[buffer_index];
                m_mesh_ids[buffer_index] = new ClwOutput(GetContext(), color_width, color_height);
                m_mesh_ids[buffer_index]->Clear(0.f);
            }

            delete m_motion_buffer;
            delete m_updated_variance;
            delete m_edge_detection;
            delete m_blending_weight_calculation;


            m_motion_buffer = new ClwOutput(GetContext(), color_width, color_height);
            m_updated_variance = new ClwOutput(GetContext(), color_width, color_height);
            m_edge_detection = new ClwOutput(GetContext(), color_width, color_height);
            m_blending_weight_calculation = new ClwOutput(GetContext(), color_width, color_height);

            m_motion_buffer->Clear(0.f);
            m_updated_variance->Clear(0.f);
            m_edge_detection->Clear(0.f);
            m_blending_weight_calculation->Clear(0.f);

            m_buffers_width = color_width;
            m_buffers_height = color_height;
        }

        // Copy buffers
        {
            auto copy_buffers_kernel = GetKernel("CopyBuffers_main");

            int argc = 0;

            // Set kernel parameters
            copy_buffers_kernel.SetArg(argc++, color->data());
            copy_buffers_kernel.SetArg(argc++, position->data());
            copy_buffers_kernel.SetArg(argc++, normal->data());
            copy_buffers_kernel.SetArg(argc++, mesh_id->data());
            copy_buffers_kernel.SetArg(argc++, color->width());
            copy_buffers_kernel.SetArg(argc++, color->height());
            copy_buffers_kernel.SetArg(argc++, m_colors[m_current_buffer_index]->data());
            copy_buffers_kernel.SetArg(argc++, m_positions[m_current_buffer_index]->data());
            copy_buffers_kernel.SetArg(argc++, m_normals[m_current_buffer_index]->data());
            copy_buffers_kernel.SetArg(argc++, m_mesh_ids[m_current_buffer_index]->data());

            // Run shading kernel
            {
                size_t gs[] = { static_cast<size_t>((output.width() + 7) / 8 * 8), static_cast<size_t>((output.height() + 7) / 8 * 8) };
                size_t ls[] = { 8, 8 };

                GetContext().Launch2D(0, gs, ls, copy_buffers_kernel);
            }
        }

        // Generate screen space motion blur buffer
        {
            auto generate_motion_kernel = GetKernel("WaveletGenerateMotionBuffer_main");

            int argc = 0;

            // Set kernel parameters
            generate_motion_kernel.SetArg(argc++, m_positions[m_current_buffer_index]->data());
            generate_motion_kernel.SetArg(argc++, color->width());
            generate_motion_kernel.SetArg(argc++, color->height());
            generate_motion_kernel.SetArg(argc++, m_view_proj_buffer);
            generate_motion_kernel.SetArg(argc++, m_prev_view_proj_buffer);
            generate_motion_kernel.SetArg(argc++, m_motion_buffer->data());

            // Run shading kernel
            {
                size_t gs[] = { static_cast<size_t>((output.width() + 7) / 8 * 8), static_cast<size_t>((output.height() + 7) / 8 * 8) };
                size_t ls[] = { 8, 8 };

                GetContext().Launch2D(0, gs, ls, generate_motion_kernel);
            }
        }

        // Temporal accumulation of color and moments
        {
            auto accumulation_kernel = GetKernel("TemporalAccumulation_main");

            int argc = 0;

            // Set kernel parameters
            accumulation_kernel.SetArg(argc++, m_colors[prev_buffer_index]->data());
            accumulation_kernel.SetArg(argc++, m_positions[prev_buffer_index]->data());
            accumulation_kernel.SetArg(argc++, m_normals[prev_buffer_index]->data());
            accumulation_kernel.SetArg(argc++, m_colors[m_current_buffer_index]->data());
            accumulation_kernel.SetArg(argc++, m_positions[m_current_buffer_index]->data());
            accumulation_kernel.SetArg(argc++, m_normals[m_current_buffer_index]->data());
            accumulation_kernel.SetArg(argc++, m_motion_buffer->data());
            accumulation_kernel.SetArg(argc++, m_moments[prev_buffer_index]->data());
            accumulation_kernel.SetArg(argc++, m_moments[m_current_buffer_index]->data());
            accumulation_kernel.SetArg(argc++, m_mesh_ids[m_current_buffer_index]->data());
            accumulation_kernel.SetArg(argc++, m_mesh_ids[prev_buffer_index]->data());
            accumulation_kernel.SetArg(argc++, m_buffers_width);
            accumulation_kernel.SetArg(argc++, m_buffers_height);

            // Run shading kernel
            {
                size_t gs[] = { static_cast<size_t>((output.width() + 7) / 8 * 8), static_cast<size_t>((output.height() + 7) / 8 * 8) };
                size_t ls[] = { 8, 8 };

                GetContext().Launch2D(0, gs, ls, accumulation_kernel);
            }
        }

        {
            auto copy_buffer_kernel = GetKernel("CopyBuffer_main");

            int argc = 0;

            // Set kernel parameters
            copy_buffer_kernel.SetArg(argc++, m_colors[m_current_buffer_index]->data());
            copy_buffer_kernel.SetArg(argc++, m_tmp_buffers[0]->data());
            copy_buffer_kernel.SetArg(argc++, m_buffers_width);
            copy_buffer_kernel.SetArg(argc++, m_buffers_height);

            // Run shading kernel
            {
                size_t gs[] = { static_cast<size_t>((output.width() + 7) / 8 * 8), static_cast<size_t>((output.height() + 7) / 8 * 8) };
                size_t ls[] = { 8, 8 };

                GetContext().Launch2D(0, gs, ls, copy_buffer_kernel);
            }
        }

        {
            auto filter_kernel = GetKernel("WaveletFilter_main");
            auto update_variance_kernel = GetKernel("UpdateVariance_main");

            Baikal::ClwOutput* current_input = nullptr;
            Baikal::ClwOutput* current_output = nullptr;
            Baikal::ClwOutput* current_variance = nullptr;

            for (uint32_t pass_index = 0; pass_index < m_max_wavelet_passes; pass_index++)
            {
                if (pass_index == 0)
                {
                    // Result of first wavelet pass goes to color buffer for next frame
                    current_input = m_tmp_buffers[pass_index % m_num_tmp_buffers];
                    current_output = m_max_wavelet_passes == 1 ? out_color : m_colors[m_current_buffer_index];
                    current_variance = m_moments[m_current_buffer_index];
                }
                else
                {
                    // Last pass goes to output buffer
                    current_input = pass_index == 1 ? m_colors[m_current_buffer_index] : m_tmp_buffers[pass_index % m_num_tmp_buffers];
                    current_output = m_tmp_buffers[(pass_index + 1) % m_num_tmp_buffers];
                    current_variance = m_updated_variance;
                }

                const int step_width = 1 << pass_index;

                int argc = 0;

                // Set kernel parameters
                filter_kernel.SetArg(argc++, current_input->data());
                filter_kernel.SetArg(argc++, m_normals[m_current_buffer_index]->data());
                filter_kernel.SetArg(argc++, m_positions[m_current_buffer_index]->data());
                filter_kernel.SetArg(argc++, current_variance->data());
                filter_kernel.SetArg(argc++, albedo->data());

                filter_kernel.SetArg(argc++, color->width());
                filter_kernel.SetArg(argc++, color->height());
                filter_kernel.SetArg(argc++, step_width);
                filter_kernel.SetArg(argc++, sigma_color);
                filter_kernel.SetArg(argc++, sigma_position);
                filter_kernel.SetArg(argc++, current_output->data());

                // Run wavelet filter kernel
                {
                    size_t gs[] = { static_cast<size_t>((output.width() + 7) / 8 * 8), static_cast<size_t>((output.height() + 7) / 8 * 8) };
                    size_t ls[] = { 8, 8 };

                    GetContext().Launch2D(0, gs, ls, filter_kernel);
                }

                argc = 0;

                update_variance_kernel.SetArg(argc++, current_output->data());
                update_variance_kernel.SetArg(argc++, m_positions[m_current_buffer_index]->data());
                update_variance_kernel.SetArg(argc++, m_normals[m_current_buffer_index]->data());
                update_variance_kernel.SetArg(argc++, color->width());
                update_variance_kernel.SetArg(argc++, color->height());
                update_variance_kernel.SetArg(argc++, m_updated_variance->data());

                // Run update variance kernel
                {
                    size_t gs[] = { static_cast<size_t>((output.width() + 7) / 8 * 8), static_cast<size_t>((output.height() + 7) / 8 * 8) };
                    size_t ls[] = { 8, 8 };

                    GetContext().Launch2D(0, gs, ls, update_variance_kernel);
                }
            }

            int argc = 0;

            auto edge_detection_kernel = GetKernel("EdgeDetectionMLAA");
            edge_detection_kernel.SetArg(argc++, m_mesh_ids[m_current_buffer_index]->data());
            edge_detection_kernel.SetArg(argc++, normal->data());
            edge_detection_kernel.SetArg(argc++, color->width());
            edge_detection_kernel.SetArg(argc++, color->height());
            edge_detection_kernel.SetArg(argc++, m_edge_detection->data());

            // Run edge detection kernel
            {
                size_t gs[] = { static_cast<size_t>((output.width() + 7) / 8 * 8), static_cast<size_t>((output.height() + 7) / 8 * 8) };
                size_t ls[] = { 8, 8 };

                GetContext().Launch2D(0, gs, ls, edge_detection_kernel);
            }

            argc = 0;

            auto blending_weight_calclulation_kernel = GetKernel("BlendingWeightCalculationMLAA");
            blending_weight_calclulation_kernel.SetArg(argc++, m_edge_detection->data());
            blending_weight_calclulation_kernel.SetArg(argc++, m_area_map_buffer);
            blending_weight_calclulation_kernel.SetArg(argc++, color->width());
            blending_weight_calclulation_kernel.SetArg(argc++, color->height());
            blending_weight_calclulation_kernel.SetArg(argc++, m_blending_weight_calculation->data());

            // Run blending weight kernel
            {
                size_t gs[] = { static_cast<size_t>((output.width() + 7) / 8 * 8), static_cast<size_t>((output.height() + 7) / 8 * 8) };
                size_t ls[] = { 8, 8 };

                GetContext().Launch2D(0, gs, ls, blending_weight_calclulation_kernel);
            }

            argc = 0;

            auto neighborhood_blending_kernel = GetKernel("NeighborhoodBlendingMLAA");
            neighborhood_blending_kernel.SetArg(argc++, current_output->data());
            neighborhood_blending_kernel.SetArg(argc++, m_blending_weight_calculation->data());
            neighborhood_blending_kernel.SetArg(argc++, color->width());
            neighborhood_blending_kernel.SetArg(argc++, color->height());
            neighborhood_blending_kernel.SetArg(argc++, out_color->data());

            // Run neighborhood blending kernel
            {
                size_t gs[] = { static_cast<size_t>((output.width() + 7) / 8 * 8), static_cast<size_t>((output.height() + 7) / 8 * 8) };
                size_t ls[] = { 8, 8 };

                GetContext().Launch2D(0, gs, ls, neighborhood_blending_kernel);
            }
        }
    }

    inline void WaveletDenoiser::Update(PerspectiveCamera* camera)
    {
        m_prev_view_proj = m_view_proj;

        const float focal_length = camera->GetFocalLength();
        const RadeonRays::float2 sensor_size = camera->GetSensorSize();

        RadeonRays::float2 z_range = camera->GetDepthRange();

        // Nan-avoidance in perspective matrix
        z_range.x = std::max(z_range.x, std::numeric_limits<float>::epsilon());

        const float fovy = atan(sensor_size.y / (2.0f * focal_length));

        const RadeonRays::float3 up = camera->GetUpVector();
        const RadeonRays::float3 right = -camera->GetRightVector();
        const RadeonRays::float3 forward = camera->GetForwardVector();
        const RadeonRays::float3 pos = camera->GetPosition();

        const RadeonRays::matrix proj = RadeonRays::perspective_proj_fovy_rh_gl(fovy, camera->GetAspectRatio(), z_range.x, z_range.y);
        const RadeonRays::float3 ip = RadeonRays::float3(-dot(right, pos), -dot(up, pos), -dot(forward, pos));

        const RadeonRays::matrix view = RadeonRays::matrix(right.x, right.y, right.z, ip.x,
            up.x, up.y, up.z, ip.y,
            forward.x, forward.y, forward.z, ip.z,
            0.0f, 0.0f, 0.0f, 1.0f);

        m_view_proj = proj * view;

        GetContext().WriteBuffer(0, m_view_proj_buffer, &m_view_proj.m[0][0], 16).Wait();
        GetContext().WriteBuffer(0, m_prev_view_proj_buffer, &m_prev_view_proj.m[0][0], 16).Wait();
    }
}
