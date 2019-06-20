#include "clw_render_factory.h"

#include "Output/clwoutput.h"
#include "Renderers/monte_carlo_renderer.h"
#include "Renderers/adaptive_renderer.h"
#include "Estimators/path_tracing_estimator.h"

#ifdef ENABLE_DENOISER
#include "PostEffects/bilateral_denoiser.h"
#include "PostEffects/wavelet_denoiser.h"
#endif

#include <memory>

namespace Baikal
{
    ClwRenderFactory::ClwRenderFactory(CLWContext context, std::string const& cache_path)
    : m_context(context)
    , m_cache_path(cache_path)
    , m_program_manager(cache_path)
    , m_intersector(
        CreateFromOpenClContext(
            context, 
            context.GetDevice(0).GetID(), 
            context.GetCommandQueue(0)
        )
        , RadeonRays::IntersectionApi::Delete
    )
    
    {
    }

    // Create a renderer of specified type
    std::unique_ptr<Renderer> ClwRenderFactory::CreateRenderer(
                                                    RendererType type) const
    {
        switch (type)
        {
            case RendererType::kUnidirectionalPathTracer:
                return std::unique_ptr<Renderer>(
                    new MonteCarloRenderer(
                        m_context, 
                        &m_program_manager,
                        std::make_unique<PathTracingEstimator>(m_context, m_intersector, &m_program_manager)
                        ));
            default:
                throw std::runtime_error("Renderer not supported");
        }
    }

    std::unique_ptr<Output> ClwRenderFactory::CreateOutput(std::uint32_t w,
                                                           std::uint32_t h)
                                                           const
    {
        return std::unique_ptr<Output>(new ClwOutput(m_context, w, h));
    }

    std::unique_ptr<PostEffect> ClwRenderFactory::CreatePostEffect(
                                                    PostEffectType type) const
    {
#ifdef ENABLE_DENOISER
        switch (type)
        {
            case PostEffectType::kBilateralDenoiser:
                return std::unique_ptr<PostEffect>(
                                            new BilateralDenoiser(m_context, &m_program_manager));
            case PostEffectType::kWaveletDenoiser:
                return std::unique_ptr<PostEffect>(
                                            new WaveletDenoiser(m_context, &m_program_manager));
            default:
                throw std::runtime_error("PostEffect is not supported");
        }
#else
        throw std::runtime_error("PostEffect is not supported");
#endif
    }

    std::unique_ptr<SceneController<ClwScene>> ClwRenderFactory::CreateSceneController() const
    {
        return std::make_unique<ClwSceneController>(m_context, m_intersector.get(), &m_program_manager);
    }
}
