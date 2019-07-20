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

#include "math/int2.h"
#include "monte_carlo_renderer.h"
#include "CLW.h"
#include "Utils/distribution1d.h"

#include <memory>



namespace Baikal
{
    class ClwOutput;
    struct ClwScene;
    
    ///< Renderer implementation
    class AdaptiveRenderer : public MonteCarloRenderer
    {
    public:
        AdaptiveRenderer(
            CLWContext context, 
            const CLProgramManager *program_manager,
            std::unique_ptr<Estimator> estimator
        );

        ~AdaptiveRenderer() = default;

        // Renderer overrides
        void Clear(RadeonRays::float3 const& val,
            Output& output) const override;

        // Render single tile
        void RenderTile(ClwScene const& scene,
            RadeonRays::int2 const& tile_origin,
            RadeonRays::int2 const& tile_size) override;

        // Set output
        void SetOutput(OutputType type, Output* output) override;

        // DEBUG STUFF
        CLWBuffer<float> GetVarianceBuffer() const { return m_variance_buffer; }
    protected:
        void AccumulateSamples(
            CLWBuffer<float3> sample_buffer,
            CLWBuffer<float3> accumulation_buffer,
            std::uint32_t num_elements
        );

        void EstimateVariance(
            CLWBuffer<float3> accumulation_buffer,
            std::uint32_t width,
            std::uint32_t height
        );

        void GenerateTileDomain(
            int2 const& output_size,
            int2 const& tile_origin,
            int2 const& tile_size
        ) override;

        void UpdateTileDistribution();

    private:
        mutable CLWBuffer<float> m_variance_buffer;
        mutable CLWBuffer<float3> m_sample_buffer;
        CLWBuffer<int> m_tile_distribution_buffer;
        Distribution1D m_tile_distribution;
    };
    
}
