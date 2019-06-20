#include "adaptive_renderer.h"
#include "Output/clwoutput.h"

namespace Baikal
{
    
    AdaptiveRenderer::AdaptiveRenderer(
        CLWContext context,
        const CLProgramManager *program_manager,
        std::unique_ptr<Estimator> estimator
    ) : MonteCarloRenderer(context, program_manager, std::move(estimator))
    {
        auto samples_buffer_size = GetEstimator().GetWorkBufferSize();
        m_sample_buffer = GetContext().CreateBuffer<float3>(samples_buffer_size, CL_MEM_READ_WRITE);
    }

    void AdaptiveRenderer::Clear(RadeonRays::float3 const& val,
        Output& output) const
    {
        MonteCarloRenderer::Clear(val, output);

        GetContext().FillBuffer(0u, m_variance_buffer, 0.f, m_variance_buffer.GetElementCount()).Wait();
    }

    // Render single tile
    void AdaptiveRenderer::RenderTile(ClwScene const& scene,
        RadeonRays::int2 const& tile_origin,
        RadeonRays::int2 const& tile_size)
    {
        // Number of rays to generate
        auto output = static_cast<ClwOutput*>(GetOutput(OutputType::kColor));
        auto width = output->width();
        auto height = output->height();

        GetContext().FillBuffer(0u, m_sample_buffer, float3(), m_sample_buffer.GetElementCount()).Wait();

        if (output)
        {
            auto num_rays = tile_size.x * tile_size.y;
            auto output_size = int2(width, height);

            if (m_sample_counter < 32)
            {
                MonteCarloRenderer::GenerateTileDomain(output_size, tile_origin, tile_size);
            }
            else
            {
                GenerateTileDomain(output_size, tile_origin, tile_size);
            }

            GeneratePrimaryRays(scene, *output, tile_size);

            m_estimator->Estimate(
                scene,
                num_rays,
                Estimator::QualityLevel::kStandard,
                m_sample_buffer,
                false,
                true
            );

            AccumulateSamples(m_sample_buffer, output->data(), num_rays);

            if (m_sample_counter > 0 && m_sample_counter % 32 == 0)
            {
                auto variance_buffer_size = m_variance_buffer.GetElementCount();
                EstimateVariance(output->data(), output->width(), output->height());

                std::vector<float> probabilities(variance_buffer_size);
                GetContext().ReadBuffer(0u, m_variance_buffer, &probabilities[0], variance_buffer_size).Wait();

                m_tile_distribution.Set(&probabilities[0],
                                        (std::uint32_t)variance_buffer_size);

                UpdateTileDistribution();
            }

        }

        // Check if we have other outputs, than color
        bool aov_pass_needed = (FindFirstNonZeroOutput(false) != nullptr);
        if (aov_pass_needed)
        {
            FillAOVs(scene, tile_origin, tile_size);
            GetContext().Flush(0);
        }
    }

    void AdaptiveRenderer::AccumulateSamples(
        CLWBuffer<float3> sample_buffer,
        CLWBuffer<float3> accumulation_buffer,
        std::uint32_t num_elements
    )
    {
        auto accumulate_kernel = GetKernel("AccumulateSingleSample");

        int argc = 0;
        accumulate_kernel.SetArg(argc++, sample_buffer);
        accumulate_kernel.SetArg(argc++, accumulation_buffer);
        accumulate_kernel.SetArg(argc++, m_estimator->GetOutputIndexBuffer());
        accumulate_kernel.SetArg(argc++, num_elements);

        {
            GetContext().Launch1D(0, ((num_elements + 63) / 64) * 64, 64, accumulate_kernel);
        }
    }

    void AdaptiveRenderer::EstimateVariance(
        CLWBuffer<float3> accumulation_buffer,
        std::uint32_t width,
        std::uint32_t height
    )
    {
        auto estimate_kernel = GetKernel("EstimateVariance");

        int argc = 0;
        estimate_kernel.SetArg(argc++, accumulation_buffer);
        estimate_kernel.SetArg(argc++, m_variance_buffer);
        estimate_kernel.SetArg(argc++, width);
        estimate_kernel.SetArg(argc++, height);

        // Run shading kernel
        {
            size_t gs[] = { static_cast<size_t>((width + 15) / 16 * 16), static_cast<size_t>((width + 15) / 16 * 16) };
            size_t ls[] = { 16, 16 };

            GetContext().Launch2D(0, gs, ls, estimate_kernel);
        }
    }

    void AdaptiveRenderer::SetOutput(OutputType type, Output* output)
    {
        // Intermediate variance buffer
        MonteCarloRenderer::SetOutput(type, output);

        if (output)
        {
            auto width = output->width();
            auto height = output->height();

            auto variance_buffer_size = ((width + 15) / 16) * ((height + 15) / 16);
            m_variance_buffer = GetContext().CreateBuffer<float>(variance_buffer_size, CL_MEM_READ_WRITE);

            std::vector<float> probabilities(variance_buffer_size, 1.f);
            m_tile_distribution.Set(&probabilities[0], variance_buffer_size);

            UpdateTileDistribution();
        }
    }

    void AdaptiveRenderer::UpdateTileDistribution()
    {
        // Write distribution data
        int* distribution_ptr = nullptr;
        auto num_tiles = m_variance_buffer.GetElementCount();
        auto required_size = (1 + 1 + num_tiles + num_tiles);
        if (m_tile_distribution_buffer.GetElementCount() < required_size)
        {
            m_tile_distribution_buffer = GetContext().CreateBuffer<int>(required_size, CL_MEM_READ_ONLY);
        }

        GetContext().MapBuffer(0, m_tile_distribution_buffer, CL_MAP_WRITE, &distribution_ptr).Wait();
        auto current = distribution_ptr;

        // Write the number of segments first
        *current++ = (int)m_tile_distribution.m_num_segments;

        // Then write num_segments  + 1 CDF values
        auto values = reinterpret_cast<float*>(current);
        for (auto i = 0u; i < m_tile_distribution.m_num_segments + 1; ++i)
        {
            values[i] = m_tile_distribution.m_cdf[i];
        }

        // Then write num_segments PDF values
        values += m_tile_distribution.m_num_segments + 1;

        for (auto i = 0u; i < m_tile_distribution.m_num_segments; ++i)
        {
            values[i] = m_tile_distribution.m_func_values[i] / m_tile_distribution.m_func_sum;
        }

        GetContext().UnmapBuffer(0, m_tile_distribution_buffer, distribution_ptr);
    }

    void AdaptiveRenderer::GenerateTileDomain(
        int2 const& output_size,
        int2 const& tile_origin,
        int2 const& tile_size
    )
    {
        // Fetch kernel
        CLWKernel generate_kernel = GetKernel("GenerateTileDomain_Adaptive");

        // Set kernel parameters
        int argc = 0;
        generate_kernel.SetArg(argc++, output_size.x);
        generate_kernel.SetArg(argc++, output_size.y);
        generate_kernel.SetArg(argc++, tile_origin.x);
        generate_kernel.SetArg(argc++, tile_origin.y);
        generate_kernel.SetArg(argc++, tile_size.x);
        generate_kernel.SetArg(argc++, tile_size.y);
        generate_kernel.SetArg(argc++, rand_uint());
        generate_kernel.SetArg(argc++, m_sample_counter);
        generate_kernel.SetArg(argc++, m_estimator->GetRandomBuffer(Estimator::RandomBufferType::kRandomSeed));
        generate_kernel.SetArg(argc++, m_estimator->GetRandomBuffer(Estimator::RandomBufferType::kSobolLUT));
        generate_kernel.SetArg(argc++, m_tile_distribution_buffer);
        generate_kernel.SetArg(argc++, m_estimator->GetOutputIndexBuffer());
        generate_kernel.SetArg(argc++, m_estimator->GetRayCountBuffer());

        // Run shading kernel
        {
            size_t gs[] = { static_cast<size_t>((tile_size.x + 15) / 16 * 16), static_cast<size_t>((tile_size.y + 15) / 16 * 16) };
            size_t ls[] = { 16, 16 };

            GetContext().Launch2D(0, gs, ls, generate_kernel);
        }
    }
    
}
