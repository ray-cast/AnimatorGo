#ifndef OCTOON_LIGHT_H_
#define OCTOON_LIGHT_H_

#include <octoon/video/render_object.h>

namespace octoon
{
	class OCTOON_EXPORT Light : public RenderObject, public RenderListener
	{
		OctoonDeclareSubClass(Light, RenderObject)
	public:
		Light() noexcept;
		virtual ~Light() noexcept;

		void setRange(float range) noexcept;
		void setSize(float size) noexcept;
		void setIntensity(float intensity) noexcept;
		void setColor(const math::float3& color) noexcept;

		float getSize() const noexcept;
		float getRange() const noexcept;
		float getIntensity() const noexcept;

		const math::float3& getColor() const noexcept;

		std::shared_ptr<RenderObject> clone() const noexcept;

	private:
		void onPreRender(const Camera& camera) noexcept;
		void onPostRender(const Camera& camera) noexcept;

	private:
		Light(const Light&) noexcept = delete;
		Light& operator=(const Light&) noexcept = delete;

	private:
		float size_;
		float range_;
		float intensity_;

		math::float3 color_;
	};
}

#endif