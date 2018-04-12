#include <octoon/model/modall.h>

namespace octoon
{
	namespace model
	{
#if OCTOON_BUILD_PMX_MODEL
		std::shared_ptr<ModelLoader> pmxLoader = std::make_shared<PmxLoader>();
#endif

		void addModelLoaderFor(Model& model)
		{
#if OCTOON_BUILD_PMX_MODEL
			model.addLoader(pmxLoader);
#endif
		}
	}
}