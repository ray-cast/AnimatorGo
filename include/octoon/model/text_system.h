#ifndef OCTOON_MODEL_TEXT_SYSTEM_H_
#define OCTOON_MODEL_TEXT_SYSTEM_H_

#include <octoon/runtime/singleton.h>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT TextSystem final
		{
			OctoonDeclareSingleton(TextSystem)
		public:
			TextSystem() noexcept;
			~TextSystem() noexcept;

			void setup() noexcept(false);
			void close() noexcept;

		private:
			friend class TextFile;
			friend class EntityObject;
			void* getLibrary() const noexcept;

		private:
			TextSystem(const TextSystem&) = delete;
			TextSystem& operator=(const TextSystem&) = delete;

		private:
			void* library_;
		};
	}
}

#endif