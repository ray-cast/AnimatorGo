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

		void setRange(float range) noexcept;
		void setIntensity(float intensity) noexcept;
		void setColor(const math::float3& color) noexcept;

		float getRange() const noexcept;
		float getIntensity() const noexcept;

		const math::float3& getColor() const noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		void onPreRender(const camera::Camera& camera) noexcept;
		void onPostRender(const camera::Camera& camera) noexcept;

	private:
		Light(const Light&) noexcept = delete;
		Light& operator=(const Light&) noexcept = delete;

	private:
		float lightRange_;
		float lightIntensity_;

		math::float3 lightColor_;
	};
}

#endif