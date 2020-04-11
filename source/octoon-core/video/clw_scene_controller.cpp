#include "clw_scene_controller.h"
#include <octoon/camera/perspective_camera.h>

namespace octoon::video
{
	ClwSceneController::ClwSceneController(CLWContext context)
		: context_(context)
	{
	}

	void
	ClwSceneController::compileScene(RenderScene* scene) noexcept
	{
		auto iter = sceneCache_.find(scene);
		if (iter == sceneCache_.cend())
		{
			auto clwscene = std::make_unique<ClwScene>(this->context_);
			this->updateCamera(scene, *clwscene);
			sceneCache_[scene] = std::move(clwscene);
		}
	}

	CompiledScene&
	ClwSceneController::getCachedScene(const RenderScene* scene) const noexcept(false)
	{
		auto iter = sceneCache_.find(scene);
		if (iter != sceneCache_.cend())
			return *iter->second.get();
		else
			throw std::runtime_error("Scene has not been compiled");
	}

	void
	ClwSceneController::updateCamera(const RenderScene* scene, ClwScene& out) const
	{
		if (out.camera.GetElementCount() == 0)
			out.camera = context_.CreateBuffer<ClwScene::Camera>(scene->getCameras().size(), CL_MEM_READ_ONLY);

		ClwScene::Camera* data = nullptr;
		context_.MapBuffer(0, out.camera, CL_MAP_WRITE, &data).Wait();

		for (std::size_t i = 0; i < scene->getCameras().size(); i++)
		{
			auto camera = scene->getCameras()[i];

			data[i].forward = RadeonRays::float3(camera->getForward().x, camera->getForward().y, camera->getForward().z);
			data[i].up = RadeonRays::float3(camera->getUp().x, camera->getUp().y, camera->getUp().z);
			data[i].right = RadeonRays::float3(camera->getRight().x, camera->getRight().y, camera->getRight().z);
			data[i].p = RadeonRays::float3(camera->getTranslate().x, camera->getTranslate().y, camera->getTranslate().z);
			data[i].aspect_ratio = float(camera->getPixelViewport().width) / camera->getPixelViewport().height;
            //data->dim = camera->GetSensorSize();
            //data->zcap = camera->GetDepthRange();

            if (camera->isA<camera::PerspectiveCamera>())
            {
                auto physical_camera = camera->downcast<camera::PerspectiveCamera>();
				data[i].aperture = physical_camera->getAperture();
                auto filmSize_ = 36.0f;
                auto perspective = camera->downcast<camera::PerspectiveCamera>();
                auto ratio = std::tan(math::radians(perspective->getAperture()) * 0.5f) * 2.0f;
                auto focalLength = filmSize_ / ratio;

				data[i].focal_length = focalLength;
				data[i].focus_distance = 1.0f;
            }
		}

		context_.UnmapBuffer(0, out.camera, data);
	}

    void
    ClwSceneController::updateIntersector(const RenderScene* scene, ClwScene& out) const
    {
    }
}