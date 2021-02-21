#ifndef OCTOON_AMBIENT_LIGHT_H_
#define OCTOON_AMBIENT_LIGHT_H_

#include <octoon/light/light.h>

namespace octoon
{
	class OCTOON_EXPORT AmbientLight final : public Light
	{
		OctoonDeclareSubClass(AmbientLight, Light)
	public:
		AmbientLight() noexcept;
		virtual ~AmbientLight() noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		AmbientLight(const AmbientLight&) noexcept = delete;
		AmbientLight& operator=(const AmbientLight&) noexcept = delete;
	};
}

#endif