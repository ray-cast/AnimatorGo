#include <octoon/video/forward_scene_controller.h>

#include <octoon/camera/perspective_camera.h>
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

namespace octoon::video
{
	ForwardSceneController::ForwardSceneController(const hal::GraphicsContextPtr& context)
		: context_(context)
	{
	}

	void
	ForwardSceneController::compileScene(RenderScene* scene) noexcept
	{
		auto iter = sceneCache_.find(scene);
		if (iter != sceneCache_.cend())
		{
			this->updateCamera(scene, *(*iter).second);
			this->updateLights(scene, *(*iter).second);
			this->updateIntersector(scene, *(*iter).second);
		}
		else
		{
			auto out = std::make_unique<ForwardScene>();
			this->updateCamera(scene, *out);
			this->updateLights(scene, *out);
			this->updateIntersector(scene, *out);
			sceneCache_[scene] = std::move(out);
		}
	}

	CompiledScene&
	ForwardSceneController::getCachedScene(const RenderScene* scene) const noexcept(false)
	{
		auto iter = sceneCache_.find(scene);

		if (iter != sceneCache_.cend())
			return *iter->second.get();
		else
			throw std::runtime_error("Scene has not been compiled");
	}

	void
	ForwardSceneController::updateCamera(const RenderScene* scene, ForwardScene& out) const
	{
		out.camera = scene->getMainCamera();
	}

    void
    ForwardSceneController::updateIntersector(const RenderScene* scene, ForwardScene& out) const
    {
		out.geometries = scene->getGeometries();
    }

	void
	ForwardSceneController::updateLights(const RenderScene* scene, ForwardScene& out) noexcept
	{
		out.reset();

		for (auto& light : scene->getLights())
		{
			if (out.camera->getLayer() != light->getLayer())
				continue;

			if (light->getVisible())
			{
				light->onRenderBefore(*out.camera);

				if (light->isA<light::AmbientLight>())
				{
					out.ambientLightColors += light->getColor() * light->getIntensity() * math::PI;
				}
				else if (light->isA<light::EnvironmentLight>())
				{
					auto it = light->downcast<light::EnvironmentLight>();
					ForwardScene::EnvironmentLight environmentLight;
					environmentLight.intensity = it->getIntensity();
					environmentLight.radiance = it->getEnvironmentMap();
					if (!it->getEnvironmentMap())
						out.ambientLightColors += light->getColor() * light->getIntensity() * math::PI;
					out.environmentLights.emplace_back(environmentLight);
					out.numEnvironment++;
				}
				else if (light->isA<light::DirectionalLight>())
				{
					auto it = light->downcast<light::DirectionalLight>();
					ForwardScene::DirectionalLight directionLight;
					directionLight.direction = math::float4(math::float3x3(out.camera->getView()) * -it->getForward(), 0);
					directionLight.color = it->getColor() * it->getIntensity();
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
				else if (light->isA<light::SpotLight>())
				{
					auto it = light->downcast<light::SpotLight>();
					ForwardScene::SpotLight spotLight;
					spotLight.color.set(it->getColor() * it->getIntensity());
					spotLight.direction.set(math::float3x3(out.camera->getView()) * it->getForward());
					spotLight.position.set(it->getTranslate());
					spotLight.distance = 0;
					spotLight.decay = 0;
					spotLight.coneCos = it->getInnerCone().y;
					spotLight.penumbraCos = it->getOuterCone().y;
					spotLight.shadow = it->getShadowEnable();

					auto framebuffer = it->getCamera()->getFramebuffer();
					if (framebuffer && spotLight.shadow)
					{						
						spotLight.shadowBias = it->getShadowBias();
						spotLight.shadowRadius = it->getShadowRadius();
						spotLight.shadowMapSize = math::float2(float(framebuffer->getFramebufferDesc().getWidth()), float(framebuffer->getFramebufferDesc().getHeight()));

						out.spotShadows.emplace_back(framebuffer->getFramebufferDesc().getColorAttachment().getBindingTexture());
					}

					out.spotLights.emplace_back(spotLight);
					out.numSpot++;
				}
				else if (light->isA<light::PointLight>())
				{
					auto it = light->downcast<light::PointLight>();
					ForwardScene::PointLight pointLight;
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
				else if (light->isA<light::RectangleLight>())
				{
					auto it = light->downcast<light::RectangleLight>();
					ForwardScene::RectAreaLight rectangleLight;
					rectangleLight.color.set(it->getColor() * it->getIntensity());
					rectangleLight.position.set(it->getTranslate());
					rectangleLight.halfWidth.set(math::float3::One);
					rectangleLight.halfHeight.set(math::float3::One);
					out.rectangleLights.emplace_back(rectangleLight);
					out.numRectangle++;
				}

				light->onRenderAfter(*out.camera);
			}
		}

		if (out.numSpot)
		{
			if (!out.spotLightBuffer)
			{
				out.spotLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					out.spotLights.data(),
					sizeof(ForwardScene::SpotLight) * out.numSpot
				));
			}
			else
			{
				auto desc = out.spotLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.spotLights.size() * sizeof(ForwardScene::SpotLight))
				{
					out.spotLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						out.spotLights.data(),
						sizeof(ForwardScene::SpotLight) * out.numSpot
					));
				}
				else
				{
					void* data;
					if (out.spotLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.spotLights.data(), out.spotLights.size() * sizeof(ForwardScene::SpotLight));
					out.spotLightBuffer->unmap();
				}
			}
		}

		if (out.numPoint)
		{
			if (!out.pointLightBuffer)
			{
				out.pointLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					out.pointLights.data(),
					sizeof(ForwardScene::PointLight) * out.numPoint
				));
			}
			else
			{
				auto desc = out.pointLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.pointLights.size() * sizeof(ForwardScene::PointLight))
				{
					out.pointLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						out.pointLights.data(),
						sizeof(ForwardScene::PointLight) * out.numPoint
					));
				}
				else
				{
					void* data;
					if (out.pointLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.pointLights.data(), out.pointLights.size() * sizeof(ForwardScene::PointLight));
					out.pointLightBuffer->unmap();
				}
			}
		}

		if (out.numRectangle)
		{
			if (!out.rectangleLightBuffer)
			{
				out.rectangleLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					out.rectangleLights.data(),
					sizeof(ForwardScene::RectAreaLight) * out.numRectangle
				));
			}
			else
			{
				auto desc = out.rectangleLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.rectangleLights.size() * sizeof(ForwardScene::RectAreaLight))
				{
					out.rectangleLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						out.rectangleLights.data(),
						sizeof(ForwardScene::RectAreaLight) * out.numRectangle
					));
				}
				else
				{
					void* data;
					if (out.rectangleLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.rectangleLights.data(), out.rectangleLights.size() * sizeof(ForwardScene::RectAreaLight));
					out.rectangleLightBuffer->unmap();
				}
			}
		}

		if (out.numDirectional)
		{
			if (!out.directionLightBuffer)
			{
				out.directionLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
					hal::GraphicsDataType::UniformBuffer,
					hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
					out.directionalLights.data(),
					sizeof(ForwardScene::DirectionalLight) * out.numDirectional
				));
			}
			else
			{
				auto desc = out.directionLightBuffer->getDataDesc();
				if (desc.getStreamSize() < out.directionalLights.size() * sizeof(ForwardScene::DirectionalLight))
				{
					out.directionLightBuffer = this->context_->getDevice()->createGraphicsData(hal::GraphicsDataDesc(
						hal::GraphicsDataType::UniformBuffer,
						hal::GraphicsUsageFlagBits::ReadBit | hal::GraphicsUsageFlagBits::WriteBit,
						out.directionalLights.data(),
						sizeof(ForwardScene::DirectionalLight) * out.numDirectional
					));
				}
				else
				{
					void* data;
					if (out.directionLightBuffer->map(0, desc.getStreamSize(), &data))
						std::memcpy(data, out.directionalLights.data(), out.directionalLights.size() * sizeof(ForwardScene::DirectionalLight));
					out.directionLightBuffer->unmap();
				}
			}
		}
	}
}