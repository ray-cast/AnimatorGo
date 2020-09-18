#include "clw_scene_controller.h"
#include <octoon/camera/perspective_camera.h>
#include <octoon/camera/ortho_camera.h>
#include <octoon/light/point_light.h>
#include <octoon/light/spot_light.h>
#include <octoon/light/directional_light.h>
#include <octoon/light/environment_light.h>
#include <set>

namespace octoon::video
{
	static std::size_t align16(std::size_t value)
	{
		return (value + 0xF) / 0x10 * 0x10;
	}

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

	static std::size_t GetTextureSize(const hal::GraphicsTextureDesc& desc)
	{
		switch (desc.getTexFormat())
		{
		case hal::GraphicsFormat::R8G8B8SRGB:
		case hal::GraphicsFormat::R8G8B8SNorm:
		case hal::GraphicsFormat::R8G8B8UNorm:
			return align16(desc.getWidth() * desc.getHeight() * sizeof(math::uchar4));
		case hal::GraphicsFormat::R8G8B8A8SRGB:
		case hal::GraphicsFormat::R8G8B8A8SNorm:
		case hal::GraphicsFormat::R8G8B8A8UNorm:
			return align16(desc.getStreamSize());
		case hal::GraphicsFormat::R16G16B16A16SFloat:
			return align16(desc.getStreamSize());
		case hal::GraphicsFormat::R32G32B32A32SFloat:
			return align16(desc.getStreamSize());
		case hal::GraphicsFormat::R32G32B32SFloat:
			return align16(desc.getWidth() * desc.getHeight() * sizeof(math::float4));
		default:
			assert(false);
			return 0;
		}
	}

	static ClwScene::TextureFormat GetTextureFormat(hal::GraphicsFormat texture)
	{
		switch (texture)
		{
		case hal::GraphicsFormat::R8G8B8SRGB:
		case hal::GraphicsFormat::R8G8B8SNorm:
		case hal::GraphicsFormat::R8G8B8UNorm:
		case hal::GraphicsFormat::R8G8B8A8SRGB:
		case hal::GraphicsFormat::R8G8B8A8SNorm:
		case hal::GraphicsFormat::R8G8B8A8UNorm:
			return ClwScene::TextureFormat::RGBA8;
		case hal::GraphicsFormat::R16G16B16A16SFloat:
			return ClwScene::TextureFormat::RGBA16;
		case hal::GraphicsFormat::R32G32B32SFloat:
			return ClwScene::TextureFormat::RGBA32;
		case hal::GraphicsFormat::R32G32B32A32SFloat:
			return ClwScene::TextureFormat::RGBA32;
		default:
			assert(false);
			return ClwScene::TextureFormat::UNKNOWN;
		}
	}

	int GetTextureIndex(Collector const& collector, const hal::GraphicsTexturePtr& texture)
	{
		return texture ? collector.GetItemIndex(texture.get()) : (-1);
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
		textureCollector.Clear();
		materialCollector.Clear();

		for (auto& light : scene->getLights())
		{
			if (light->isA<light::EnvironmentLight>()) {
				auto env = light->downcast<light::EnvironmentLight>();
				if (env->getEnvironmentMap())
					textureCollector.Collect(env->getEnvironmentMap());
			}
		}

		for (auto& geometry : scene->getGeometries())
		{
			if (!geometry->getVisible() || !geometry->getGlobalIllumination()) {
				continue;
			}

			for (std::size_t i = 0; i < geometry->getMaterials().size(); ++i)
			{
				auto& mat = geometry->getMaterial(i);
				if (mat->isInstanceOf<material::MeshStandardMaterial>())
				{
					materialCollector.Collect(mat);

					auto standard = mat->downcast<material::MeshStandardMaterial>();
					if (standard->getColorMap())
						textureCollector.Collect(standard->getColorMap());
					if (standard->getOpacityMap())
						textureCollector.Collect(standard->getOpacityMap());
					if (standard->getNormalMap())
						textureCollector.Collect(standard->getNormalMap());
					if (standard->getRoughnessMap())
						textureCollector.Collect(standard->getRoughnessMap());
					if (standard->getMetalnessMap())
						textureCollector.Collect(standard->getMetalnessMap());
					if (standard->getAnisotropyMap())
						textureCollector.Collect(standard->getAnisotropyMap());
					if (standard->getSpecularMap())
						textureCollector.Collect(standard->getSpecularMap());
					if (standard->getSheenMap())
						textureCollector.Collect(standard->getSheenMap());
					if (standard->getClearCoatMap())
						textureCollector.Collect(standard->getClearCoatMap());
					if (standard->getClearCoatRoughnessMap())
						textureCollector.Collect(standard->getClearCoatRoughnessMap());
					if (standard->getEmissiveMap())
						textureCollector.Collect(standard->getEmissiveMap());
				}
			}
		}

		textureCollector.Commit();
		materialCollector.Commit();

		auto iter = sceneCache_.find(scene);
		if (iter == sceneCache_.cend())
		{
			auto clwscene = std::make_unique<ClwScene>(this->context_);
			clwscene->dirty = true;
			this->updateCamera(scene, *clwscene);
			this->updateTextures(scene, *clwscene);
			this->updateMaterials(scene, *clwscene);
			this->updateShapes(scene, *clwscene);
			this->updateLights(scene, *clwscene);
			sceneCache_[scene] = std::move(clwscene);
		}
		else
		{
			auto& out = (*iter).second;

			bool should_update_textures = !out->texture_bundle || textureCollector.NeedsUpdate(out->texture_bundle.get(),
				[](runtime::RttiInterface* ptr)->bool
			{
				return false;
			});

			bool should_update_materials = !out->material_bundle || materialCollector.NeedsUpdate(out->material_bundle.get(),
				[](runtime::RttiInterface* ptr)->bool
				{
					auto mat = ptr->downcast<material::Material>();
					return mat->isDirty();
				});

			bool should_update_lights = false;
			for (auto& light : scene->getLights())
			{
				if (light->isDirty())
				{
					should_update_lights = true;
					break;
				}
			}

			bool should_update_shapes = should_update_materials;
			if (!should_update_shapes)
			{
				for (auto& geometry : scene->getGeometries())
				{
					if (!geometry->getVisible() || !geometry->getGlobalIllumination()) {
						continue;
					}

					if (geometry->isDirty())
					{
						should_update_shapes = true;
						break;
					}
				}
			}

			auto camera = scene->getMainCamera();
			if (camera->isDirty())
				this->updateCamera(scene, *out);

			if (should_update_textures)
				this->updateTextures(scene, *out);

			if (should_update_materials)
				this->updateMaterials(scene, *out);

			if (should_update_lights)
				this->updateLights(scene, *out);

			if (should_update_shapes | should_update_textures)
				this->updateShapes(scene, *out);

			out->dirty = camera->isDirty() | should_update_textures | should_update_materials | should_update_lights | should_update_shapes;
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

		ClwScene::Camera* data = nullptr;
		context_.MapBuffer(0, out.camera, CL_MAP_WRITE, &data).Wait();

		auto camera = scene->getMainCamera();
		auto viewport = camera->getPixelViewport();

		data[0].forward = RadeonRays::float3(camera->getForward().x, camera->getForward().y, camera->getForward().z);
		data[0].up = RadeonRays::float3(camera->getUp().x, camera->getUp().y, camera->getUp().z);
		data[0].right = RadeonRays::float3(camera->getRight().x, camera->getRight().y, camera->getRight().z);
		data[0].p = RadeonRays::float3(camera->getTranslate().x, camera->getTranslate().y, camera->getTranslate().z);
		data[0].aspect_ratio = float(viewport.width) / viewport.height;

		if (camera->isA<camera::PerspectiveCamera>())
		{
			auto filmSize_ = 36.0f;
			auto perspective = camera->downcast<camera::PerspectiveCamera>();
			auto ratio = std::tan(math::radians(perspective->getAperture()) * 0.5f) * 2.0f;
			auto focalLength = filmSize_ / ratio;

			data[0].aperture = 0;
			data[0].focal_length = focalLength;
			data[0].focus_distance = 1.0f;
			data[0].dim = RadeonRays::float2(filmSize_ * viewport.width / viewport.height, filmSize_);
			data[0].zcap = RadeonRays::float2(perspective->getNear(), perspective->getFar());
		}

		context_.UnmapBuffer(0, out.camera, data);

		out.cameraType = GetCameraType(*camera);
		out.cameraVolumeIndex = -1;
	}

	void
	ClwSceneController::WriteLight(const RenderScene* scene, const light::Light& light, void* data) const
	{
		auto clwLight = reinterpret_cast<ClwScene::Light*>(data);

		auto& translate = light.getTranslate();
		auto& direction = light.getForward();
		auto power = light.getColor() * light.getIntensity();

		if (light.isA<light::PointLight>())
		{
			clwLight->type = ClwScene::kPoint;
			clwLight->p = RadeonRays::float3(translate.x, translate.y, translate.z);
			clwLight->intensity = RadeonRays::float3(power.x, power.y, power.z);
		}
		else if (light.isA<light::DirectionalLight>())
		{
			clwLight->type = ClwScene::kDirectional;
			clwLight->d = RadeonRays::float3(direction.x, direction.y, direction.z);
			clwLight->intensity = RadeonRays::float3(power.x, power.y, power.z);
		}
		else if (light.isA<light::SpotLight>())
		{
			clwLight->type = ClwScene::kSpot;
			clwLight->p = RadeonRays::float3(translate.x, translate.y, translate.z);
			clwLight->d = RadeonRays::float3(direction.x, direction.y, direction.z);
			clwLight->intensity = RadeonRays::float3(power.x, power.y, power.z);
			clwLight->ia = light.downcast<light::SpotLight>()->getInnerCone().x;
			clwLight->oa = light.downcast<light::SpotLight>()->getOuterCone().x;
		}
		else if (light.isA<light::EnvironmentLight>())
		{
			auto& ibl = static_cast<light::EnvironmentLight const&>(light);
			clwLight->type = ClwScene::kIbl;
			clwLight->multiplier = ibl.getIntensity();
			clwLight->intensity = RadeonRays::float3(power.x, power.y, power.z);
			clwLight->tex = GetTextureIndex(textureCollector, ibl.getEnvironmentMap());
			clwLight->tex_reflection = -1;			
			clwLight->tex_refraction = -1;			
			clwLight->tex_transparency = -1;			
			clwLight->tex_background = -1;
			clwLight->ibl_mirror_x = true;
		}
	}

	void
	ClwSceneController::updateLights(const RenderScene* scene, ClwScene& out)
	{
		int numLightsWritten = 0;
		std::size_t numLights = scene->getLights().size();
		std::size_t distribution_buffer_size = (1 + 1 + numLights + numLights);

		if (numLights > out.lights.GetElementCount())
		{
			out.lights = context_.CreateBuffer<ClwScene::Light>(numLights, CL_MEM_READ_ONLY);
			out.lightDistributions = context_.CreateBuffer<int>(distribution_buffer_size, CL_MEM_READ_ONLY);
		}

		ClwScene::Light* lights = nullptr;
		context_.MapBuffer(0, out.lights, CL_MAP_WRITE, &lights).Wait();
		
		out.envmapidx = -1;

		std::vector<float> lightPower(numLights);

		for (auto& light : scene->getLights())
		{
			WriteLight(scene, *light, lights + numLightsWritten);

			if (light->isA<light::EnvironmentLight>())
				out.envmapidx = numLightsWritten;

			auto power = light->getColor() * light->getIntensity();
			lightPower[numLightsWritten] = 0.2126f * power.x + 0.7152f * power.y + 0.0722f * power.z;

			numLightsWritten++;
		}

		context_.UnmapBuffer(0, out.lights, lights);

		// Create distribution over light sources based on their power
		/*Distribution1D light_distribution(&light_power[0], (std::uint32_t)light_power.size());

		int* distribution_ptr = nullptr;
		context_.MapBuffer(0, out.lightDistributions, CL_MAP_WRITE, &distribution_ptr).Wait();
		auto current = distribution_ptr;

		*current++ = (int)light_distribution.m_num_segments;

		auto values = reinterpret_cast<float*>(current);
		for (auto i = 0u; i < light_distribution.m_num_segments + 1; ++i)
			values[i] = light_distribution.m_cdf[i];

		values += light_distribution.m_num_segments + 1;

		for (auto i = 0u; i < light_distribution.m_num_segments; ++i)
			values[i] = light_distribution.m_func_values[i] / light_distribution.m_func_sum;

		context_.UnmapBuffer(0, out.lightDistributions, distribution_ptr);*/

		out.numLights = static_cast<int>(numLightsWritten);
	}

	void
	ClwSceneController::WriteTexture(const hal::GraphicsTexture& texture, std::size_t data_offset, void* data) const
	{
		auto& desc = texture.getTextureDesc();

		auto clw_texture = reinterpret_cast<ClwScene::Texture*>(data);
		clw_texture->w = desc.getWidth();
		clw_texture->h = desc.getHeight();;
		clw_texture->d = desc.getDepth();;
		clw_texture->fmt = GetTextureFormat(desc.getTexFormat());
		clw_texture->dataoffset = static_cast<int>(data_offset);
	}

	void
	ClwSceneController::WriteTextureData(hal::GraphicsTexture& texture, void* dest) const
	{
		auto& desc = texture.getTextureDesc();
		
		switch (desc.getTexFormat())
		{
		case hal::GraphicsFormat::R8G8B8A8SRGB:
		case hal::GraphicsFormat::R8G8B8A8UNorm:
		case hal::GraphicsFormat::R16G16B16A16SFloat:
		case hal::GraphicsFormat::R32G32B32A32SFloat:
		{
			char* data = nullptr;
			if (texture.map(0, 0, desc.getWidth(), desc.getHeight(), 0, (void**)&data))
			{
				std::copy(data, data + desc.getStreamSize(), static_cast<char*>(dest));
				texture.unmap();
			}
		}
		break;
		case hal::GraphicsFormat::R8G8B8SRGB:
		case hal::GraphicsFormat::R8G8B8SNorm:
		case hal::GraphicsFormat::R8G8B8UNorm:
		{
			math::uchar3* data = nullptr;
			if (texture.map(0, 0, desc.getWidth(), desc.getHeight(), 0, (void**)&data))
			{
				for (std::size_t i = 0; i < desc.getWidth() * desc.getHeight(); i++)
					((math::uchar4*)dest)[i].set(data[i]);

				texture.unmap();
			}
		}
		break;
		case hal::GraphicsFormat::R32G32B32SFloat:
		{
			math::float3* data = nullptr;
			if (texture.map(0, 0, desc.getWidth(), desc.getHeight(), 0, (void**)&data))
			{
				for (std::size_t i = 0; i < desc.getWidth() * desc.getHeight(); i++)
					((math::float4*)dest)[i].set(data[i]);

				texture.unmap();
			}
		}
		break;
		default:
			assert(false);
		}
	}

	void
	ClwSceneController::updateTextures(const RenderScene* scene, ClwScene& out)
	{
		out.texture_bundle.reset(textureCollector.CreateBundle());

		auto numTextures = textureCollector.GetNumItems();
		if (numTextures > 0)
		{
			std::size_t numTexturesWritten = 0;
			std::size_t numTexDataBufferSize = 0;

			if (numTextures > out.textures.GetElementCount())
				out.textures = context_.CreateBuffer<ClwScene::Texture>(numTextures, CL_MEM_READ_ONLY);

			ClwScene::Texture* textures;
			context_.MapBuffer(0, out.textures, CL_MAP_WRITE, &textures).Wait();

			std::unique_ptr<Iterator> tex_iter(textureCollector.CreateIterator());
			for (; tex_iter->IsValid(); tex_iter->Next())
			{
				auto tex = tex_iter->ItemAs<hal::GraphicsTexture>();
				this->WriteTexture(*tex, numTexDataBufferSize, textures + numTexturesWritten);

				numTexturesWritten++;
				numTexDataBufferSize += GetTextureSize(tex->getTextureDesc());
			}

			context_.UnmapBuffer(0, out.textures, textures);

			if (numTexDataBufferSize > out.texturedata.GetElementCount())
				out.texturedata = context_.CreateBuffer<char>(numTexDataBufferSize, CL_MEM_READ_ONLY);

			char* data = nullptr;
			context_.MapBuffer(0, out.texturedata, CL_MAP_WRITE, &data).Wait();

			std::size_t numBytesWritten = 0;
			for (tex_iter->Reset(); tex_iter->IsValid(); tex_iter->Next())
			{
				auto tex = tex_iter->ItemAs<hal::GraphicsTexture>();
				this->WriteTextureData(*tex, data + numBytesWritten);
				numBytesWritten += GetTextureSize(tex->getTextureDesc());
			}

			context_.UnmapBuffer(0, out.texturedata, data);
		}
	}

	void
	ClwSceneController::updateMaterials(const RenderScene* scene, ClwScene& out)
	{
		out.material_bundle.reset(materialCollector.CreateBundle());

		this->materialidToOffset_.clear();

		if (materialCollector.GetNumItems() >= out.materials.GetElementCount())
			out.materials = context_.CreateBuffer<ClwScene::Material>(std::max<std::size_t>(materialCollector.GetNumItems(), 1), CL_MEM_READ_ONLY);

		ClwScene::Material* materials = nullptr;
		context_.MapBuffer(0, out.materials, CL_MAP_WRITE, &materials).Wait();

		std::unique_ptr<Iterator> mat_iter(materialCollector.CreateIterator());
		for (std::size_t i = 0; mat_iter->IsValid(); mat_iter->Next(), i++)
		{
			auto mat = mat_iter->ItemAs<material::MeshStandardMaterial>();

			auto& material = materials[i];
			material.offset = 0;
			material.flags = ClwScene::BxdfFlags::kBxdfFlagsDiffuse | ClwScene::BxdfFlags::kBxdfFlagsBrdf;
			material.disney.base_color = RadeonRays::float3(mat->getColor().x, mat->getColor().y, mat->getColor().z);
			material.disney.base_color_map_idx = GetTextureIndex(textureCollector, mat->getColorMap());
			material.disney.opacity = mat->getOpacity();
			material.disney.opacity_map_idx = GetTextureIndex(textureCollector, mat->getOpacityMap());
			material.disney.normal_map_idx = GetTextureIndex(textureCollector, mat->getNormalMap());
			material.disney.roughness = 1 - mat->getSmoothness();
			material.disney.roughness_map_idx = GetTextureIndex(textureCollector, mat->getRoughnessMap());
			material.disney.metallic = mat->getMetalness();
			material.disney.metallic_map_idx = GetTextureIndex(textureCollector, mat->getMetalnessMap());
			material.disney.anisotropy = mat->getAnisotropy();
			material.disney.anisotropy_map_idx = GetTextureIndex(textureCollector, mat->getAnisotropyMap());
			material.disney.specular = mat->getSpecular();
			material.disney.specular_map_idx = GetTextureIndex(textureCollector, mat->getSpecularMap());
			material.disney.specular_tint = 0;
			material.disney.specular_tint_map_idx = -1;
			material.disney.sheen = mat->getSheen();
			material.disney.sheen_map_idx = GetTextureIndex(textureCollector, mat->getSheenMap());
			material.disney.sheen_tint = 0;
			material.disney.sheen_tint_map_idx = -1;
			material.disney.clearcoat = mat->getClearCoat();
			material.disney.clearcoat_map_idx = GetTextureIndex(textureCollector, mat->getClearCoatMap());
			material.disney.clearcoat_roughness = mat->getClearCoatRoughness();
			material.disney.clearcoat_roughness_map_idx = GetTextureIndex(textureCollector, mat->getClearCoatRoughnessMap());
			material.disney.subsurface = mat->getSubsurface();
			material.disney.emissive = RadeonRays::float3(mat->getEmissive().x, mat->getEmissive().y, mat->getEmissive().z) * mat->getEmissiveIntensity();
			material.disney.emissive_map_idx = GetTextureIndex(textureCollector, mat->getEmissiveMap());

			this->materialidToOffset_[mat] = material;
		}

		context_.UnmapBuffer(0, out.materials, materials);
	}

    void
    ClwSceneController::updateIntersector(const RenderScene* scene, ClwScene& out) const
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

		if (num_vertices > out.vertices.GetElementCount())
			out.vertices = context_.CreateBuffer<math::float4>(num_vertices, CL_MEM_READ_ONLY);
		if (num_vertices > out.normals.GetElementCount())
			out.normals = context_.CreateBuffer<math::float4>(num_vertices, CL_MEM_READ_ONLY);
		if (num_vertices > out.uvs.GetElementCount())
			out.uvs = context_.CreateBuffer<math::float2>(num_vertices, CL_MEM_READ_ONLY);
		if (num_indices > out.indices.GetElementCount())
			out.indices = context_.CreateBuffer<int>(num_indices, CL_MEM_READ_ONLY);

		out.shapes = context_.CreateBuffer<ClwScene::Shape>(num_shapes, CL_MEM_READ_ONLY);
		out.shapesAdditional = context_.CreateBuffer<ClwScene::ShapeAdditionalData>(num_shapes, CL_MEM_READ_ONLY);

		math::float4* vertices = nullptr;
		math::float4* normals = nullptr;
		math::float2* uvs = nullptr;
		int* indices = nullptr;
		ClwScene::Shape* shapes = nullptr;
		ClwScene::ShapeAdditionalData* shapesAdditional = nullptr;

		context_.MapBuffer(0, out.vertices, CL_MAP_WRITE, &vertices);
		context_.MapBuffer(0, out.normals, CL_MAP_WRITE, &normals);
		context_.MapBuffer(0, out.uvs, CL_MAP_WRITE, &uvs);
		context_.MapBuffer(0, out.indices, CL_MAP_WRITE, &indices);
		context_.MapBuffer(0, out.shapes, CL_MAP_WRITE, &shapes).Wait();
		context_.MapBuffer(0, out.shapesAdditional, CL_MAP_WRITE, &shapesAdditional).Wait();

		int id = 1;

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