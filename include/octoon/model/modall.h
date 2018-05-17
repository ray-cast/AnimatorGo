#ifndef OCTOON_MODALL_H_
#define OCTOON_MODALL_H_

#include <octoon/model/model_loader.h>

#define OCTOON_BUILD_PMX_MODEL 1

namespace octoon
{
	namespace model
	{
		void addModelLoaderFor(Model& model);
	}
}

#endif