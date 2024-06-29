#include <octoon/video/scriptable_scene_controller.h>
#include <octoon/video/scriptable_render_buffer.h>
#include <octoon/video/scriptable_render_material.h>

#include <octoon/mesh/plane_mesh.h>

#include <octoon/light/ambient_light.h>
#include <octoon/light/directional_light.h>
#include <octoon/light/point_light.h>
#include <octoon/light/spot_light.h>
#include <octoon/light/disk_light.h>
#include <octoon/light/rectangle_light.h>
#include <octoon/light/environment_light.h>
#include <octoon/light/tube_light.h>

#include <octoon/hal/graphics_device.h>
#include <octoon/hal/graphics_framebuffer.h>
#include <octoon/hal/graphics_data.h>
#include <octoon/hal/graphics_context.h>

#include <stdexcept>

namespace octoon
{
	ScriptableSceneController::ScriptableSceneController()
	{
	}

	ScriptableSceneController::ScriptableSceneController(const GraphicsContextPtr& context)
		: context_(context)
	{
	}

	ScriptableSceneController::~ScriptableSceneController()
	{
	}

	void
	ScriptableSceneController::compileScene(const std::shared_ptr<RenderScene>& scene) noexcept
	{
		materialCollector.Clear();

		for (auto& geometry : scene->getGeometries())
		{
			if (!geometry->getVisible())
				continue;

			for (std::size_t i = 0; i < geometry->getMaterials().size(); ++i)
			{
				auto& mat = geometry->getMaterial(i);
				if (mat)
					materialCollector.Collect(mat);
			}
		}

		materialCollector.Commit();

		auto iter = sceneCache_.find(scene);
		if (iter == sceneCache_.cend())
		{
			auto out = std::make_shared<RenderingData>();

			this->updateCamera(scene, *out, true);
			this->updateLights(scene, *out, true);
			this->updateMaterials(scene, *out, true);
			this->updateShapes(scene, *out, true);

			sceneCache_[scene] = out;
		}
		else
		{
			auto& renderingData = (*iter).second;

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

			bool should_update_shapes = scene->isSceneDirty();
			if (!should_update_shapes)
			{
				for (auto& geometry : scene->getGeometries())
				{
					if (!geometry->getVisible())
						continue;

					if (geometry->isDirty())
					{
						should_update_shapes = true;
						break;
					}
				}
			}

			bool should_update_materials = !renderingData->material_bundle || materialCollector.NeedsUpdate(renderingData->material_bundle.get(),
				[](Object* ptr)->bool
			{
				auto mat = ptr->downcast<Material>();
				return mat->isDirty();
			});

			auto camera = scene->getMainCamera();
			bool should_update_camera = renderingData->camera != camera || camera->isDirty();

			if (should_update_camera)
				this->updateCamera(scene, *renderingData);
			
			if (should_update_lights || should_update_camera)
				this->updateLights(scene, *renderingData);

			if (should_update_materials)
				this->updateMaterials(scene, *renderingData);

			if (should_update_shapes)
				this->updateShapes(scene, *renderingData);
		}
	}

	RenderingData&
	ScriptableSceneController::getCachedScene(const std::shared_ptr<RenderScene>& scene) const noexcept(false)
	{
		auto iter = sceneCache_.find(scene);
		if (iter != sceneCache_.cend())
			return *iter->second.get();
		else
			throw std::runtime_error("Scene has not been compiled");
	}

	void
	ScriptableSceneController::cleanCache() noexcept
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
	ScriptableSceneController::updateCamera(const std::shared_ptr<RenderScene>& scene, RenderingData& out, bool force)
	{
		out.camera = scene->getMainCamera();
	}

	void
	ScriptableSceneController::updateLights(const std::shared_ptr<RenderScene>& scene, RenderingData& out, bool force)
	{
		out.reset();

		for (auto& light : scene->getLights())
		{
			if (!light->getVisible())
				continue;

			if (light->getLayer() != out.camera->getLayer())
				continue;
			
			light->onRenderBefore(*out.camera);

			if (light->isA<AmbientLight>())
			{
				out.ambientLightColors += light->getColor() * light->getIntensity();
			}
			else if (light->isA<EnvironmentLight>())
			{
				auto it = light->downcast<EnvironmentLight>();
				RenderingData::EnvironmentLight environmentLight;
				environmentLight.intensity = it->getIntensity();
				environmentLight.offset = it->getOffset();
				environmentLight.radiance.reset();
				if (it->getEnvironmentMap())
					environmentLight.radiance = it->getEnvironmentMap()->getNativeTexture();
				else
					out.ambientLightColors += light->getColor() * light->getIntensity();
				out.environmentLights.emplace_back(environmentLight);
				out.numEnvironment++;
			}
			else if (light->isA<DirectionalLight>())
			{
				auto it = light->downcast<DirectionalLight>();
				auto color = it->getColor() * it->getIntensity();
				RenderingData::DirectionalLight directionLight;
				directionLight.direction = math::float4(math::float3x3(out.camera->getView()) * -it->getForward(), 0);
				directionLight.color[0] = color.x;
				directionLight.color[1] = color.y;
				directionLight.color[2] = color.z;
				directionLight.shadow = it->getShadowEnable();

				auto framebuffer = it->getCamera()->getFramebuffer();
				if (framebuffer && directionLight.shadow)
				{
					directionLight.shadow = it->getShadowEnable();
					directionLight.shadowBias = it->getShadowBias();
					directionLight.shadowRadius = it->getShadowRadius();
					directionLight.shadowMapSize = math::float2(float(framebuffer->getFramebufferDesc().getWidth()), float(framebuffer->getFramebufferDesc().getHeight()));

					math::float4x4 viewport;
					viewport.makeScale(math::float3(0.5f, 0.5f, 0.5f));
					viewport.translate(math::float3(0.5f, 0.5f, 0.5f));

					out.directionalShadows.emplace_back(framebuffer->getFramebufferDesc().getColorAttachment().getBindingTexture());
					out.directionalShadowMatrix.push_back(viewport * it->getCamera()->getViewProjection());
				}

				out.numDirectional++;
				out.directionalLights.emplace_back(directionLight);
			}
			else if (light->isA<SpotLight>())
			{
				auto it = light->downcast<SpotLight>();
				RenderingData::SpotLight spotLight;
				spotLight.color.set(it->getColor() * it->getIntensity());
				spotLight.direction.set(math::float3x3(out.camera->getView()) * it->getForward());
				spotLight.position.set(it->getTranslate());
				spotLight.distance = 0;
				spotLight.decay = 0;
				spotLight.coneCos = it->getInnerCone().y;
				spotLight.penumbraCos = it->getOuterCone().y;
				spotLight.shadow = it->getShadowEnable();

				if (spotLight.shadow)
				{
					auto framebuffer = it->getCamera()->getFramebuffer();
					if (framebuffer)
					{
						spotLight.shadowBias = it->getShadowBias();
						spotLight.shadowRadius = it->getShadowRadius();
						spotLight.shadowMapSize = math::float2(float(framebuffer->getFramebufferDesc().getWidth()), float(framebuffer->getFramebufferDesc().getHeight()));

						out.spotShadows.emplace_back(framebuffer->getFramebufferDesc().getColorAttachment().getBindingTexture());
					}
				}

				out.spotLights.emplace_back(spotLight);
				out.numSpot++;
			}
			else if (light->isA<PointLight>())
			{
				auto it = light->downcast<PointLight>();
				RenderingData::PointLight pointLight;
				pointLight.color.set(it->getColor() * it->getIntensity());
				pointLight.position.set(it->getTranslate());
				pointLight.distance = 0;
				pointLight.decay = 0;
				pointLight.shadow = it->getShadowEnable();

				if (pointLight.shadow)
				{
					auto framebuffer = it->getCamera()->getFramebuffer();
					if (framebuffer && pointLight.shadow)
					{
						pointLight.shadowBias = it->getShadowBias();
						pointLight.shadowRadius = it->getShadowRadius();
						pointLight.shadowMapSize = math::float2(float(framebuffer->getFramebufferDesc().getWidth()), float(framebuffer->getFramebufferDesc().getHeight()));

						out.pointShadows.emplace_back(framebuffer->getFramebufferDesc().getColorAttachment().getBindingTexture());
					}
				}

				out.pointLights.emplace_back(pointLight);
				out.numPoint++;
			}
			else if (light->isA<RectangleLight>())
			{
				auto it = light->downcast<RectangleLight>();
				RenderingData::RectAreaLight rectangleLight;
				rectangleLight.color.set(it->getColor() * it->getIntensity());
				rectangleLight.position.set(it->getTranslate());
				rectangleLight.halfWidth.set(math::float3::One);
				rectangleLight.halfHeight.set(math::float3::One);
				out.rectangleLights.emplace_back(rectangleLight);
				out.numRectangle++;
			}

			light->onRenderAfter(*out.camera);

			out.lights.push_back(light);
		}

		if (out.numSpot)
		{
			if (!out.spotLightBuffer)
			{
				out.spotLightBuffer = this->context_->getDevice()->createGraphicsData(GraphicsDataDesc(
					GraphicsDataType::UniformBuffer,
					GraphicsUsageFlagBits::ReadBit | GraphicsUsageFlagBits::WriteBit,
					out.spotLights.data(),
					sizeof(RenderingData::SpotLight) * out.numSpot
				));
			}
			else
			{
				auto desc = out.spotLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.spotLights.size() * sizeof(RenderingData::SpotLight))
				{
					out.spotLightBuffer = this->context_->getDevice()->createGraphicsData(GraphicsDataDesc(
						GraphicsDataType::UniformBuffer,
						GraphicsUsageFlagBits::ReadBit | GraphicsUsageFlagBits::WriteBit,
						out.spotLights.data(),
						sizeof(RenderingData::SpotLight) * out.numSpot
					));
				}
				else
				{
					void* data;
					if (out.spotLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.spotLights.data(), out.spotLights.size() * sizeof(RenderingData::SpotLight));
					out.spotLightBuffer->unmap();
				}
			}
		}

		if (out.numPoint)
		{
			if (!out.pointLightBuffer)
			{
				out.pointLightBuffer = this->context_->getDevice()->createGraphicsData(GraphicsDataDesc(
					GraphicsDataType::UniformBuffer,
					GraphicsUsageFlagBits::ReadBit | GraphicsUsageFlagBits::WriteBit,
					out.pointLights.data(),
					sizeof(RenderingData::PointLight) * out.numPoint
				));
			}
			else
			{
				auto desc = out.pointLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.pointLights.size() * sizeof(RenderingData::PointLight))
				{
					out.pointLightBuffer = this->context_->getDevice()->createGraphicsData(GraphicsDataDesc(
						GraphicsDataType::UniformBuffer,
						GraphicsUsageFlagBits::ReadBit | GraphicsUsageFlagBits::WriteBit,
						out.pointLights.data(),
						sizeof(RenderingData::PointLight) * out.numPoint
					));
				}
				else
				{
					void* data;
					if (out.pointLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.pointLights.data(), out.pointLights.size() * sizeof(RenderingData::PointLight));
					out.pointLightBuffer->unmap();
				}
			}
		}

		if (out.numRectangle)
		{
			if (!out.rectangleLightBuffer)
			{
				out.rectangleLightBuffer = this->context_->getDevice()->createGraphicsData(GraphicsDataDesc(
					GraphicsDataType::UniformBuffer,
					GraphicsUsageFlagBits::ReadBit | GraphicsUsageFlagBits::WriteBit,
					out.rectangleLights.data(),
					sizeof(RenderingData::RectAreaLight) * out.numRectangle
				));
			}
			else
			{
				auto desc = out.rectangleLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.rectangleLights.size() * sizeof(RenderingData::RectAreaLight))
				{
					out.rectangleLightBuffer = this->context_->getDevice()->createGraphicsData(GraphicsDataDesc(
						GraphicsDataType::UniformBuffer,
						GraphicsUsageFlagBits::ReadBit | GraphicsUsageFlagBits::WriteBit,
						out.rectangleLights.data(),
						sizeof(RenderingData::RectAreaLight) * out.numRectangle
					));
				}
				else
				{
					void* data;
					if (out.rectangleLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.rectangleLights.data(), out.rectangleLights.size() * sizeof(RenderingData::RectAreaLight));
					out.rectangleLightBuffer->unmap();
				}
			}
		}

		if (out.numDirectional)
		{
			if (!out.directionLightBuffer)
			{
				out.directionLightBuffer = this->context_->getDevice()->createGraphicsData(GraphicsDataDesc(
					GraphicsDataType::UniformBuffer,
					GraphicsUsageFlagBits::ReadBit | GraphicsUsageFlagBits::WriteBit,
					out.directionalLights.data(),
					sizeof(RenderingData::DirectionalLight) * out.numDirectional
				));
			}
			else
			{
				auto desc = out.directionLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.directionalLights.size() * sizeof(RenderingData::DirectionalLight))
				{
					out.directionLightBuffer = this->context_->getDevice()->createGraphicsData(GraphicsDataDesc(
						GraphicsDataType::UniformBuffer,
						GraphicsUsageFlagBits::ReadBit | GraphicsUsageFlagBits::WriteBit,
						out.directionalLights.data(),
						sizeof(RenderingData::DirectionalLight) * out.numDirectional
					));
				}
				else
				{
					void* data;
					if (out.directionLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.directionalLights.data(), out.directionalLights.size() * sizeof(RenderingData::DirectionalLight));
					out.directionLightBuffer->unmap();
				}
			}
		}
	}

	void
	ScriptableSceneController::updateMaterials(const std::shared_ptr<RenderScene>& scene, RenderingData& out, bool force)
	{
		out.material_bundle.reset(materialCollector.CreateBundle());

		if (out.depthMaterial->isDirty())
		{
			out.materials_[out.depthMaterial.get()] = std::make_shared<ScriptableRenderMaterial>(this->context_, out.depthMaterial, out);
			out.depthMaterial->setDirty(false);
		}

		if (out.overrideMaterial)
		{
			out.materials_[out.depthMaterial.get()] = std::make_shared<ScriptableRenderMaterial>(this->context_, out.depthMaterial, out);
			out.overrideMaterial->setDirty(false);
		}

		std::unique_ptr<Iterator> mat_iter(materialCollector.CreateIterator());
		for (std::size_t i = 0; mat_iter->IsValid(); mat_iter->Next(), i++)
		{
			auto mat = mat_iter->ItemAs<Material>();
			if (mat->isDirty() || force)
			{
				auto material = mat->downcast_pointer<Material>();
				out.materials_[material.get()] = std::make_shared<ScriptableRenderMaterial>(this->context_, material, out);
			}
		}
	}

    void
    ScriptableSceneController::updateShapes(const std::shared_ptr<RenderScene>& scene, RenderingData& out, bool force)
    {
		out.geometries = scene->getGeometries();

		if (!out.screenQuad)
		{
			out.screenQuad = std::make_shared<Geometry>();
			out.screenQuad->setMesh(std::make_shared<PlaneMesh>(2.0f, 2.0f));

			auto screenMesh = out.screenQuad->getMesh();
			out.buffers_[screenMesh.get()] = std::make_shared<ScriptableRenderBuffer>(this->context_->getDevice(), screenMesh);
		}

		for (auto& geometry : scene->getGeometries())
		{
			if (!geometry->getVisible())
				continue;

			if (geometry->isDirty() || force)
			{
				auto mesh = geometry->getMesh();
				auto it = out.buffers_.find(mesh.get());
				if (it != out.buffers_.end())
				{
					(*it).second->updateData(this->context_->getDevice(), mesh);
				}
				else
				{
					out.buffers_[mesh.get()] = std::make_shared<ScriptableRenderBuffer>(this->context_->getDevice(), mesh);
				}
			}
		}
    }
}
