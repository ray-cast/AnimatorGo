#ifndef OCTOON_SCRIPTABLE_RENDERING_MATERIAL_H_
#define OCTOON_SCRIPTABLE_RENDERING_MATERIAL_H_

#include <octoon/camera/camera.h>
#include <octoon/geometry/geometry.h>
#include <octoon/material/material.h>
#include <octoon/video/scriptable_render_context.h>

namespace octoon
{
	class RenderingData;
	class OCTOON_EXPORT ScriptableRenderMaterial final
	{
	public:
		ScriptableRenderMaterial() noexcept;
		ScriptableRenderMaterial(GraphicsContextPtr&context, const MaterialPtr& material, const RenderingData& scene) noexcept;
		virtual ~ScriptableRenderMaterial() noexcept;

		const GraphicsPipelinePtr& getPipeline() const noexcept;
		const GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept;

		void update(const RenderingData& context, const Camera& camera, const Geometry& geometry) noexcept;

	private:
		void updateParameters(bool force = false) noexcept;
		void updateMaterial(GraphicsContextPtr& context, const MaterialPtr& material, const RenderingData& scene) noexcept(false);

		void setupProgram(GraphicsContextPtr& context, const MaterialPtr& material, const RenderingData& scene);
		void setupRenderState(GraphicsContextPtr& context, const MaterialPtr& material);

		void parseIncludes(std::string& shader);
		void replaceLightNums(std::string& shader, const RenderingData& parameters);

	private:
		ScriptableRenderMaterial(const ScriptableRenderMaterial&) = delete;
		ScriptableRenderMaterial& operator=(const ScriptableRenderMaterial&) = delete;

	private:
		MaterialPtr material_;

		GraphicsProgramPtr program_;
		GraphicsPipelinePtr pipeline_;
		std::shared_ptr<RenderState> renderState_;
		GraphicsDescriptorSetPtr descriptorSet_;

		GraphicsUniformSetPtr ambientLightColor_;
		GraphicsUniformSetPtr directionalLights_;
		GraphicsUniformSetPtr pointLights_;
		GraphicsUniformSetPtr spotLights_;
		GraphicsUniformSetPtr rectAreaLights_;
		GraphicsUniformSetPtr hemisphereLights_;
		GraphicsUniformSetPtr flipEnvMap_;
		GraphicsUniformSetPtr envMap_;
		GraphicsUniformSetPtr envMapIntensity_;
		GraphicsUniformSetPtr envMapOffset_;

		GraphicsUniformSetPtr viewMatrix_;
		GraphicsUniformSetPtr viewProjMatrix_;
		GraphicsUniformSetPtr normalMatrix_;
		GraphicsUniformSetPtr modelMatrix_;
		GraphicsUniformSetPtr modelViewMatrix_;
		GraphicsUniformSetPtr projectionMatrix_;

		std::vector<GraphicsUniformSetPtr> directionalShadowMaps_;
		std::vector<GraphicsUniformSetPtr> directionalShadowMatrixs_;
	};
}

#endif