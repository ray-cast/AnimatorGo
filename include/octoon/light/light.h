#ifndef OCTOON_LIGHT_H_
#define OCTOON_LIGHT_H_

#include <octoon/video/render_object.h>

namespace octoon::light
{
	class OCTOON_EXPORT Light : public video::RenderObject, public video::RenderListener
	{
		OctoonDeclareSubClass(Light, video::RenderObject)
	public:
		Light() noexcept;
		virtual ~Light() noexcept;

		void setLightRange(float range) noexcept;
		void setLightIntensity(float intensity) noexcept;
		void setLightColor(const math::float3& color) noexcept;

		float getLightRange() const noexcept;
		float getLightIntensity() const noexcept;

		const math::float3& getLightColor() const noexcept;

		video::RenderObjectPtr clone() const noexcept;

	private:
		void onPreRender(const camera::Camera& camera) noexcept;
		void onPostRender(const camera::Camera& camera) noexcept;

	private:
		Light(const Light&) noexcept = delete;
		Light& operator=(const Light&) noexcept = delete;

	private:
		float _lightRange;
		float _lightIntensity;

		math::float3 _lightColor;
	};
}

#endif