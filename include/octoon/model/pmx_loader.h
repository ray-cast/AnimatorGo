#ifndef OCTOON_PMX_LOADER_H_
#define OCTOON_PMX_LOADER_H_

#include <octoon/model/loader.h>

namespace octoon
{
	namespace model
	{
		class PmxLoader : public ModelLoader
		{
		public:
			PmxLoader() = default;
			~PmxLoader() = default;

			bool doCanLoad(octoon::io::istream& stream) noexcept;
			bool doCanLoad(const std::string& type) noexcept;
			bool doCanLoad(const char* type) noexcept;

			bool doLoad(octoon::io::istream& stream, Pmx& pmx) noexcept;
			bool doLoad(octoon::io::istream& stream, Model& model) noexcept;

			bool doSave(octoon::io::ostream& stream, const Pmx& pmx) noexcept;
			bool doSave(octoon::io::ostream& stream, const Model& model) noexcept;
		};
	}
}
#endif // !OCTOON_PMX_LOADER_H_

