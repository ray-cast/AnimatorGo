#ifndef OCTOON_MODALL_H_
#define OCTOON_MODALL_H_

#include <octoon/model/model_loader.h>

#define OCTOON_BUILD_PMX_MODEL 1

namespace octoon
{
	namespace model
	{
		OCTOON_EXPORT bool emptyLoader() noexcept;
		OCTOON_EXPORT bool addHandler(ModelLoaderPtr&& handler) noexcept;
		OCTOON_EXPORT bool addHandler(const ModelLoaderPtr& handler) noexcept;
		OCTOON_EXPORT bool removeHandler(const ModelLoaderPtr& handler) noexcept;

		OCTOON_EXPORT ModelLoaderPtr findHandler(const char* type) noexcept;
		OCTOON_EXPORT ModelLoaderPtr findHandler(istream& stream) noexcept;
		OCTOON_EXPORT ModelLoaderPtr findHandler(istream& stream, const char* type) noexcept;
	}
}

#endif