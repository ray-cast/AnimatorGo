#ifndef OCTOON_TUBE_LIGHT_H_
#define OCTOON_TUBE_LIGHT_H_

#include <octoon/light/light.h>

namespace octoon
{
	class OCTOON_EXPORT TubeLight final : public Light
	{
		OctoonDeclareSubClass(TubeLight, Light)
	public:
		TubeLight() noexcept;
		virtual ~TubeLight() noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		TubeLight(const TubeLight&) noexcept = delete;
		TubeLight& operator=(const TubeLight&) noexcept = delete;
	};
}

#endif