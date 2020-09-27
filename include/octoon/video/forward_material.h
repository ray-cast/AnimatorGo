#ifndef OCTOON_VIDEO_FORWARD_MATERIAL_H_
#define OCTOON_VIDEO_FORWARD_MATERIAL_H_

#include <octoon/camera/camera.h>
#include <octoon/geometry/geometry.h>
#include <octoon/material/material.h>

namespace octoon::video
{
	class ForwardScene;
	class OCTOON_EXPORT ForwardMaterial final
	{
	public:
		ForwardMaterial() noexcept;
		ForwardMaterial(const material::MaterialPtr& material, const ForwardScene& context) noexcept;
		virtual ~ForwardMaterial() noexcept;

		void setMaterial(const material::MaterialPtr& material, const ForwardScene& context) noexcept;
		const material::MaterialPtr& getMaterial() const noexcept;

		const hal::GraphicsPipelinePtr& getPipeline() const noexcept;
		const hal::GraphicsDescriptorSetPtr& getDescriptorSet() const noexcept;

		void update(const ForwardScene& context, const geometry::Geometry& geometry) noexcept;

	private:
		void updateParameters(bool force = false) noexcept;
		void updateMaterial(const material::MaterialPtr& material, const ForwardScene& context) noexcept(false);

		void setupProgram(const material::MaterialPtr& material, const ForwardScene& context);
		void setupRenderState(const material::MaterialPtr& material);

		void parseIncludes(std::string& shader);
		void replaceLightNums(std::string& shader, const ForwardScene& parameters);

	private:
		ForwardMaterial(const ForwardMaterial&) = delete;
		ForwardMaterial& operator=(const ForwardMaterial&) = delete;

	private:
		material::MaterialPtr material_;

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