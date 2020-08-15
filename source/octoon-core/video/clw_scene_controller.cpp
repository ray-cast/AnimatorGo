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
			this->updateMaterials(scene, *clwscene);
			this->updateShapes(scene, *clwscene);
			sceneCache_[scene] = std::move(clwscene);
		}
		else
		{
			this->updateCamera(scene, *(*iter).second);
			this->updateMaterials(scene, *(*iter).second);
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

	ClwScene::Material
	ClwSceneController::getMaterialIndex(const material::MaterialPtr& material) const
	{
		auto it = materialidToOffset_.find(material.get());
		if (it != materialidToOffset_.end())
			return it->second;
		else
			throw std::runtime_error("Cannot find the material");
	}

	void
	ClwSceneController::updateCamera(const RenderScene* scene, ClwScene& out) const
	{
		if (out.camera.GetElementCount() == 0)
			out.camera = context_.CreateBuffer<ClwScene::Camera>(scene->getCameras().size(), CL_MEM_READ_ONLY);

		auto camera = scene->getMainCamera();
		out.cameraType = GetCameraType(*camera);

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

		out.cameraVolumeIndex = -1;
	}

	void
	ClwSceneController::updateTextures(const RenderScene* scene, ClwScene& out)
	{
		ClwScene::Texture texture;
	}

	void
	ClwSceneController::updateMaterials(const RenderScene* scene, ClwScene& out)
	{
		std::vector<int> mat_buffer;
		mat_buffer.reserve(1024 * 1024);

		this->materialidToOffset_.clear();

		for (auto& geometry : scene->getGeometries())
		{
			if (!geometry->getVisible() || !geometry->getGlobalIllumination()) {
				continue;
			}

			for (std::size_t i = 0 ; i < geometry->getMaterials().size(); ++i)
			{
				auto& mat = geometry->getMaterial(i);
				if (mat->isInstanceOf<material::MeshStandardMaterial>())
				{
					auto standard = mat->downcast<material::MeshStandardMaterial>();

					ClwScene::Material material;
					material.offset = static_cast<int>(mat_buffer.size());
					material.diffuse = RadeonRays::float4(standard->getColor().x, standard->getColor().y, standard->getColor().z, 1);
					material.flags = ClwScene::BxdfFlags::kBxdfFlagsDiffuse;

					if (i == 0)
					{
						material.emissive = material.diffuse * 10;
						material.flags = ClwScene::BxdfFlags::kBxdfFlagsEmissive;
					}

					this->materialidToOffset_[mat.get()] = material;
				}
			}
		}

		if (mat_buffer.size() >= out.materialAttributes.GetElementCount())
			out.materialAttributes = context_.CreateBuffer<std::int32_t>(std::max<std::size_t>(mat_buffer.size(), 1), CL_MEM_READ_ONLY);

		std::int32_t* materials = nullptr;

		context_.MapBuffer(0, out.materialAttributes, CL_MAP_WRITE, &materials).Wait();
		memcpy(materials, mat_buffer.data(), mat_buffer.size() * sizeof(std::int32_t));
		context_.UnmapBuffer(0, out.materialAttributes, materials);
	}

    void
    ClwSceneController::updateIntersector(const RenderScene* scene, ClwScene& out) const
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
			api_->DetachAll();

			for (auto& shape : out.isectShapes)
				api_->DeleteShape(shape);

			out.isectShapes.clear();
			out.visibleShapes.clear();

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

					out.isectShapes.push_back(shape);
					out.visibleShapes.push_back(shape);
				}

				this->api_->Commit();
			}
		}
    }

	void
	ClwSceneController::updateShapes(const RenderScene* scene, ClwScene& out) const
	{
		std::size_t num_vertices = 0;
		std::size_t num_indices = 0;
		std::size_t num_shapes = 0;

		std::size_t num_vertices_written = 0;
		std::size_t num_normals_written = 0;
		std::size_t num_uvs_written = 0;
		std::size_t num_indices_written = 0;
		std::size_t num_shapes_written = 0;

		for (auto& geometry : scene->getGeometries())
		{
			if (!geometry->getVisible() || !geometry->getGlobalIllumination()) {
				continue;
			}

			auto& mesh = geometry->getMesh();
			num_vertices += mesh->getVertexArray().size();

			for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
			{
				num_indices += mesh->getIndicesArray(i).size();
				num_shapes++;
			}
		}

		out.vertices = context_.CreateBuffer<math::float4>(num_vertices, CL_MEM_READ_ONLY);
		out.normals = context_.CreateBuffer<math::float4>(num_vertices, CL_MEM_READ_ONLY);
		out.uvs = context_.CreateBuffer<math::float2>(num_vertices, CL_MEM_READ_ONLY);
		out.indices = context_.CreateBuffer<int>(num_indices, CL_MEM_READ_ONLY);

		out.shapes = context_.CreateBuffer<ClwScene::Shape>(num_shapes, CL_MEM_READ_ONLY);
		out.shapesAdditional = context_.CreateBuffer<ClwScene::ShapeAdditionalData>(num_shapes, CL_MEM_READ_ONLY);

		math::float4* vertices = nullptr;
		math::float4* normals = nullptr;
		math::float2* uvs = nullptr;
		int* indices = nullptr;
		ClwScene::Shape* shapes = nullptr;
		ClwScene::ShapeAdditionalData* shapesAdditional = nullptr;

		// Map arrays and prepare to write data
		context_.MapBuffer(0, out.vertices, CL_MAP_WRITE, &vertices);
		context_.MapBuffer(0, out.normals, CL_MAP_WRITE, &normals);
		context_.MapBuffer(0, out.uvs, CL_MAP_WRITE, &uvs);
		context_.MapBuffer(0, out.indices, CL_MAP_WRITE, &indices);
		context_.MapBuffer(0, out.shapes, CL_MAP_WRITE, &shapes).Wait();
		context_.MapBuffer(0, out.shapesAdditional, CL_MAP_WRITE, &shapesAdditional).Wait();

		std::size_t id = 1;

		for (auto& geometry : scene->getGeometries())
		{
			if (!geometry->getVisible() || !geometry->getGlobalIllumination()) {
				continue;
			}

			auto& mesh = geometry->getMesh();

			auto mesh_vertex_array = mesh->getVertexArray().data();
			auto mesh_num_vertices = mesh->getVertexArray().size();

			auto mesh_normal_array = mesh->getNormalArray().data();
			auto mesh_num_normals = mesh->getNormalArray().size();

			auto mesh_uv_array = mesh->getTexcoordArray().data();
			auto mesh_num_uvs = mesh->getTexcoordArray().size();

			for (auto i = 0; i < mesh_num_vertices; i++)
			{
				vertices[num_vertices_written + i].set(mesh_vertex_array[i]);
				normals[num_normals_written + i].set(mesh_normal_array[i]);
			}

			std::copy(mesh_uv_array, mesh_uv_array + mesh_num_uvs, uvs + num_uvs_written);

			for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
			{
				ClwScene::Shape shape;
				shape.id = id++;
				shape.startvtx = static_cast<int>(num_vertices_written);
				shape.startidx = static_cast<int>(num_indices_written);

				auto transform = geometry->getTransform();
				shape.transform.m0 = { transform.a1, transform.a2, transform.a3, transform.a4 };
				shape.transform.m1 = { transform.b1, transform.b2, transform.b3, transform.b4 };
				shape.transform.m2 = { transform.c1, transform.c2, transform.c3, transform.c4 };
				shape.transform.m3 = { transform.d1, transform.d2, transform.d3, transform.d4 };

				shape.linearvelocity = float3(0.0f, 0.f, 0.f);
				shape.angularvelocity = float3(0.f, 0.f, 0.f, 1.f);
				shape.material = this->getMaterialIndex(geometry->getMaterial(i));
				shape.volume_idx = 0;

				auto mesh_index_array = mesh->getIndicesArray(i).data();
				auto mesh_num_indices = mesh->getIndicesArray(i).size();

				std::copy(mesh_index_array, mesh_index_array + mesh_num_indices, indices + num_indices_written);
				num_indices_written += mesh_num_indices;

				shapes[num_shapes_written++] = shape;
			}

			num_vertices_written += mesh_num_vertices;
			num_normals_written += mesh_num_normals;
			num_uvs_written += mesh_num_uvs;
		}

		context_.UnmapBuffer(0, out.vertices, vertices);
		context_.UnmapBuffer(0, out.normals, normals);
		context_.UnmapBuffer(0, out.uvs, uvs);
		context_.UnmapBuffer(0, out.indices, indices);
		context_.UnmapBuffer(0, out.shapes, shapes).Wait();
		context_.UnmapBuffer(0, out.shapesAdditional, shapesAdditional).Wait();

		this->updateIntersector(scene, out);
	}
}