
/**********************************************************************
 Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
 
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

#include "render_factory.h"
#include "radeon_rays_cl.h"
#include "CLW.h"

#include "SceneGraph/clwscene.h"
#include "Utils/cl_program_manager.h"

#include <memory>
#include <string>


namespace Baikal
{
    /**
     \brief RenderFactory class is in charge of render entities creation.
     
     \details RenderFactory makes sure renderer objects are compatible between
     each other since many of them might use either CPU or GPU implementation.
     Entities create via the same factory are known to be compatible.
     */
    class ClwRenderFactory : public RenderFactory<ClwScene>
    {
    public:
        ClwRenderFactory(CLWContext context, std::string const& cache_path="");

        // Create a renderer of specified type
        std::unique_ptr<Renderer> 
            CreateRenderer(RendererType type) const override;
        // Create an output of specified type
        std::unique_ptr<Output> 
            CreateOutput(std::uint32_t w, std::uint32_t h) const override;
        // Create post effect of specified type
        std::unique_ptr<PostEffect> 
            CreatePostEffect(PostEffectType type) const override;

        std::unique_ptr<SceneController<ClwScene>>
            CreateSceneController() const override;

    private:
        CLWContext m_context;
        std::string m_cache_path;
        CLProgramManager m_program_manager;

        using RadeonRaysInstanceDelete = decltype(RadeonRays::IntersectionApi::Delete);

        std::shared_ptr<RadeonRays::IntersectionApi> m_intersector;
    };
}
