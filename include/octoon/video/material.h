#ifndef OCTOON_MATERIAL_H_
#define OCTOON_MATERIAL_H_

#include <octoon/video/render_types.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT Material
		{
		public:
			Material() noexcept;
			~Material() noexcept;

			virtual MaterialPtr clone() const noexcept;

		private:
			Material(const Material&) = delete;
			Material& operator=(const Material&) = delete;
		};
	}
}

#endif