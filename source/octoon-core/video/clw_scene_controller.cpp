#include "clw_scene_controller.h"
#include <octoon/camera/perspective_camera.h>
#include <octoon/camera/ortho_camera.h>
#include <set>

namespace octoon::video
{
	static CameraType GetCameraType(const camera::Camera& camera)
	{
		auto perspective = dynamic_cast<const camera::PerspectiveCamera*>(&camera);
		if (perspective)
		{
			return perspective->getAperture() > 0.f ? CameraType::kPhysicalPerspective : CameraType::kPerspective;
		}

		auto ortho = dynamic_cast<const camera::OrthographicCamera*>(&camera);
		if (ortho)
		{
			return CameraType::kOrthographic;
		}

		return CameraType::kPerspective;
	}

	ClwSceneController::ClwSceneController(const CLWContext& context, const std::shared_ptr<RadeonRays::IntersectionApi>& api, const CLProgramManager* program_manager)
		: context_(context)
		, api_(api)
		, programManager_(program_manager)
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

		auto camera = scene->getMainCamera();
		out.camera_type = GetCameraType(*camera);

		ClwScene::Camera* data = nullptr;
		context_.MapBuffer(0, out.camera, CL_MAP_WRITE, &data).Wait();

		auto sensorSize = camera->getPixelViewport();

		data[0].forward = RadeonRays::float3(camera->getForward().x, camera->getForward().y, camera->getForward().z);
		data[0].up = RadeonRays::float3(camera->getUp().x, camera->getUp().y, camera->getUp().z);
		data[0].right = RadeonRays::float3(camera->getRight().x, camera->getRight().y, camera->getRight().z);
		data[0].p = RadeonRays::float3(camera->getTranslate().x, camera->getTranslate().y, camera->getTranslate().z);
		data[0].aspect_ratio = float(sensorSize.width) / sensorSize.height;

        if (camera->isA<camera::PerspectiveCamera>())
        {
            auto filmSize_ = 36.0f;
            auto perspective = camera->downcast<camera::PerspectiveCamera>();
            auto ratio = std::tan(math::radians(perspective->getAperture()) * 0.5f) * 2.0f;
            auto focalLength = filmSize_ / ratio;

			data[0].aperture = 0;
			data[0].focal_length = focalLength;
			data[0].focus_distance = 1.0f;
			data[0].dim = RadeonRays::float2(filmSize_ * sensorSize.width / sensorSize.height, filmSize_);
			data[0].zcap = RadeonRays::float2(perspective->getNear(), perspective->getFar());
        }

		context_.UnmapBuffer(0, out.camera, data);
	}

    void
    ClwSceneController::updateIntersector(const RenderScene* scene, ClwScene& out) const
    {
		api_->DetachAll();

        for (auto& shape : out.isect_shapes)
            api_->DeleteShape(shape);

        out.isect_shapes.clear();
        out.visible_shapes.clear();

        int id = 1;
        for (auto& geometry : scene->getGeometries())
        {
			if (!geometry->getVisible() || !geometry->getGlobalIllumination()) {
				continue;
			}

			auto& mesh = geometry->getMesh();
			for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
			{
				auto shape = this->api_->CreateMesh(
					(float*)mesh->getVertexArray().data(),
					static_cast<int>(mesh->getVertexArray().size()),
					3 * sizeof(float),
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

				this->api_->AttachShape(shape);

				out.isect_shapes.push_back(shape);
				out.visible_shapes.push_back(shape);
			}

			this->api_->Commit();
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