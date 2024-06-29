#ifndef OCTOON_MODEL_TEXT_SYSTEM_H_
#define OCTOON_MODEL_TEXT_SYSTEM_H_

#include <octoon/runtime/singleton.h>

namespace octoon::font
{
	class OCTOON_EXPORT FontSystem final
	{
		OctoonDeclareSingleton(FontSystem)
	public:
		FontSystem() noexcept;
		~FontSystem() noexcept;

		void setup() noexcept(false);
		void close() noexcept;

	private:
		friend class Font;
		friend class EntityObject;
		void* getLibrary() const noexcept;

	private:
		FontSystem(const FontSystem&) = delete;
		FontSystem& operator=(const FontSystem&) = delete;

	private:
		void* library_;
	};
}

#endif