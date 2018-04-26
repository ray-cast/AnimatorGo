#ifndef OCTOON_PMX_LOADER_H_
#define OCTOON_PMX_LOADER_H_

#include <octoon/model/loader.h>
#include <octoon/model/pmx.h>

namespace octoon
{
	namespace model
	{
		class PmxLoader : public ModelLoader
		{
		public:
			PmxLoader() = default;
			~PmxLoader() = default;

			bool doCanLoad(io::istream& stream) noexcept;
			bool doCanLoad(const std::string& type) noexcept;
			bool doCanLoad(const char* type) noexcept;

			bool doLoad(io::istream& stream, Pmx& pmx) noexcept;
			bool doLoad(io::istream& stream, Model& model) noexcept;

			bool doSave(io::ostream& stream, const Pmx& pmx) noexcept;
			bool doSave(io::ostream& stream, const Model& model) noexcept;
		};
	}
}
#endif // !OCTOON_PMX_LOADER_H_

