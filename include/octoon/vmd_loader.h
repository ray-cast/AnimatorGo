#ifndef OCTOON_VMD_LOADER_H_
#define OCTOON_VMD_LOADER_H_

#include <octoon/io/iostream.h>
#include <octoon/animation/animation.h>

namespace octoon
{
	class OCTOON_EXPORT VMDLoader final
	{
	public:
		VMDLoader() noexcept;
		~VMDLoader() noexcept;

		static bool doCanRead(io::istream& stream) noexcept;
		static bool doCanRead(const char* type) noexcept;

		static Animation<float> load(io::istream& stream) noexcept(false);
		static void save(io::ostream& stream, const Animation<float>& animation) noexcept(false);

	private:
		VMDLoader(const VMDLoader&) = delete;
		VMDLoader& operator=(const VMDLoader&) = delete;
	};
}

#endif