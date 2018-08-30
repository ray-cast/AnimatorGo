#ifndef OCTOON_MODEL_LOADER_H_
#define OCTOON_MODEL_LOADER_H_

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

			virtual bool doCanRead(istream& stream) const noexcept = 0;
			virtual bool doCanRead(const std::string& type) const noexcept = 0;
			virtual bool doCanRead(const char* type) const noexcept = 0;

			virtual bool doLoad(istream& stream, class Model& model) noexcept = 0;
			virtual bool doSave(ostream& stream, const class Model& model) noexcept = 0;

		private:
			ModelLoader(const ModelLoader&) = delete;
			ModelLoader& operator=(const ModelLoader&) = delete;
		};
	}
}

#endif