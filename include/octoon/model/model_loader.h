#ifndef OCTOON_LOADER_H_
#define OCTOON_LOADER_H_

#include <octoon/model/modtypes.h>

#include <string>

namespace octoon
{
	namespace model
	{
		class ModelLoader
		{
		public:
			ModelLoader() = default;
			virtual ~ModelLoader() noexcept = default;

			virtual bool doCanLoad(istream& stream) noexcept = 0;
			virtual bool doCanLoad(const std::string& type) noexcept = 0;
			virtual bool doCanLoad(const char* type) noexcept = 0;
			virtual bool doLoad(istream& stream, class Model& model) noexcept = 0;
			virtual bool doSave(ostream& stream, const class Model& model) noexcept = 0;

		private:
			ModelLoader(const ModelLoader&) = delete;
			ModelLoader& operator=(const ModelLoader&) = delete;
		};
	}
}

#endif