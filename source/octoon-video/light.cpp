#include <octoon/video/light.h>
#include <octoon/video/camera.h>
#include <octoon/video/perspective_camera.h>
#include <octoon/video/ortho_camera.h>
#include <octoon/hal/graphics_texture.h>

namespace octoon
{
	namespace video
	{
		OctoonImplementSubClass(Light, RenderObject, "Light")

		Light::Light() noexcept
			: _lightType(LightType::LightTypePoint)
			, _lightIntensity(1.0f)
			, _lightRange(10.0f)
			, _lightColor(math::float3::One)
			, _lightAttenuation(math::float3::UnitZ)
			, _spotInnerCone(5.0f, math::cos(math::radians(5.0f)))
			, _spotOuterCone(40.0f, math::cos(math::radians(40.0f)))
			, _enableSoftShadow(false)
			, _enableGlobalIllumination(false)
			, _shadowMode(ShadowMode::ShadowModeNone)
			, _shadowBias(0.1f)
			, _shadowFactor(600.0f)
		{
		}

		Light::~Light() noexcept
		{
			this->destroyShadowMap();
			this->destroyReflectiveShadowMap();
		}

		void
		Light::setLightType(LightType type) noexcept
		{
			_lightType = type;
			this->_updateBoundingBox();
		}

		void
		Light::setLightIntensity(float intensity) noexcept
		{
			_lightIntensity = intensity;
		}

		void
		Light::setLightRange(float range) noexcept
		{
			_lightRange = range;
			this->_updateBoundingBox();
		}

		void
		Light::setLightColor(const math::float3& color) noexcept
		{
			_lightColor = color;
		}

		void
		Light::setSpotInnerCone(float value) noexcept
		{
			_spotInnerCone.x = math::min(_spotOuterCone.x, value);
			_spotInnerCone.y = math::cos(math::radians(_spotInnerCone.x));
		}

		void
		Light::setSpotOuterCone(float value) noexcept
		{
			_spotOuterCone.x = math::max(_spotInnerCone.x, value);
			_spotOuterCone.y = math::cos(math::radians(_spotOuterCone.x));
			this->_updateBoundingBox();
		}

		LightType
		Light::getLightType() const noexcept
		{
			return _lightType;
		}

		float
		Light::getLightIntensity() const noexcept
		{
			return _lightIntensity;
		}

		float
		Light::getLightRange() const noexcept
		{
			return _lightRange;
		}

		const math::float3&
		Light::getLightColor() const noexcept
		{
			return _lightColor;
		}

		void
		Light::setLightAttenuation(const math::float3& attenuation) noexcept
		{
			_lightAttenuation = attenuation;
		}

		const math::float3&
		Light::getLightAttenuation() const noexcept
		{
			return _lightAttenuation;
		}

		const math::float2&
		Light::getSpotInnerCone() const noexcept
		{
			return _spotInnerCone;
		}

		const math::float2&
		Light::getSpotOuterCone() const noexcept
		{
			return _spotOuterCone;
		}

		void
		Light::setShadowMode(ShadowMode shadowMode) noexcept
		{
			if (_shadowMode != shadowMode)
			{
				_shadowMode = shadowMode;

				if (shadowMode != ShadowMode::ShadowModeNone)
					this->setupShadowMap();
				else
					this->destroyShadowMap();

				if (_enableGlobalIllumination)
				{
					this->destroyReflectiveShadowMap();
				}
			}
		}

		ShadowMode
		Light::getShadowMode() const noexcept
		{
			return _shadowMode;
		}

		void
		Light::setGlobalIllumination(bool enable) noexcept
		{
			if (_enableGlobalIllumination != enable)
			{
				_enableGlobalIllumination = enable;
			}
		}

		bool
		Light::getGlobalIllumination() const noexcept
		{
			return _enableGlobalIllumination;
		}

		const CameraPtr&
		Light::getCamera() const noexcept
		{
			return _shadowCamera;
		}

		void
		Light::setShadowBias(float bias) noexcept
		{
			_shadowBias = bias;
		}

		void
		Light::setShadowFactor(float factor) noexcept
		{
			_shadowFactor = factor;
		}

		float
		Light::getShadowBias() const noexcept
		{
			return _shadowBias;
		}

		float
		Light::getShadowFactor() const noexcept
		{
			return _shadowFactor;
		}

		void
		Light::setSkyBox(const hal::GraphicsTexturePtr& texture) noexcept
		{
			assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
			_skybox = texture;
		}

		const hal::GraphicsTexturePtr&
		Light::getSkyBox() const noexcept
		{
			return _skybox;
		}

		void
		Light::setSkyLightingDiffuse(const hal::GraphicsTexturePtr& texture) noexcept
		{
			assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube);
			_skyDiffuseIBL = texture;
		}

		const hal::GraphicsTexturePtr&
		Light::getSkyLightingDiffuse() const noexcept
		{
			return _skyDiffuseIBL;
		}

		void
		Light::setSkyLightingSpecular(const hal::GraphicsTexturePtr& texture) noexcept
		{
			assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube);
			_skySpecularIBL = texture;
		}

		const hal::GraphicsTexturePtr&
		Light::getSkyLightingSpecular() const noexcept
		{
			return _skySpecularIBL;
		}

		bool
		Light::setupShadowMap() noexcept
		{
			auto shadowCamera = std::make_shared<PerspectiveCamera>();
			shadowCamera->setOwnerListener(this);
			shadowCamera->setCameraType(CameraType::Custom);
			shadowCamera->setAperture(90.0f);
			shadowCamera->setNear(0.1f);
			shadowCamera->setRatio(1.0f);

			return true;
		}

		void
		Light::destroyShadowMap() noexcept
		{
			if (_shadowCamera)
				_shadowCamera->setFramebuffer(nullptr);
		}

		void
		Light::destroyReflectiveShadowMap() noexcept
		{
			if (_shadowCamera)
				_shadowCamera->setFramebuffer(nullptr);
		}

		void
		Light::_updateTransform() noexcept
		{
			if (_shadowCamera)
				_shadowCamera->setTransform(this->getTransform(), this->getTransformInverse());
		}

		void
		Light::_updateBoundingBox() noexcept
		{
			math::BoundingBox boundingBox;

			/*if (!_shadowCamera)
			{
				math::Vector3 min(-_lightRange, -_lightRange, -_lightRange);
				math::Vector3 max(_lightRange, _lightRange, _lightRange);

				math::BoundingBox bound;
				bound.encapsulate(min);
				bound.encapsulate(max);

				boundingBox.encapsulate(bound);
			}
			else
			{
				if (_lightType == LightType::LightTypeAmbient || _lightType == LightType::LightTypeEnvironment)
				{
					math::Vector3 min(-_lightRange, -_lightRange, -_lightRange);
					math::Vector3 max(_lightRange, _lightRange, _lightRange);

					math::BoundingBox bound;
					bound.encapsulate(min);
					bound.encapsulate(max);

					boundingBox.encapsulate(bound);
				}
				else
				{
					float znear = _shadowCamera->getNear();
					float zfar = _lightRange;

					math::float3 corners[8];
					corners[0].set(-znear, +znear, znear);
					corners[1].set(+znear, +znear, znear);
					corners[2].set(-znear, -znear, znear);
					corners[3].set(+znear, -znear, znear);
					corners[4].set(-zfar, +zfar, zfar);
					corners[5].set(+zfar, +zfar, zfar);
					corners[6].set(-zfar, -zfar, zfar);
					corners[7].set(+zfar, -zfar, zfar);

					math::BoundingBox bound;
					bound.encapsulate(corners, 8);
					bound.transform((math::float3x3)_shadowCamera->getTransform());

					boundingBox.encapsulate(bound);

					if (_lightType == LightType::LightTypeSun || _lightType == LightType::LightTypeDirectional)
					{
						float w = bound.size().x * 0.5f;
						float h = bound.size().y * 0.5f;

						_shadowCamera->setOrtho(float4(-w, w, -h, h));
						_shadowCamera->setFar(zfar);
						_shadowCamera->setCameraType(CameraType::CameraTypeOrtho);
					}
					else if (_lightType == LightType::LightTypeSpot)
					{
						_shadowCamera->setAperture(this->getSpotOuterCone().x * 2);
						_shadowCamera->setFar(zfar);
						_shadowCamera->setCameraType(CameraType::CameraTypePerspective);
					}
					else if (_lightType == LightType::LightTypePoint)
					{
						_shadowCamera->setAperture(90.0f);
						_shadowCamera->setCameraType(CameraType::CameraTypePerspective);
					}
				}
			}

			this->setBoundingBox(boundingBox);*/
		}

		void
		Light::onSceneChangeBefore() noexcept
		{
		}

		void
		Light::onSceneChangeAfter() noexcept
		{
		}

		void
		Light::onRenderObjectPre(const Camera& camera) noexcept
		{
		}

		void
		Light::onRenderObjectPost(const Camera& camera) noexcept
		{
		}

		void
		Light::onMoveAfter() noexcept
		{
			_updateTransform();
		}

		RenderObjectPtr
		Light::clone() const noexcept
		{
			auto light = std::make_shared<Light>();
			light->setLightType(this->getLightType());
			light->setLightColor(this->getLightColor());
			light->setLightIntensity(this->getLightIntensity());
			light->setLightRange(this->getLightRange());
			light->setTransform(this->getTransform(), this->getTransformInverse());
			light->setBoundingBox(this->getBoundingBox());

			light->_spotInnerCone = _spotInnerCone;
			light->_spotOuterCone = _spotOuterCone;

			return light;
		}
	}
}