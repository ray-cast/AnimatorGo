#include "clw_scene_controller.h"
#include <octoon/camera/perspective_camera.h>
#include <set>

namespace octoon::video
{
	ClwSceneController::ClwSceneController(const CLWContext& context, const std::shared_ptr<RadeonRays::IntersectionApi>& api)
		: context_(context)
		, api_(api)
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
			this->updateShapes(scene, *clwscene);
			sceneCache_[scene] = std::move(clwscene);
		}
		else
		{
			this->updateCamera(scene, *(*iter).second);
			this->updateShapes(scene, *(*iter).second);
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
                auto filmSize_ = 36.0f;
                auto perspective = camera->downcast<camera::PerspectiveCamera>();
                auto ratio = std::tan(math::radians(perspective->getAperture()) * 0.5f) * 2.0f;
                auto focalLength = filmSize_ / ratio;

				data[i].aperture = perspective->getAperture();
				data[i].focal_length = focalLength;
				data[i].focus_distance = 1.0f;
            }
		}

		context_.UnmapBuffer(0, out.camera, data);
	}

    void
    ClwSceneController::updateIntersector(const RenderScene* scene, ClwScene& out) const
    {
        for (auto& shape : out.isect_shapes)
        {
            api_->DetachShape(shape);
            api_->DeleteShape(shape);
        }

        out.isect_shapes.clear();
        out.visible_shapes.clear();

        int id = 1;
        for (auto& geometry : scene->getGeometries())
        {
			auto& mesh = geometry->getMesh();
			for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
			{
				auto shape = api_->CreateMesh(
					(float*)mesh->getVertexArray().data(),
					static_cast<int>(mesh->getVertexArray().size() / 3),
					sizeof(float3),
					reinterpret_cast<int const*>(mesh->getIndicesArray(i).data()),
					0,
					nullptr,
					static_cast<int>(mesh->getIndicesArray(i).size() / 3)
				);

				auto transform = geometry->getTransform();
				auto transformInverse = geometry->getTransformInverse();

				RadeonRays::matrix m(
					transform.a1, transform.a2, transform.a3, transform.a4,
					transform.b1, transform.b2, transform.b3, transform.b4,
					transform.c1, transform.c2, transform.c3, transform.c4,
					transform.d1, transform.d2, transform.d3, transform.d4);

				RadeonRays::matrix minv(
					transformInverse.a1, transformInverse.a2, transformInverse.a3, transformInverse.a4,
					transformInverse.b1, transformInverse.b2, transformInverse.b3, transformInverse.b4,
					transformInverse.c1, transformInverse.c2, transformInverse.c3, transformInverse.c4,
					transformInverse.d1, transformInverse.d2, transformInverse.d3, transformInverse.d4);

				shape->SetId(id++);
				shape->SetTransform(m, inverse(m));

				out.isect_shapes.push_back(shape);
				out.visible_shapes.push_back(shape);
			}
        }
    }

	void
	ClwSceneController::updateShapes(const RenderScene* scene, ClwScene& out) const
	{
		bool dirty = false;
		for (auto& geometry : scene->getGeometries())
		{
			if (geometry->isDirty())
			{
				dirty = true;
				break;
			}
		}

		if (dirty)
		{
			this->updateIntersector(scene, out);
		}
	}
}