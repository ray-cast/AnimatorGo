#ifndef OCTOON_VIDEO_RTX_MANAGER_H_
#define OCTOON_VIDEO_RTX_MANAGER_H_

#include <vector>
#include <memory>

#include <GL/glew.h>

#include "clw_output.h"
#include "clw_pipeline.h"
#include "clw_scene_controller.h"
#include "clw_render_factory.h"

#include <octoon/camera/camera.h>
#include <octoon/video/render_scene.h>

namespace octoon::video
{
	enum DeviceType
	{
		kPrimary,
		kSecondary
	};

	enum Mode
	{
		kUseAll,
		kUseGpus,
		kUseSingleGpu,
		kUseSingleCpu,
		kUseCpus
	};

	class RtxManager
	{
	public:
		RtxManager();

		void setRenderScene(RenderScene* scene) noexcept;
		const RenderScene* getRenderScene() const noexcept;

		void render(RenderScene* scene);

	private:
		void prepareScene(RenderScene* scene) noexcept;
		void createFrameBufferFromGLTexture(GLenum target, GLint miplevel, GLuint texture);

	private:
		struct Config
		{
			DeviceType type;
			std::unique_ptr<octoon::video::Pipeline> pipeline;
			std::unique_ptr<octoon::video::SceneController> controller;
			std::unique_ptr<octoon::video::RenderFactory> factory;
			CLWContext context;
			bool caninterop;
		};

		RenderScene* scene_;
		std::vector<Config> configs_;
	};
}

#endif
