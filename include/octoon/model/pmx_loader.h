#ifndef OCTOON_PMX_LOADER_H_
#define OCTOON_PMX_LOADER_H_

#include <octoon/model/model_loader.h>
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

			bool doCanRead(istream& stream) const noexcept;
			bool doCanRead(const std::string& type) const noexcept;
			bool doCanRead(const char* type) const noexcept;

			bool doLoad(istream& stream, Pmx& pmx) noexcept;
			bool doLoad(istream& stream, Model& model) noexcept;

			bool doSave(ostream& stream, const Pmx& pmx) noexcept;
			bool doSave(ostream& stream, const Model& model) noexcept;
		};
	}
}

#endif