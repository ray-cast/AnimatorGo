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
#include "renderer.h"

#include "SceneGraph/clwscene.h"
#include "Controllers/clw_scene_controller.h"
#include "Utils/clw_class.h"
#include "Estimators/estimator.h"

#include "CLW.h"

#include <memory>


namespace Baikal
{
    class ClwOutput;
    struct ClwScene;
    class CLProgramManager;

    ///< Renderer implementation
    class MonteCarloRenderer : public Renderer, protected ClwClass
    {
    public:

        MonteCarloRenderer(
            CLWContext context,
            const CLProgramManager *program_manager,
            std::unique_ptr<Estimator> estimator
        );

        ~MonteCarloRenderer() = default;

        // Renderer overrides
        void Clear(RadeonRays::float3 const& val,
                   Output& output) const override;

        // Render the scene into the output
        void Render(ClwScene const& scene) override;

        // Render single tile
        void RenderTile(ClwScene const& scene,
                        RadeonRays::int2 const& tile_origin,
                        RadeonRays::int2 const& tile_size) override;

        // Set output
        void SetOutput(OutputType type, Output* output) override;

        void SetRandomSeed(std::uint32_t seed) override;

        // Interop function
        CLWKernel GetCopyKernel();
        // Add function
        CLWKernel GetAccumulateKernel();
        // Run render benchmark
        void Benchmark(ClwScene const& scene, Estimator::RayTracingStats& stats);

        // Set max number of light bounces
        void SetMaxBounces(std::uint32_t max_bounces);
        
    protected:
        void GeneratePrimaryRays(
            ClwScene const& scene,
            Output const& output,
            int2 const& tile_size,
            bool generate_at_pixel_center = false
        );

        void FillAOVs(
            ClwScene const& scene, 
            int2 const& tile_origin,
            int2 const& tile_size
        );

        virtual void GenerateTileDomain(
            int2 const& output_size,
            int2 const& tile_origin,
            int2 const& tile_size
        );

        Estimator& GetEstimator() { return *m_estimator;  }

        // Find non-zero AOV
        Output* FindFirstNonZeroOutput(bool include_multipass = true, bool include_singlepass = true) const;

        // Handler for missed rays used when scene have background override with plain image
        void HandleMissedRays(const ClwScene &scene, uint32_t w, uint32_t h,
            CLWBuffer<ray> rays, CLWBuffer<Intersection> intersections, CLWBuffer<int> pixel_indices,
            CLWBuffer<int> output_indices, std::size_t size, CLWBuffer<RadeonRays::float3> output);

    public:
        std::unique_ptr<Estimator> m_estimator;
        mutable std::uint32_t m_sample_counter;

    private:
        ClwClass m_uberv2_kernels;
    };

}
