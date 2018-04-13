#ifndef OCTOON_MATERIAL_H_
#define OCTOON_MATERIAL_H_

#include <octoon/video/render_types.h>

namespace octoon
{
	class OCTOON_EXPORT Material final
	{
	public:
		Material() noexcept;
		~Material() noexcept;

	private:
		Material(const Material&) = delete;
		Material& operator=(const Material&) = delete;
	};
}

#endif