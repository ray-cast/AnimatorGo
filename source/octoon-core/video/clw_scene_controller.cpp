#include "clw_scene_controller.h"
#include <octoon/camera/film_camera.h>
#include <octoon/camera/perspective_camera.h>
#include <octoon/camera/ortho_camera.h>
#include <octoon/light/point_light.h>
#include <octoon/light/spot_light.h>
#include <octoon/light/directional_light.h>
#include <octoon/light/environment_light.h>
#include <set>

namespace octoon
{
	static std::size_t align16(std::size_t value)
	{
		return (value + 0xF) / 0x10 * 0x10;
	}

	static CameraType GetCameraType(const Camera& camera)
	{
		auto perspective = dynamic_cast<const PerspectiveCamera*>(&camera);
		if (perspective)
		{
			return CameraType::kPerspective;
		}

		auto film = dynamic_cast<const FilmCamera*>(&camera);
		if (film)
		{
			return film->getAperture() > 0.f ? CameraType::kPhysicalPerspective : CameraType::kPerspective;
		}

		auto ortho = dynamic_cast<const OrthographicCamera*>(&camera);
		if (ortho)
		{
			return CameraType::kOrthographic;
		}

		return CameraType::kPerspective;
	}

	static std::size_t GetTextureSize(const Texture& texture)
	{
		switch (texture.format())
		{
		case Format::B8G8R8SRGB:
		case Format::B8G8R8SNorm:
		case Format::B8G8R8UNorm:
		case Format::R8G8B8SRGB:
		case Format::R8G8B8SNorm:
		case Format::R8G8B8UNorm:
			return align16(texture.width() * texture.height() * sizeof(math::uchar4));
		case Format::B8G8R8A8SRGB:
		case Format::B8G8R8A8SNorm:
		case Format::B8G8R8A8UNorm:
		case Format::R8G8B8A8SRGB:
		case Format::R8G8B8A8SNorm:
		case Format::R8G8B8A8UNorm:
			return align16(texture.size());
		case Format::R16G16B16A16SFloat:
			return align16(texture.size());
		case Format::R32G32B32A32SFloat:
			return align16(texture.size());
		case Format::R32G32B32SFloat:
			return align16(texture.width() * texture.height() * sizeof(math::float4));
		default:
			assert(false);
			return 0;
		}
	}

	static ClwScene::TextureFormat GetTextureFormat(Format texture)
	{
		switch (texture)
		{
		case Format::B8G8R8SRGB:
		case Format::B8G8R8SNorm:
		case Format::B8G8R8UNorm:
		case Format::B8G8R8A8SRGB:
		case Format::B8G8R8A8SNorm:
		case Format::B8G8R8A8UNorm:
			return ClwScene::TextureFormat::BGRA8;
		case Format::R8G8B8SRGB:
		case Format::R8G8B8SNorm:
		case Format::R8G8B8UNorm:
		case Format::R8G8B8A8SRGB:
		case Format::R8G8B8A8SNorm:
		case Format::R8G8B8A8UNorm:
			return ClwScene::TextureFormat::RGBA8;
		case Format::R16G16B16A16SFloat:
			return ClwScene::TextureFormat::RGBA16;
		case Format::R32G32B32SFloat:
			return ClwScene::TextureFormat::RGBA32;
		case Format::R32G32B32A32SFloat:
			return ClwScene::TextureFormat::RGBA32;
		default:
			assert(false);
			return ClwScene::TextureFormat::UNKNOWN;
		}
	}

	int GetTextureIndex(Collector const& collector, const std::shared_ptr<Texture>& texture)
	{
		return texture ? collector.GetItemIndex(texture.get()) : (-1);
	}

	ClwSceneController::ClwSceneController(const CLWContext& context, const std::shared_ptr<RadeonRays::IntersectionApi>& api, const CLProgramManager* program_manager)
		: context_(context)
		, api_(api)
		, programManager_(program_manager)
	{
		auto acc_type = "fatbvh";
		auto builder_type = "sah";

#ifdef ENABLE_RAYMASK
		api_->SetOption("bvh.force2level", 1.f);
#endif

		api_->SetOption("acc.type", acc_type);
		api_->SetOption("bvh.builder", builder_type);
		api_->SetOption("bvh.sah.num_bins", 16.f);
	}

	void
	ClwSceneController::cleanCache() noexcept
	{
		for (auto it = sceneCache_.begin(); it != sceneCache_.end();)
		{
			if ((*it).first.use_count() == 1)
				it = sceneCache_.erase(it);
			else
				++it;
		}
	}

	void
	ClwSceneController::compileScene(const std::shared_ptr<RenderScene>& scene) noexcept
	{
		textureCollector.Clear();
		materialCollector.Clear();

		for (auto& light : scene->getLights())
		{
			if (light->isA<EnvironmentLight>()) {
				auto env = light->downcast<EnvironmentLight>();
				if (env->getEnvironmentMap())
					textureCollector.Collect(env->getEnvironmentMap());
				if (env->getBackgroundMap())
					textureCollector.Collect(env->getBackgroundMap());
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
				if (!mat)
					continue;

				if (mat->isInstanceOf<MeshStandardMaterial>())
				{
					auto standard = mat->downcast<MeshStandardMaterial>();
					if (standard->getOpacity() > 0)
					{
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
						if (standard->getSubsurfaceMap())
							textureCollector.Collect(standard->getSubsurfaceMap());
						if (standard->getSubsurfaceColorMap())
							textureCollector.Collect(standard->getSubsurfaceColorMap());
						if (standard->getEmissiveMap())
							textureCollector.Collect(standard->getEmissiveMap());

						materialCollector.Collect(mat);
					}
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
				[](Object* ptr)->bool
			{
				return false;
			});

			bool should_update_materials = !out->material_bundle || materialCollector.NeedsUpdate(out->material_bundle.get(),
				[](Object* ptr)->bool
				{
					auto mat = ptr->downcast<Material>();
					return mat->isDirty();
				});

			bool should_update_lights = scene->isSceneDirty();
			if (!should_update_lights)
			{
				for (auto& light : scene->getLights())
				{
					if (light->isDirty())
					{
						should_update_lights = true;
						break;
					}
				}
			}

			bool should_update_shapes = scene->isSceneDirty() | should_update_materials;
			if (!should_update_shapes)
			{
				std::size_t num_geometries = 0;

				for (auto& geometry : scene->getGeometries())
				{
					if (!geometry->getVisible() || !geometry->getGlobalIllumination()) {
						continue;
					}

					num_geometries++;

					if (geometry->isDirty())
					{
						should_update_shapes = true;
						break;
					}
				}

				should_update_shapes |= (num_geometries != out->numGeometries);
			}

			auto camera = scene->getMainCamera();
			if (camera->isDirty())
				this->updateCamera(scene, *out);

			if (should_update_textures)
				this->updateTextures(scene, *out);

			if (should_update_materials | should_update_textures)
				this->updateMaterials(scene, *out);

			if (should_update_lights | should_update_textures)
				this->updateLights(scene, *out);

			if (should_update_shapes | should_update_textures)
				this->updateShapes(scene, *out);

			out->dirty = camera->isDirty() | should_update_textures | should_update_materials | should_update_lights | should_update_shapes;
		}
	}

	CompiledScene&
	ClwSceneController::getCachedScene(const std::shared_ptr<RenderScene>& scene) const noexcept(false)
	{
		auto iter = sceneCache_.find(scene);
		if (iter != sceneCache_.cend())
			return *iter->second.get();
		else
			throw std::runtime_error("Scene has not been compiled");
	}

	std::optional<ClwScene::Material>
	ClwSceneController::getMaterialIndex(const MaterialPtr& material) const
	{
		auto it = materialidToOffset_.find(material.get());
		if (it != materialidToOffset_.end())
			return it->second;
		else
			return std::nullopt;
	}

	void
	ClwSceneController::updateCamera(const std::shared_ptr<RenderScene>& scene, ClwScene& out) const
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

		if (camera->isA<PerspectiveCamera>())
		{
			auto filmSize_ = 36.0f;
			auto perspective = camera->downcast<PerspectiveCamera>();
			auto ratio = std::tan(math::radians(perspective->getFov()) * 0.5f) * 2.0f;
			auto focalLength = filmSize_ / ratio;

			data[0].aperture = 0;
			data[0].focal_length = focalLength;
			data[0].focus_distance = 1.0f;
			data[0].dim = RadeonRays::float2(filmSize_ * viewport.width / viewport.height, filmSize_);
			data[0].zcap = RadeonRays::float2(perspective->getNear(), perspective->getFar());
		}
		else if (camera->isA<FilmCamera>())
		{
			auto film = camera->downcast<FilmCamera>();
			auto filmSize_ = film->getFilmSize();

			data[0].aperture = film->getAperture() > 0.0f ? 1.0f / film->getAperture() : 0.0f;
			data[0].focal_length = film->getFocalLength();
			data[0].focus_distance = film->getFocalDistance();
			data[0].dim = RadeonRays::float2(filmSize_ * viewport.width / viewport.height, filmSize_);
			data[0].zcap = RadeonRays::float2(film->getNear(), film->getFar());
		}

		context_.UnmapBuffer(0, out.camera, data);

		out.cameraType = GetCameraType(*camera);
		out.cameraVolumeIndex = -1;
	}

	void
	ClwSceneController::WriteTexture(const Texture& texture, std::size_t data_offset, void* data) const
	{
		auto clw_texture = reinterpret_cast<ClwScene::Texture*>(data);
		clw_texture->w = texture.width();
		clw_texture->h = texture.height();
		clw_texture->d = texture.depth();
		clw_texture->fmt = GetTextureFormat(texture.format());
		clw_texture->dataoffset = static_cast<int>(data_offset);
	}

	void
	ClwSceneController::WriteTextureData(Texture& texture, void* dest) const
	{
		switch (texture.format())
		{
		case Format::B8G8R8A8SRGB:
		case Format::B8G8R8A8SNorm:
		case Format::B8G8R8A8UNorm:
		case Format::R8G8B8A8SRGB:
		case Format::R8G8B8A8SNorm:
		case Format::R8G8B8A8UNorm:
		case Format::R16G16B16A16SFloat:
		case Format::R32G32B32A32SFloat:
		{
			std::copy(texture.data(), texture.data() + texture.size(), static_cast<char*>(dest));
		}
		break;
		case Format::B8G8R8SRGB:
		case Format::B8G8R8SNorm:
		case Format::B8G8R8UNorm:
		case Format::R8G8B8SRGB:
		case Format::R8G8B8SNorm:
		case Format::R8G8B8UNorm:
		{
			auto data = (math::uchar3*)texture.data();
			for (std::size_t i = 0; i < texture.width() * texture.height(); i++)
				((math::uchar4*)dest)[i].set(data[i].x, data[i].y, data[i].z, 255);
		}
		break;
		case Format::R32G32B32SFloat:
		{
			auto data = (math::float3*)texture.data();
			for (std::size_t i = 0; i < texture.width() * texture.height(); i++)
				((math::float4*)dest)[i].set(data[i]);
		}
		break;
		default:
			assert(false);
		}
	}

	void
	ClwSceneController::updateTextures(const std::shared_ptr<RenderScene>& scene, ClwScene& out)
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
				auto tex = tex_iter->ItemAs<Texture>();
				this->WriteTexture(*tex, numTexDataBufferSize, textures + numTexturesWritten);

				numTexturesWritten++;
				numTexDataBufferSize += GetTextureSize(*tex);
			}

			context_.UnmapBuffer(0, out.textures, textures);

			if (numTexDataBufferSize > out.texturedata.GetElementCount())
				out.texturedata = context_.CreateBuffer<char>(numTexDataBufferSize, CL_MEM_READ_ONLY);

			char* data = nullptr;
			context_.MapBuffer(0, out.texturedata, CL_MAP_WRITE, &data).Wait();

			std::size_t numBytesWritten = 0;
			for (tex_iter->Reset(); tex_iter->IsValid(); tex_iter->Next())
			{
				auto tex = tex_iter->ItemAs<Texture>();
				this->WriteTextureData(*tex, data + numBytesWritten);
				numBytesWritten += GetTextureSize(*tex);
			}

			context_.UnmapBuffer(0, out.texturedata, data);
		}
	}

	void
	ClwSceneController::WriteLight(const std::shared_ptr<RenderScene>& scene, const Light& light, void* data) const
	{
		auto clwLight = reinterpret_cast<ClwScene::Light*>(data);

		auto& translate = light.getTranslate();
		auto& direction = light.getForward();

		auto size = light.getSize();
		auto power = light.getColor() * light.getIntensity();

		if (light.isA<PointLight>())
		{
			clwLight->type = ClwScene::kPoint;
			clwLight->p = RadeonRays::float3(translate.x, translate.y, translate.z);
			clwLight->intensity = RadeonRays::float3(power.x, power.y, power.z);
			clwLight->size = size;
		}
		else if (light.isA<DirectionalLight>())
		{
			clwLight->type = ClwScene::kDirectional;
			clwLight->d = RadeonRays::float3(direction.x, direction.y, direction.z);
			clwLight->intensity = RadeonRays::float3(power.x, power.y, power.z);
			clwLight->size = exp2(15 * (1 - size) + 1);
		}
		else if (light.isA<SpotLight>())
		{
			clwLight->type = ClwScene::kSpot;
			clwLight->p = RadeonRays::float3(translate.x, translate.y, translate.z);
			clwLight->d = RadeonRays::float3(direction.x, direction.y, direction.z);
			clwLight->intensity = RadeonRays::float3(power.x, power.y, power.z);
			clwLight->ia = light.downcast<SpotLight>()->getInnerCone().x;
			clwLight->oa = light.downcast<SpotLight>()->getOuterCone().x;
			clwLight->size = size;
		}
		else if (light.isA<EnvironmentLight>())
		{
			auto& ibl = static_cast<EnvironmentLight const&>(light);
			clwLight->type = ClwScene::kIbl;
			clwLight->multiplier = ibl.getIntensity();
			clwLight->intensity = RadeonRays::float3(power.x, power.y, power.z);
			clwLight->tex = GetTextureIndex(textureCollector, ibl.getBackgroundMap());
			clwLight->tex_reflection = -1;
			clwLight->tex_refraction = -1;
			clwLight->tex_transparency = -1;
			clwLight->tex_background = GetTextureIndex(textureCollector, ibl.getBackgroundMap());
			clwLight->offset = RadeonRays::float2(ibl.getOffset().x, ibl.getOffset().y);
			clwLight->ibl_mirror_x = true;
			clwLight->size = size;
		}
	}

	void
	ClwSceneController::updateLights(const std::shared_ptr<RenderScene>& scene, ClwScene& out)
	{
		int numLightsWritten = 0;
		std::size_t numLights = scene->getLights().size();
		std::size_t distribution_buffer_size = (1 + 1 + numLights + numLights);

		if (numLights > 0)
		{
			if (numLights > out.lights.GetElementCount())
			{
				out.lights = context_.CreateBuffer<ClwScene::Light>(numLights, CL_MEM_READ_ONLY);
				out.lightDistributions = context_.CreateBuffer<int>(distribution_buffer_size, CL_MEM_READ_ONLY);
			}

			ClwScene::Light* lights = nullptr;
			context_.MapBuffer(0, out.lights, CL_MAP_WRITE, &lights).Wait();

			out.envmapidx = -1;
			out.showBackground = false;

			std::vector<float> lightPower(numLights);

			for (auto& light : scene->getLights())
			{
				WriteLight(scene, *light, lights + numLightsWritten);

				if (light->isA<EnvironmentLight>())
				{
					out.envmapidx = numLightsWritten;
					out.showBackground = light->downcast<EnvironmentLight>()->getShowBackground();
				}

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
		}

		out.numLights = static_cast<int>(numLightsWritten);
	}

	void
	ClwSceneController::updateMaterials(const std::shared_ptr<RenderScene>& scene, ClwScene& out)
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
			auto mat = mat_iter->ItemAs<MeshStandardMaterial>();

			auto& material = materials[i];
			material.offset = 0;
			material.shadow = mat->getReceiveShadow();
			material.flags = ClwScene::BxdfFlags::kBxdfFlagsDiffuse;
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
			material.disney.subsurface_map_idx = GetTextureIndex(textureCollector, mat->getSubsurfaceMap());
			material.disney.subsurface_color = RadeonRays::float3(mat->getSubsurfaceColor().x, mat->getSubsurfaceColor().y, mat->getSubsurfaceColor().z);
			material.disney.subsurface_color_map_idx = GetTextureIndex(textureCollector, mat->getSubsurfaceColorMap());
			material.disney.emissive = RadeonRays::float3(mat->getEmissive().x, mat->getEmissive().y, mat->getEmissive().z) * mat->getEmissiveIntensity();
			material.disney.emissive_map_idx = GetTextureIndex(textureCollector, mat->getEmissiveMap());
			material.disney.refraction_ior = mat->getRefractionRatio();
			material.disney.transmission = mat->getTransmission();

			if (mat->getTransmission() == 0)
				material.flags |= ClwScene::BxdfFlags::kBxdfFlagsBrdf;

			this->materialidToOffset_[mat] = material;
		}

		context_.UnmapBuffer(0, out.materials, materials);
	}

    void
    ClwSceneController::updateIntersector(const std::shared_ptr<RenderScene>& scene, ClwScene& out) const
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
			if (!mesh)
				continue;

			for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
			{
				auto material = this->getMaterialIndex(geometry->getMaterial(i));
				if (!material)
					continue;

				auto shape = this->api_->CreateMesh(
					(float*)mesh->getVertexArray().data(),
					static_cast<int>(mesh->getVertexArray().size()),
					sizeof(math::float3),
					reinterpret_cast<int const*>(mesh->getIndicesArray(i).data()),
					0,
					nullptr,
					static_cast<int>(mesh->getIndicesArray(i).size() / 3)
				);

				auto transform = geometry->getTransform();
				auto transformInverse = geometry->getTransformInverse();

				RadeonRays::matrix m(
					transform.a1, transform.b1, transform.c1, transform.d1,
					transform.a2, transform.b2, transform.c2, transform.d2,
					transform.a3, transform.b3, transform.c3, transform.d3,
					transform.a4, transform.b4, transform.c4, transform.d4);

				RadeonRays::matrix minv(
					transformInverse.a1, transformInverse.b1, transformInverse.c1, transformInverse.d1,
					transformInverse.a2, transformInverse.b2, transformInverse.c2, transformInverse.d2,
					transformInverse.a3, transformInverse.b3, transformInverse.c3, transformInverse.d3,
					transformInverse.a4, transformInverse.b4, transformInverse.c4, transformInverse.d4);

				shape->SetId(id++);
				shape->SetTransform(m, minv);

				this->api_->AttachShape(shape);

				out.isectShapes.push_back(shape);
				out.visibleShapes.push_back(shape);
			}
		}

		if (!out.isectShapes.empty())
			this->api_->Commit();
	}

	void
	ClwSceneController::updateShapes(const std::shared_ptr<RenderScene>& scene, ClwScene& out) const
	{
		std::size_t num_vertices = 0;
		std::size_t num_indices = 0;
		std::size_t num_shapes = 0;
		std::size_t num_geometries = 0;

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
			if (mesh)
			{
				num_vertices += mesh->getVertexArray().size();
				num_geometries++;

				for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
				{
					num_indices += mesh->getIndicesArray(i).size();
					num_shapes++;
				}
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

		if (num_shapes > 0)
		{
			out.shapes = context_.CreateBuffer<ClwScene::Shape>(num_shapes, CL_MEM_READ_ONLY);
			out.shapesAdditional = context_.CreateBuffer<ClwScene::ShapeAdditionalData>(num_shapes, CL_MEM_READ_ONLY);

			math::float4* vertices = nullptr;
			math::float4* normals = nullptr;
			math::float2* uvs = nullptr;
			std::int32_t* indices = nullptr;
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
				if (!mesh) {
					continue;
				}

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
					auto material = this->getMaterialIndex(geometry->getMaterial(i));
					if (!material)
						continue;

					ClwScene::Shape shape;
					shape.id = id++;
					shape.startvtx = static_cast<int>(num_vertices_written);
					shape.startidx = static_cast<int>(num_indices_written);

					auto transform = geometry->getTransform();
					shape.transform.m0 = { transform.a1, transform.b1, transform.c1, transform.d1 };
					shape.transform.m1 = { transform.a2, transform.b2, transform.c2, transform.d2 };
					shape.transform.m2 = { transform.a3, transform.b3, transform.c3, transform.d3 };
					shape.transform.m3 = { transform.a4, transform.b4, transform.c4, transform.d4 };

					shape.linearvelocity = float3(0.0f, 0.f, 0.f);
					shape.angularvelocity = float3(0.f, 0.f, 0.f, 1.f);
					shape.material = material.value();
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
		}

		this->updateIntersector(scene, out);

		out.numGeometries = static_cast<int>(num_geometries);
	}
}