#ifndef OCTOON_MODEL_TEXT_FILE_H_
#define OCTOON_MODEL_TEXT_FILE_H_

#include <string>

#include <octoon/model/modtypes.h>

namespace octoon
{
	class TextMeshingComponent;

	namespace model
	{
		class OCTOON_EXPORT TextFile final
		{
		public:
			TextFile() noexcept;
			TextFile(const char* fontpath) noexcept(false);
			TextFile(const std::uint8_t* stream, std::size_t size) noexcept(false);
			~TextFile() noexcept;

			void open(const char* fontpath) noexcept(false);
			void open(const std::uint8_t* stream, std::size_t size) noexcept(false);
			void close() noexcept;

			const std::string& getFontPath() const noexcept;

			void* getFont() const noexcept;

		private:
			TextFile(const TextFile&) = delete;
			TextFile& operator=(const TextFile&) = delete;

		private:
			void* font_;
			std::string fontpath_;
		};
	}
}

#endif