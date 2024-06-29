#ifndef OCTOON_ENVIRONMENT_LIGHT_H_
#define OCTOON_ENVIRONMENT_LIGHT_H_

#include <octoon/light/light.h>
#include <octoon/texture/texture.h>

namespace octoon
{
	class OCTOON_EXPORT EnvironmentLight final : public Light
	{
		OctoonDeclareSubClass(EnvironmentLight, Light)
	public:
		EnvironmentLight() noexcept;
		virtual ~EnvironmentLight() noexcept;

		void setOffset(const math::float2& offset) noexcept;
		const math::float2& getOffset() const noexcept;

		void setShowBackground(bool show) noexcept;
		bool getShowBackground() const noexcept;

		void setBackgroundMap(const std::shared_ptr<Texture>& texture) noexcept;
		const std::shared_ptr<Texture>& getBackgroundMap() const noexcept;

		void setEnvironmentMap(const std::shared_ptr<Texture>& texture) noexcept;
		const std::shared_ptr<Texture>& getEnvironmentMap() const noexcept;

		std::shared_ptr<RenderObject> clone() const noexcept;

	private:
		EnvironmentLight(const EnvironmentLight&) noexcept = delete;
		EnvironmentLight& operator=(const EnvironmentLight&) noexcept = delete;

	private:
		bool showBackground_;
		math::float2 offset_;
		std::shared_ptr<Texture> radiance_;
		std::shared_ptr<Texture> backgroundMap_;
		std::shared_ptr<Texture> environmentMap_;
	};
}

#endif