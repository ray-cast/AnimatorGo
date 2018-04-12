#ifndef OCTOON_MODALL_H_
#define OCTOON_MODALL_H_

#include <octoon/model/moddef.h>
#include <octoon/model/pmx_loader.h>
#include <octoon/model/model.h>

namespace octoon
{
	namespace model
	{
		void addModelLoaderFor(Model& model);
	}
}
#endif // !OCTOON_MODALL_H_
