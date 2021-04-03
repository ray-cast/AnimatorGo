#ifndef OCTOON_VIDEO_FORWARD_MATERIAL_H_
#define OCTOON_VIDEO_FORWARD_MATERIAL_H_

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
		ScriptableRenderMaterial(ScriptableRenderContext& context, const MaterialPtr& material, const RenderingData& scene) noexcept;
		virtual ~ScriptableRenderMaterial() noexcept;

		const hal::GraphicsPipelinePtr& getPipeline() const noexcept;
		const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept;

		void update(const RenderingData& context, const Camera& camera, const Geometry& geometry) noexcept;

	private:
		void updateParameters(bool force = false) noexcept;
		void updateMaterial(ScriptableRenderContext& context, const MaterialPtr& material, const RenderingData& scene) noexcept(false);

		void setupProgram(ScriptableRenderContext& context, const MaterialPtr& material, const RenderingData& scene);
		void setupRenderState(ScriptableRenderContext& context, const MaterialPtr& material);

		void parseIncludes(std::string& shader);
		void replaceLightNums(std::string& shader, const RenderingData& parameters);

	private:
		ScriptableRenderMaterial(const ScriptableRenderMaterial&) = delete;
		ScriptableRenderMaterial& operator=(const ScriptableRenderMaterial&) = delete;

	private:
		MaterialPtr material_;

		hal::GraphicsProgramPtr program_;
		hal::GraphicsPipelinePtr pipeline_;
		hal::GraphicsStatePtr renderState_;
		hal::GraphicsDescriptorSetPtr descriptorSet_;

		hal::GraphicsUniformSetPtr ambientLightColor_;
		hal::GraphicsUniformSetPtr directionalLights_;
		hal::GraphicsUniformSetPtr pointLights_;
		hal::GraphicsUniformSetPtr spotLights_;
		hal::GraphicsUniformSetPtr rectAreaLights_;
		hal::GraphicsUniformSetPtr hemisphereLights_;
		hal::GraphicsUniformSetPtr flipEnvMap_;
		hal::GraphicsUniformSetPtr envMap_;
		hal::GraphicsUniformSetPtr envMapIntensity_;
		hal::GraphicsUniformSetPtr envMapOffset_;

		hal::GraphicsUniformSetPtr viewMatrix_;
		hal::GraphicsUniformSetPtr viewProjMatrix_;
		hal::GraphicsUniformSetPtr normalMatrix_;
		hal::GraphicsUniformSetPtr modelMatrix_;
		hal::GraphicsUniformSetPtr modelViewMatrix_;
		hal::GraphicsUniformSetPtr projectionMatrix_;

		std::vector<hal::GraphicsUniformSetPtr> directionalShadowMaps_;
		std::vector<hal::GraphicsUniformSetPtr> directionalShadowMatrixs_;
	};
}

#endif