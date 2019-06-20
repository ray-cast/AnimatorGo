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

/**
 \file renderer.h
 \author Dmitry Kozlov
 \version 1.0
 \brief Contains declaration of Baikal::Renderer class, core interface
        representing representing the renderer.
 */
#pragma once

#include "math/float3.h"
#include "math/int2.h"
#include <cstdint>
#include <array>
#include <stdexcept>

namespace Baikal
{
    class Output;
    struct ClwScene;

    /**
     \brief Interface for the renderer.

     Renderer implemenation is taking the scene and producing its image into
     an output surface.
     */
    class Renderer
    {
    public:
        enum class OutputType
        {
            // Multi-pass outputs that will
            // be rendered in Estimator kernel
            kColor = 0,
            kOpacity,
            kVisibility,
            kMaxMultiPassOutput,
            // Single-pass outputs that will
            // be rendered in AOV kernel
            kWorldPosition,
            kWorldShadingNormal,
            kViewShadingNormal,
            kWorldGeometricNormal,
            kUv,
            kWireframe,
            kAlbedo,
            kWorldTangent,
            kWorldBitangent,
            kGloss,
            kMeshID,
            kGroupID,
            kBackground,
            kDepth,
            kShapeId,
            kMax
        };

        Renderer();
        virtual ~Renderer() = default;

        /**
         \brief Clear output surface using given value.

         \param val Value to clear to
         \param output Output to clear
         */
        virtual 
        void Clear(RadeonRays::float3 const& val, Output& output) const = 0;

        /**
         \brief Render single iteration.

         \param scene Scene to render
         */
        virtual
        void Render(ClwScene const& scene) = 0;

        /**
        \brief Render single iteration.

        \param scene Scene to render
        */
        virtual
        void RenderTile(ClwScene const& scene,
            RadeonRays::int2 const& tile_origin,
            RadeonRays::int2 const& tile_size) = 0;

        /**
         \brief Set the output for rendering.

         \param output The output to render into.
         */
        virtual void SetOutput(OutputType type, Output* output);

        /**
        \brief Get the output for rendering.

        \param output The output to render into.
        */
        virtual Output* GetOutput(OutputType type) const;

        /**
        \brief Set random seed value for the renderer. Renders
        with the same random seed are guaranteed to be the same.

        \param seed Seed value
        */
        virtual void SetRandomSeed(std::uint32_t seed) = 0;

        /**
            Disallow copies and moves.
         */
        Renderer(Renderer const&) = delete;
        Renderer& operator = (Renderer const&) = delete;

    private:
        std::array<Output*, static_cast<std::size_t>(OutputType::kMax)>
            m_outputs;
    };

    inline Renderer::Renderer()
    {
        std::fill(m_outputs.begin(), m_outputs.end(), nullptr);
    }

    inline void Renderer::SetOutput(OutputType type, Output* output)
    {
        auto idx = static_cast<std::size_t>(type);
        if (idx >= static_cast<std::size_t>(OutputType::kMax))
            throw std::out_of_range("Output type is out of supported range");

        m_outputs[idx] = output;
    }

    inline Output* Renderer::GetOutput(OutputType type) const
    {
        auto idx = static_cast<std::size_t>(type);
        if (idx >= static_cast<std::size_t>(OutputType::kMax))
            throw std::out_of_range("Output type is out of supported range");
        return m_outputs[idx];
    }
}
